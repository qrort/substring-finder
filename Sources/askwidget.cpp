#include "../Headers/askwidget.h"
#include "ui_askwidget.h"
#include "../Headers/fileindexer.h"
#include "../Headers/searcher.h"
#include <memory>
#include <QDesktopWidget>
#include <QFile>
#include <thread>
#include <QThread>
#include <QMessageBox>
#include <QDebug>

void AskWidget::updateChanged() {
    ui->warn->setText(QString::number(++changedCount) + " directories are changed during search.");
}

AskWidget::AskWidget(DirectoryIndex const& _data, int entries, int changed, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AskWidget)
{
    progress = 0;
    ui->setupUi(this);
    ui->progressBar->setMaximum(entries);
    ui->progressBar->setValue(0);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, 3 * size() / 2, qApp->desktop()->availableGeometry()));
    data = _data;
    changedCount = changed - 1;
    updateChanged();
    setWindowTitle("Search");
    connect(ui->searchButton, &QPushButton::clicked, this, &AskWidget::searchSubstrings);
    connect(ui->query, &QLineEdit::returnPressed, this, &AskWidget::searchSubstrings);
    connect(ui->sortButton, &QPushButton::clicked, this, &AskWidget::sortEntries);
    connect(this, &AskWidget::FileProcessed, this, &AskWidget::updateProgress);
    qRegisterMetaType<QString>("QString");
}

void AskWidget::sortEntries() {
    QVector <QString> items;
    for (int i = 0; i < ui->listWidget->count(); i++) {
        items.push_back(ui->listWidget->item(i)->text());
    }
    ui->listWidget->clear();
    entriesCount = 0;
    qSort(items.begin(), items.end());
    for (QString path : items) {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(path);
        ui->listWidget->insertItem(entriesCount++, newItem);
    }
}

void AskWidget::updateProgress(QString entryPath) {

    ui->progressBar->setValue(++progress);
    if (!entryPath.isEmpty()) {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(entryPath);
        ui->listWidget->insertItem(entriesCount++, newItem);
    }
    if (progress == ui->progressBar->maximum()) {
        emit Done();
       // QMessageBox::information(this, QString("Done"),
       //                              "GJ",
       //                               QMessageBox::Ok);
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
    entriesCount = progress = 0;
    ui->progressBar->setValue(0);
    if (ui->query->text().size() >= FileIndexer::SUBSTRING_SIZE) {
        ui->listWidget->clear();
        int threadsCount = std::thread::hardware_concurrency();
        threadsCount = std::min(threadsCount, (int)data.size());
        threadsCount = 1;
        QVector <int> bounds(1, 0);
        for (int i = 0;i < threadsCount - 1; i++) bounds.push_back(bounds.back() + (int)data.size() / threadsCount);
        bounds.push_back(data.size());
        connect(this, &AskWidget::Done, this, &AskWidget::checkZero);
        for (int i = 0; i < threadsCount; i++) {
            QThread *thread = new QThread;
            Searcher *searcher = new Searcher(&data, ui->query->text(), bounds[i], bounds[i + 1]);
            searcher->moveToThread(thread);
            connect(searcher, &Searcher::FileProcessed, this, &AskWidget::updateProgress);
            connect(searcher, &Searcher::Done, thread, &QThread::quit);
            connect(searcher, &Searcher::Done, searcher, &Searcher::deleteLater);
            connect(thread, &QThread::started, searcher, &Searcher::DoWork);
            threads.push_back(thread);
            thread->start();
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

void AskWidget::on_query_returnPressed()
{

}
