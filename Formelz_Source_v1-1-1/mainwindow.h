#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <data.h>
#include <functions.h>
#include <klfbackend.h>
#include <klfpreviewbuilderthread.h>

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QClipboard>
#include <QCompleter>
#include <QCheckBox>
#include <QFile>
#include <QTextStream>
#include <QSignalMapper>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialog>
#include <QDesktopWidget>
#include <QDebug>
#include <QItemSelectionModel>
#include <QState>
#include <QKeyEvent>
#include <QLayout>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QPicture>
#include <QDir>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QDateTime>
#include <QStringListModel>
#include <QScroller>
#include <QPalette>

namespace Ui {
class MainWindow;
}

//---------------------------------------------------------------

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //GUI
    void setGui();


    //Formulas
    void formulaAddBegin(QString str1, QString str2, QString str3, QString str4, QString str5, QString str6);
    void formulaAddEnd();

    //Formulasymbols
    void formulasymbolsAddBegin();
    QList<QString> formulasymbolsSearch(QString formula);

    void formulasymbolConnect(QString id);
    void formulasymbolAddEnd();

    //Learning Mode


    //KLFBackend
    QPixmap latexRun(QString string, int size);
    void latexUpdateAllFormulasymbols();

    //Sonstiges
    void refreshEditButtons();
    void dialogWindow(QString text, QString title, int type);
    void save();
    void keyReleaseEvent(QKeyEvent *e);
    void p();




private:
    //Klassen
    Data *data;
    functions *functions;

    //Informationen
    QList<QList<QString>> visibleList;
    QList<QList<QString>> formulasList;
    QList<QList<QString>> formulasymbolsList;
    QList<QList<QString>> categoriesList;
    QList<QString> settingsData;
    QList<QString> latexCommandsList;
    QList<QString> greekList;
    QString programVersion;

    int resolution;
    QList<QString> headerList; //gleich weg
    QList<int> infoList;
    int maximumModelSize;
    QString semester;
    bool formulaNew;
    bool formulasymbolsGUIActive;
    bool formulasymbolsDefineActive;

    //GUI
    Ui::MainWindow *ui;
    int h,w;
    QDialog *dialog;
    QDialog *dialogSettings;
    QStandardItemModel *model;

    QLineEdit *nameEdit;
    QLineEdit *formulaEdit;
    QPushButton *previewButton;
    QLineEdit *descriptionEdit;
    QLineEdit *relevanceEdit;
    QLabel *displayLabel;
    QLineEdit *categoryEdit;
    QComboBox *subjectCombo;
    QCheckBox *formulasymbolDefineCheck;
    QPushButton *addButton;
    QPushButton *cancelButton;
    QPushButton *completeButton;

    QComboBox *zeichenFoundCombo;
    QComboBox *zeichenAvailableCombo;
    QPushButton *previousAuswahlButton;
    QPushButton *nextAuswahlButton;
    QPushButton *defineFormulasymbolsButton;
    QPushButton *notDefineFormulasymbolsButton;
    QLineEdit *formulasymbolsNameEdit;
    QLineEdit *einheitEdit;
    QLineEdit *grundeinheitEdit;
    bool filterGroupShow = false;
    bool editGroupShow = true;

    QLineEdit *editSettingsSemester;
    QLineEdit *editSettingsTable;
    QLineEdit *editSettingsLatex;
    QLineEdit *editSettingsMiktexPath;

    //Formulas
    QString currentFormulaId;
    QList<QString> formulaCache;

    //Formulasymbols
    QString selectedFormulasymbol;
    QString selectedFormulasymbolId;
    QList<QString> currentFormulasymbolId;
    bool formulasymbolExists;

    //KLFBackend
    KLFBackend::klfSettings settings;
    KLFBackend::klfInput input;
    QList<QPixmap> pixmapList;
    bool renderingNecessary;

    //Sonstiges
    QString installPath = QDir::currentPath();
    QCompleter *kategorieCompleter;
    QCompleter *latexCompleter;
    QString latestFormulaText;



private slots:
    //GUI
    void formulasymbolsGUI();
    void formulasGUI();
    void refreshDisplay();
    void settingsDialog();
    void dialogDestroy();
    void horizontalHeaderClicked(int section);
    void searchButtonPressed();
    void filterConfirmButtonPressed();
    void toggleFilterButtonPressed();
    void toggleEditButtonPressed();

    void nameCheckPressed(bool checked);
    void formulaCheckPressed(bool checked);
    void descriptionCheckPressed(bool checked);
    void subjectCheckPressed(bool checked);
    void categoryCheckPressed(bool checked);
    void relevanceCheckPressed(bool checked);
    void semesterCheckPressed(bool checked);
    void ratingCheckPressed(bool checked);
    void dateCheckPressed(bool checked);

    //Formulas
    void addButtonPressed();
    void formulaEditExecuted();
    void formulaInfomation();
    void formulaDestroy();
    void cancelButtonPressed_Main();

    //Formulasymbols
    void cancelButtonPressed_Formulasymbols();
    void formulasymbolAddAndDefine();
    void formulasymbolPreviousAuswahl();
    void formulasymbolNextAuswahl();
    void formulasymbolDefine();
    void formulasymbolNotDefine();
    void formulasymbolSelected(QString zeichen);
    void formulasymbolUnitSelected(int auswahl);

    //Learning Mode
    void learningModeStart();

    //KLFBackend
    void latexPreviewButtonPressed();
    void latexUpdateAll();
    void latexDialog();

    //Sonstiges
    void settingsSave();
    void autocomplete();
    void autocompleteConfirm();
    void keyboardShortcutDiaglog();
    void creditsDialog();


};

#endif // MAINWINDOW_H
