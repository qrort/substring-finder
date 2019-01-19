#ifndef SEARCHER_H
#define SEARCHER_H
#include <QObject>
#include "fileindexcontainer.h"

struct Searcher : public QObject {
    Q_OBJECT
public:
    explicit Searcher(DirectoryIndex *_data, QString query);
    ~Searcher();
public slots:
    void DoWork();
signals:
    void FileProcessed(QString);
    void Done();
private:
    DirectoryIndex *data;
    std::string query;
};

#endif //SEARCHER_H

