#include "../Headers/searcher.h"
#include "../Headers/fileindexer.h"
#include <QFile>

Searcher::Searcher(DirectoryIndex *_data, QString _query) {
    data = _data;
    query = _query.toStdString();
}

Searcher::~Searcher() {}

void Searcher::DoWork() {
    QVector <uint32_t> queryHashes;
    for (int i = 0; i < (int)query.size() - FileIndexer::SUBSTRING_SIZE + 1; i++) {
        uint32_t queryHash = 0;
        for (int j = 0; j < FileIndexer::SUBSTRING_SIZE; j++) {
            queryHash += (static_cast<unsigned char>(query[i + j]) <<
                          (1 << ((FileIndexer::SUBSTRING_SIZE - j - 1) * 8)));
        }
        queryHashes.push_back(queryHash);
    }

    for (auto& container : *data) {
        bool similar = true;
        for (auto& hash : queryHashes) {
           similar |= container.contains(hash);
        }
        if (similar) {
            bool in = false;
            QFile file(container.getFilePath());
            file.open(QIODevice::ReadOnly);
            std::string text = file.readAll().toStdString();
            for (int i = 0; i < (int)text.size() - (int)query.size() + 1; i++) {
                bool ok = true;
                for (int j = i; j < i + (int)query.size(); j++) ok &= (text[j] == query[j - i]);
                if (ok) {
                    in = true;
                    break;
                }
            }
            file.close();
            QString entryPath = "";
            if (in) {
                entryPath = container.getFilePath();
            }
            emit FileProcessed(entryPath);
        }
    }
    emit Done();
}
