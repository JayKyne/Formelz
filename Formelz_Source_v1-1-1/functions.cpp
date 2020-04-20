#include "functions.h"

//==========
//contains some common functions
//==========

functions::functions()
{
}

//just print a list
void functions::printList(QList<QList<QString>> list)
{
    QString string;
    for (int k = 0; k < list.size(); k ++) {
        for (int i = 0; i < list[0].size(); i++) {
            string += list[k][i] + "   ";
            if (i == list[0].size()) {
                string += "\n";
            }
        }
    }
    qDebug() << string;
}

//get the listplace of an item with its belonging id
int functions::getPlace(QString id, QList<QList<QString>> list)
{
    for (int j = 0; j < list.size(); j++) {
        if (id == list[j][0]) {
            return j;
        }
    }
    return 0;
}

//find a new which is not used now
int functions::findUnusedId(QList<QList<QString>> list)
{
    list = sort(list, 0, "int");

    for (int i = 0; i < list.size(); i++) {
        if (list[i][0] != QString::number(i)) {
            return i;
        }
    }

    return list.size();
}

//reverse list
QList<QList<QString>> functions::reverseList(QList<QList<QString>> list)
{
    QList<QList<QString>> listReversed;
    for (int i = 0; i < list.size(); i++) {
        listReversed.append(list[list.size()-i-1]);
    }
    return listReversed;
}

//convert char to ascii integer
int functions::convertToAscii(QChar chr)
{
    QString str = QString::number(chr.unicode(), 10);
    int integer = str.toInt();
    if (integer <= 90) {
        integer += 32;
    }
    return  integer;
}

//convert date to integer
int functions::convertDateToInt(QString str)
{
    QString dateString;
    int dateInt;
    dateString = QString(str[6]) + QString(str[7]) + QString(str[3]) + QString(str[4]) + QString(str[0]) + QString(str[1]);
    dateInt = dateString.toInt();
    return dateInt;
}


//sort list
QList<QList<QString>> functions::sort(QList<QList<QString>> list, int place, QString type) //does not function
{
    QList<QList<QString>> sortedList;

    if (type == "str") {
        QMap<QString, QList<QString>> map;
        foreach (const QList<QString> &str, list)
            map.insert(str[place].toLower(), str);
        sortedList = map.values();
    } else {
        QMap<int, QList<QString>> mapInt;
        foreach (const QList<QString> &str, list)
            if (type == "int") {
                mapInt.insert(str[place].toInt(), str);
            } else {
                mapInt.insert(convertDateToInt(str[place]), str);
            }
        sortedList = mapInt.values();
    }

    //if items weren't unique and sortalgorith lost some
    int lenght = sortedList.size();
    if (lenght < list.size()) {
        //make a list with the unique items
        QList<QString> uniqueItems;
        for (int i = 0; i < lenght; i++) {
            uniqueItems.append(sortedList[i][place]);
        }

        //create sublists which include all items for each unique item
        QList<QList<QString>> subLists[lenght];
        for (int i = 0; i < lenght; i++) {
            for (int j = 0; j < list.size(); j++) {
                if (list[j][place] == uniqueItems[i]) {
                    subLists[i].append(list[j]);
                }
            }
        }

        //sort the sublists and merge them together
        sortedList.clear();
        for (int i = 0; i < lenght; i++) {
            QMap<QString, QList<QString>> subMap;
            foreach (const QList<QString> &str, subLists[i])
                subMap.insert(str[1].toLower(), str);
            sortedList.append(subMap.values());
        }
    }
    return sortedList;
}



//get current date
QString functions::getDate()
{
    QString date = QDateTime::currentDateTime().toString("dd.MM.yy");
    return date;
}

//
bool functions::inList(QString str, QList<QString> list)
{
    for (int i = 0; i < list.size(); i++) {
        if (str == list[i]) {
            return true;
        }
    }
    return false;
}

//convert string to list
QList<QString> functions::stringToList(QString str)
{
    QString item;
    QList<QString> list;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] == ",") {
            list.append(item);
            item.clear();
        } else {
            item += str[i];
        }
    }
    list.append(item);
    return list;
}
