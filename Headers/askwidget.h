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
    explicit AskWidget(DirectoryIndex const& _data, int entries, int changed, QWidget *parent = 0);
    ~AskWidget();
    Ui::AskWidget *ui;
public slots:
    void searchSubstrings();
    void updateProgress(QString);
    void checkZero();
    void updateChanged();
    void sortEntries();
signals:
    void Done();
    void FileProcessed(QString);
private slots:
    void on_query_returnPressed();

private:
    QVector <QThread *> threads;
    DirectoryIndex data;
    int progress, entriesCount, changedCount;
};

#endif // ASKWIDGET_H
