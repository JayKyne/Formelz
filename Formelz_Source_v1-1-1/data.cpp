#include "data.h"

//==========
//responsible for data management
//==========

Data::Data()
{
    functions = new class functions();
}

//save main date to datenbank textfile
void Data::save(QString v, QList<QList<QString>> list1, QList<QList<QString>> list2, QList<QList<QString>> list3)
{    
    //define "Time" to avoid crash
    if (list2.isEmpty()) {
        QList<QString> smallList;
        smallList << "0" << "t" << "Zeit" << "s" << "s";
        list2.append(smallList);
    }

    //write version into List
    QList<QString> vList;
    vList << "0" << v;
    QList<QList<QString>> list0;
    list0.append(vList);

    //sort lists and save to database
    QList<QList<QList<QString>>> database;
    database << list0 << functions->sort(list1,0, "int") << functions->sort(list2,0, "int") << functions->sort(list3,0, "int");
    QString str = convertWriteData(database);
    writeData(str, "/database.txt");
}

//save settings to settings textfile
void Data::saveSettings(QList<QString> list)
{
    QString str = convertWriteSettings(list);
    writeData(str, "/settings.txt");
}

//----------------------------------------------------------------------------------------------
//read data from a textfile
QString Data::readData(QString txt) //read textfile
{
    QString filePath;
    filePath = installPath + "/bin" + txt;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QTextStream in(&file);
    QString text = in.readAll();

    return text;
}

//write data to a textfile
void Data::writeData(QString str, QString txt)
{
    QString filePath = installPath + "/bin" + txt;
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream stream(&file);
        stream.reset();
        stream << str;
    }

}

//----------------------------------------------------------------------------------------------
//convert a raw string from textfile to a list
QList<QList<QList<QString>>> Data::convertReadData(QString data)
{
    QString text;
    QList<QString> item;
    QList<QList<QString>> singleList;
    QList<QList<QList<QString>>> wholeList;

    for (int i = 0; i < data.size(); i++) {
        if (data[i] == ">") {
            wholeList.append(singleList);

            singleList.clear();
        } else if (data[i] == "\n" && data[i-1] != ">") {
            item.append(text);
            singleList.append(item);

            text.clear();
            item.clear();
        } else if (data[i] == ";") {
            item.append(text);

            text.clear();
        } else if (data[i] != "\n") {
            text.append(data[i]);
        }
    }
    item.append(text);
    singleList.append(item);
    wholeList.append(singleList);

    return wholeList;

}

//convert a list to saveable string
QString Data::convertWriteData(QList<QList<QList<QString>>> data)
{
    QString text;
    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data[i].size(); j++) {
            for (int k = 0; k < data[i][j].size(); k++) {

                if (j == data[i].size()-1 && k == data[i][j].size()-1) {
                    text += data[i][j][k] + "\n>\n";

                } else if (k == data[i][j].size()-1) {
                    text += data[i][j][k] + "\n";

                } else {
                    text += data[i][j][k] + ";";
                }
            }
        }
    }
    text.remove(text.size()-3,3);
    return text;
}

//----------------------------------------------------------------------------------------------
//read formula PNGs from folder and convert to pixmap
QPixmap Data::readDataPixmap(QString id)
{
    QPixmap pix;
    QString filepath = installPath + "/img/formulas/";

    QFile file(filepath + id + ".png");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> pix;
    }
    return pix;
}

//read formula symbols PNGs from folder and convert to pixmap
QPixmap Data::readDataPixmapZeichen(QString id)
{
    QPixmap pix;
    QString filepath = installPath + "/img/formulasymbols/";
    QString list1 = "abcdefghijklmnopqrstuvwxyz";
    QString list2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (id.size() == 1) {
        if (list1.contains(id)) {
            id += "_small";
        } else {
            id += "_big";
        }
    } else {
        if (list1.contains(id[0])) {
            id += "_small";
        } else {
            id += "_big";
        }
    }
    QFile file(filepath + id + ".png");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> pix;
    }
    return pix;
}

//save pixmap in folder
void Data::writeDataPixmap(QPixmap pix, QString id, QString subfolder)
{
    QString filepath = installPath + "/img/" + subfolder;
    QFile file(filepath + id + ".png");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out << pix;
}

//----------------------------------------------------------------------------------------------
//convert settings-data-string to a list
QList<QString> Data::convertReadSettings(QString str)
{
    QString text;
    QList<QString> list;
    for (int i = 0; i < str.size(); i++) {
        if (str[i] != "\n") {
            text.append(str[i]);
        } else {
            list.append(text);
            text.clear();
        }
    }
    list.append(text);
    return list;
}

//convert settings-list to saveable string
QString Data::convertWriteSettings(QList<QString> list)
{
    QString str;
    for (int i = 0; i < list.size(); i++) {
        str.append(list[i] + "\n");
    }
    str.remove(str.size()-1,1);
    return str;
}

//----------------------------------------------------------------------------------------------
//make a backup file from the main data and save it to the backup-folder
void Data::backup()
{
    //aelteste Datei loeschen
    QString path = installPath + "/backup";
    QDir dir(path);
    QList<QString> list = dir.entryList();
    if (list.size() > 21) {
        dir.remove(list[2]);
    }

    //neue Datei erstellen
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HHmmss");
    QString filePath = installPath + "/backup/Formulas [" + time + "].txt";
    QFile file(filePath);
    QString text = readData("/database.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream stream(&file);
        stream.reset();
        stream << text;
    }
}
