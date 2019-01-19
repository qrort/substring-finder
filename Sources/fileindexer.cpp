#include "../Headers/fileindexer.h"
#include <QFile>
#include <QThread>
#include <QDebug>

FileIndexer::FileIndexer(const QVector <QString> & files) {
    fileQuery = files;
}

FileIndexContainer FileIndexer::IndexFile(const QString & filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QSet <uint32_t> substrings;
        char buf[BUFFER_SIZE + SUBSTRING_SIZE - 1];
        bool prev = false;

        while (int len = static_cast<int>(file.read(buf + prev * (SUBSTRING_SIZE - 1), BUFFER_SIZE))) {
            for (int i = 0; i < len + prev * (SUBSTRING_SIZE - 1) - SUBSTRING_SIZE + 1; i++) {
                uint32_t substringHash = 0;
                for (int j = 0; j < SUBSTRING_SIZE; j++) {
                    substringHash += (static_cast<unsigned char>(buf[i + j]) << (1 << ((SUBSTRING_SIZE - j - 1) * 8)));
                }
                substrings.insert(substringHash);
            }
            if (substrings.size() > BINARY_FILE_BOUND) {
                emit FileIndexed();
                return FileIndexContainer();
            }
            std::copy(buf + len - SUBSTRING_SIZE + 1, buf + len, buf);
            prev = true;
        }
        emit FileIndexed();
        return std::move(FileIndexContainer(filePath, substrings));
    } else {
        emit FileIndexed();
        return FileIndexContainer();
    }
}

void FileIndexer::IndexFiles() {
    QVector <FileIndexContainer> result;
    for (auto& filePath : fileQuery) {
        if (QThread::currentThread()->isInterruptionRequested()) return;
        result.push_back(IndexFile(filePath));
    }
    emit Done(result);
}
