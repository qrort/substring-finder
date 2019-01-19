#include "../Headers/mainwindow.h"
#include "ui_mainwindow.h"
#include "../Headers/askwidget.h"
#include "ui_askwidget.h"
#include "../Headers/indexer.h"
#include <QCommonStyle>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QProgressDialog>
#include <QTreeView>
#include <QDebug>
#include <QListWidgetItem>

void MainWindow::set_selected_directory(const QDir & dir) {
    selected_directory = dir;
    ui->lineEdit->setText("Selected directory: " + dir.absolutePath());
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, 3 * size() / 2, qApp->desktop()->availableGeometry()));
    model = new QFileSystemModel(this);
    model->setRootPath(QDir::homePath());

    indexing_thread = nullptr;
    errors = progress = 0;
    ui->progressBar->setValue(0);
    ui->treeView->setModel(model);
    ui->treeView->setColumnWidth(0, 320);
    ui->treeView->setRootIndex(model->index(QDir::homePath()));
    set_selected_directory(QDir::home());

    qRegisterMetaType<DirectoryIndex>("DirectoryIndex");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selectButton_clicked()
{



    QString dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                        selected_directory.absolutePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        set_selected_directory(QDir(dir));
         ui->treeView->setRootIndex(model->index(dir));
    }
}

void MainWindow::update_progress() {
    ui->progressBar->setValue(++progress);
    if (ui->progressBar->isMaximized()) {
        progress = 0;
        ui->progressBar->setValue(0);
    }
}

void MainWindow::reset_progress() {
    indexing_thread->quit();
    indexing_thread->wait();
    progress = 0;
    ui->progressBar->setValue(0);
    delete indexing_thread;
    indexing_thread = nullptr;
}

void MainWindow::ask(DirectoryIndex index) {
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    qDebug() << QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()) +
                "ms";
    AskWidget *askWidget = new AskWidget(index, files_count);
    reset_progress();
    askWidget->show();
}

int MainWindow::count() {
    int result = 0;
    QDirIterator it(selected_directory.absolutePath(), QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo file_info(it.next());
        if (file_info.isFile()) result++;
    }
    return result;
}

void MainWindow::list_error(QString message) {
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(message);
    ui->errorList->insertItem(errors++, newItem);
}

void MainWindow::on_scanButton_clicked()
{
    if (indexing_thread == nullptr) {

        begin = std::chrono::steady_clock::now();

        files_count = count();
        ui->progressBar->setMaximum(files_count);
        progress = 0;

        indexing_thread = new QThread;
        Indexer *indexer = new Indexer(selected_directory);
        indexer->moveToThread(indexing_thread);

        connect(indexer, &Indexer::FileIndexed, this, &MainWindow::update_progress);
        connect(indexer, &Indexer::Done, this, &MainWindow::ask);
        connect(indexer, &Indexer::Done, indexer, &Indexer::deleteLater);
        connect(indexer, &Indexer::log, this, &MainWindow::list_error);
        connect(indexing_thread, &QThread::started, indexer, &Indexer::IndexDirectory);
        indexing_thread->start();
    }
}

void MainWindow::on_cancelButton_clicked()
{
    if (indexing_thread != nullptr && indexing_thread->isRunning()) {
        indexing_thread->requestInterruption();
        reset_progress();
    }
}

