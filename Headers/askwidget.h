#ifndef ASKWIDGET_H
#define ASKWIDGET_H

#include <QWidget>
#include <QMap>
#include "fileindexcontainer.h"
#include <QThread>

namespace Ui {
class AskWidget;
}

class AskWidget : public QWidget {
    Q_OBJECT

public:
    explicit AskWidget(DirectoryIndex const& _data, int count, QWidget *parent = 0);
    ~AskWidget();
public slots:
    void searchSubstrings();
    void updateProgress(QString);
    void deleteThread();
    void checkZero();
signals:
    void FileProcessed(QString);
private:
    QThread *worker_thread;
    DirectoryIndex data;
    Ui::AskWidget *ui;
    int progress, entriesCount;
};

#endif // ASKWIDGET_H
