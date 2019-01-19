#include "../Headers/indexer.h"
#include "../Headers/fileindexer.h"
#include <QDirIterator>
#include <QThread>
#include <QFileInfo>

Indexer::Indexer(const QDir &_dir) {
    dir = _dir;
    qRegisterMetaType<DirectoryIndex>("DirectoryIndex");
}

bool Indexer::isOpenable(const QFileInfo & file) {
    if (file.isFile()) {
        if (!(QFile::permissions(file.filePath()) & QFile::ReadUser)) {
           emit log("Do not have permission for opening" + file.filePath());
           return false;
        }
        if (!file.isReadable()) {
           emit log(file.filePath() + "is not readable");
           return false;
        }
        return true;
    } else return false;
}

void Indexer::updateProgressBar() {
    emit FileIndexed();
}

void Indexer::IndexDirectory() {

    QDirIterator it(dir.absolutePath(), QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);

    QVector <QString> files;

    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()) return;

        QFileInfo fileInfo(it.next());
        if (fileInfo.isFile()) {
            if (isOpenable(fileInfo)) {
                files.push_back(fileInfo.absoluteFilePath());
            } else emit FileIndexed();
        }
    }
    FileIndexer *fileIndexer = new FileIndexer(files);

    connect(fileIndexer, &FileIndexer::FileIndexed, this, &Indexer::updateProgressBar);
    connect(fileIndexer, &FileIndexer::Done, this, &Indexer::Done);
    connect(fileIndexer, &FileIndexer::Done, fileIndexer, &FileIndexer::deleteLater);
    fileIndexer->IndexFiles();
}
