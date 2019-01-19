#include "../Headers/askwidget.h"
#include "ui_askwidget.h"
#include "../Headers/fileindexer.h"
#include "../Headers/searcher.h"
#include <memory>
#include <QDesktopWidget>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

void AskWidget::updateChanged() {
    ui->warn->setText(QString::number(++changedCount) + " files are changed during search.");
}

AskWidget::AskWidget(DirectoryIndex const& _data, int entries, int changed, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AskWidget)
{
    progress = 0;
    worker_thread = nullptr;
    ui->setupUi(this);
    ui->progressBar->setMaximum(entries);
    ui->progressBar->setValue(0);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, 3 * size() / 2, qApp->desktop()->availableGeometry()));
    data = _data;
    changedCount = changed - 1;
    updateChanged();
    setWindowTitle("Search");
    connect(ui->searchButton, &QPushButton::clicked, this, &AskWidget::searchSubstrings);
    connect(this, &AskWidget::FileProcessed, this, &AskWidget::updateProgress);
    qRegisterMetaType<QString>("QString");
}

void AskWidget::updateProgress(QString entryPath) {

    ui->progressBar->setValue(++progress);
    if (ui->progressBar->isMaximized()) {
        progress = 0;
        ui->progressBar->setValue(0);
    }
    if (!entryPath.isEmpty()) {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(entryPath);
        ui->listWidget->insertItem(entriesCount++, newItem);
    }
}

void AskWidget::deleteThread() {
    if (worker_thread != nullptr) {
        worker_thread->quit();
        worker_thread->wait();
        delete worker_thread;
        worker_thread = nullptr;
    }
}

void AskWidget::checkZero() {
    if (entriesCount == 0) {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText("No files found.");
        ui->listWidget->insertItem(entriesCount++, newItem);
    }
}

void AskWidget::searchSubstrings() {
    entriesCount = 0;
    if (ui->query->text().size() >= FileIndexer::SUBSTRING_SIZE) {
        if (worker_thread == nullptr) {
            ui->listWidget->clear();
            progress = 0;
            ui->progressBar->setValue(0);
            worker_thread = new QThread;
            Searcher *searcher = new Searcher(&data, ui->query->text());
            searcher->moveToThread(worker_thread);

            connect(searcher, &Searcher::FileProcessed, this, &AskWidget::updateProgress);
            connect(searcher, &Searcher::Done, this, &AskWidget::deleteThread);
            connect(searcher, &Searcher::Done, this, &AskWidget::checkZero);
            connect(searcher, &Searcher::Done, searcher, &Searcher::deleteLater);
            connect(worker_thread, &QThread::started, searcher, &Searcher::DoWork);
            worker_thread->start();
        }
    } else {
        QMessageBox::warning(this, QString("Invalid query"),
                                     "Query length should exceed " + QString::number(FileIndexer::SUBSTRING_SIZE),
                                      QMessageBox::Ok);
    }
}

AskWidget::~AskWidget()
{
    delete ui;
}
