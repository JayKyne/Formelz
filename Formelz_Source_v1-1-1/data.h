#ifndef DATA_H
#define DATA_H

#include <functions.h>

#include <QStandardItemModel>
#include <QDir>
#include <cstring>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QSignalMapper>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDateTime>
#include <QLabel>
#include <QDialog>
#include <QDesktopWidget>
#include <QDebug>
#include <QDateTime>
#include <QDir>

class Data
{
public:
    Data();

    void save(QString v, QList<QList<QString>> list1, QList<QList<QString>> list2, QList<QList<QString>> list3);
    void saveSettings(QList<QString> list);

    QString readData(QString txt);
    void writeData(QString str, QString txt);

    QList<QList<QList<QString>>> convertReadData(QString data);
    QString convertWriteData(QList<QList<QList<QString>>> data);

    QList<QString> convertReadSettings(QString str);
    QString convertWriteSettings(QList<QString> list);

    QPixmap readDataPixmap(QString id);
    QPixmap readDataPixmapZeichen(QString id);
    void writeDataPixmap(QPixmap pix, QString id, QString subfolder);



    void backup();

private:
    functions *functions;

    QString installPath = QDir::currentPath();
};

#endif // DATA_H
