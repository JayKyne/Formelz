#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QStandardItemModel>
#include <QDebug>
#include <QDateTime>

class functions
{
public:
    functions();

    void printList(QList<QList<QString>> list);
    int getPlace(QString id, QList<QList<QString>> list);
    int findUnusedId(QList<QList<QString>> list);
    QList<QList<QString>> reverseList(QList<QList<QString>> list);
    int convertToAscii(QChar letter);
    int convertDateToInt(QString str);
    QList<QList<QString>> sort(QList<QList<QString>> list, int place, QString type);
    QString getDate();
    bool inList(QString str, QList<QString> list);
    QList<QString> stringToList(QString str);
};

#endif // FUNCTIONS_H
