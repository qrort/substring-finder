#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QThread>
#include "fileindexcontainer.h"
#include <chrono>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_selectButton_clicked();

    void on_scanButton_clicked();

    void ask(DirectoryIndex);

    void on_cancelButton_clicked();

    void update_progress();

    void update_changed();

    void list_error(QString);
private:
    std::chrono::steady_clock::time_point begin;
    Ui::MainWindow *ui;
    QFileSystemModel *model;
    QFileSystemWatcher *watcher;
    QDir selected_directory;
    QThread *indexing_thread;
    int progress, errors, files_count, changed_count;

    void show_directory();
    void set_selected_directory(const QDir & dir);
    void reset_progress();
    int count();
};

#endif // MAINWINDOW_H
