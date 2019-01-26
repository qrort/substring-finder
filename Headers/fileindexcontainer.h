#ifndef FILEINDEXCONTAINER_H
#define FILEINDEXCONTAINER_H
#include <QString>
#include <QSet>
#include <QVector>
#include <QtAlgorithms>

class FileIndexContainer {
public:
    FileIndexContainer() : filePath(""), ready(false) {}
    FileIndexContainer(QString filePath, QSet <uint32_t> & set) : filePath(filePath) {
        for (auto el : set) substrings.push_back(el);
    }
    void insert(uint32_t substring_hash) {
        substrings.push_back(substring_hash);
    }
    bool contains(uint32_t substring_hash) {
        return (*qLowerBound(substrings.begin(), substrings.end(), substring_hash) == substring_hash);
    }
    size_t size() {
        return substrings.size();
    }
    void clear() {
        substrings.clear();
    }
    QString getFilePath() {
        return filePath;
    }
    bool isEmpty() {
        return substrings.isEmpty();
    }
    void prepare() {
        if (!ready) {
            ready = true;
            qSort(substrings.begin(), substrings.end());
        }
    }

private:
    QString filePath;
    QVector <uint32_t> substrings;
    bool ready;
};

using DirectoryIndex = QVector <FileIndexContainer>;

#endif
