#ifndef INDEXER_H
#define INDEXER_H
#include <QObject>
#include <QDir>
#include "fileindexcontainer.h"

class Indexer : public QObject {
    Q_OBJECT
public:
    explicit Indexer(const QDir & _dir);
public slots:
    void IndexDirectory();
    void updateProgressBar();
signals:
    void log(QString);
    void FileIndexed();
    void Done(DirectoryIndex);
private:
    QDir dir;
    bool isOpenable(const QFileInfo & file);
};

#endif //INDEXER_H
