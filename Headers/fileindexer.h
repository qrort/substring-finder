#ifndef FILEINDEXER_H
#define FILEINDEXER_H
#include <QObject>
#include "fileindexcontainer.h"

class FileIndexer : public QObject {
    Q_OBJECT
public:
    explicit FileIndexer(const QVector <QString> & files);
public slots:
    FileIndexContainer IndexFile(const QString & filePath);
    void IndexFiles();
signals:
    void FileIndexed();
    void Done(DirectoryIndex);
public:
    static uint32_t _hash(QVector <char> a);
    static const int BUFFER_SIZE = 1 << 12;
    static const int SUBSTRING_SIZE = 3; //if set to x > 4, appropriate changes to substring_hash type should be done
    static const int BINARY_FILE_BOUND = 30000;
private:
    QVector <QString> fileQuery;
};

#endif //FILETRIGRAMCOUNTER_H
