#-------------------------------------------------
#
# Project created by QtCreator 2019-01-18T16:15:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = substring-finder
TEMPLATE = app

SOURCES += Sources/main.cpp\
        Sources/mainwindow.cpp\
        Sources/fileindexer.cpp\
        Sources/indexer.cpp\
        Sources/askwidget.cpp\
        Sources/searcher.cpp

HEADERS  += Headers/mainwindow.h\
            Headers/fileindexer.h\
            Headers/indexer.h\
            Headers/fileindexcontainer.h\
            Headers/askwidget.h\
            Headers/searcher.h

FORMS    += Forms/mainwindow.ui\
            Forms/askwidget.ui
