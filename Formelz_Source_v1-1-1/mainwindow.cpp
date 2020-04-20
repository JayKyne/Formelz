#include "mainwindow.h"
#include "ui_mainwindow.h"


//====================================================================================
//MAIN
//====================================================================================

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    data = new class Data();
    functions = new class functions();

    formulasymbolsGUIActive = false;
    formulasymbolsDefineActive = false;
    latestFormulaText = ""; //for autocompletion
    programVersion = "1.1.1";

    settingsData = data->convertReadSettings(data->readData("/settings.txt"));
    latexCommandsList = data->convertReadSettings(data->readData("/commands.txt"));
    greekList = data->convertReadSettings(data->readData("/greek.txt"));
    latexCommandsList.append(greekList);
    QList<QList<QList<QString>>> mainList = data->convertReadData(data->readData("/database.txt"));


    //check version of database
    if (mainList[0][0].size() == 9) { //version <= 1.1.0
        if (mainList.size() == 3) { //avoid crash
            formulasList = mainList[0];
            formulasymbolsList = mainList[1];
            categoriesList = mainList[2];
        }

        for (int i = 0; i < formulasList.size(); i++) {
            formulasList[i].insert(4, "Generel");
            formulasList[i].insert(6, "1");
        }

        data->save(programVersion, formulasList, formulasymbolsList, categoriesList);
    }

    if (mainList[0][0][1].remove(".").toInt() >= 111) { //version >= 1.1.1
        if (mainList.size() == 4) { //avoid crash
            QString version = mainList[0][0][1];
            formulasList = mainList[1];
            formulasymbolsList = mainList[2];
            categoriesList = mainList[3];
        }
    }

    //infoList: 0: show    1: increase sorting     2: decrease sorting      3: don't show
    for (int i = 0; i < settingsData[5].size(); i++) {
        infoList.append(QString(settingsData[5][i]).toInt());
    }
    visibleList = formulasList; //list is only for display, it is not always completle

    setGui();
    refreshDisplay();

    data->backup();

    //latexUpdateAllformulasymbolsList(); //update all formulasymbols, just do it once
}

//====================================================================================
/* LIST OF CONTENTS
 * -------------------
 * 1. GUI
 * 2. Formula
 * 3. Formula Symbols
 * 4. KLFBackend
 * 5. Filter
 * 6. Dialog
 * 7. Else
 * 8. Keyboard
 */
//====================================================================================
//GUI
//====================================================================================

void MainWindow::setGui() //set GUI and show it
{
    ui->setupUi(this);

    //-----------------------------------------

    w = this->width();
    h = this->height();

    ui->editGroup->setMinimumHeight(int(0.3 * h));
    ui->editGroup->setMaximumHeight(int(0.8 * h));
    ui->filterGroup->setMaximumWidth(int(0.2 * w));

    //--------------------------------------------------

    nameEdit = new QLineEdit();
    formulaEdit = new QLineEdit();
    previewButton = new QPushButton("Preview");
    descriptionEdit = new QLineEdit();
    relevanceEdit = new QLineEdit();
    displayLabel = new QLabel();
    categoryEdit = new QLineEdit();
    subjectCombo = new QComboBox();
    formulasymbolDefineCheck = new QCheckBox();
    addButton = new QPushButton("Add");
    cancelButton = new QPushButton("Clear");
    completeButton = new QPushButton();

    //set on gridlayout
    ui->editLayout->addWidget(nameEdit,0,0,1,2);
    ui->editLayout->addWidget(formulaEdit,0,2);
    ui->editLayout->addWidget(previewButton,0,3);

    ui->editLayout->addWidget(subjectCombo,1,0,1,1);
    ui->editLayout->addWidget(categoryEdit,1,1,1,1);
    ui->editLayout->addWidget(displayLabel,1,2,2,2);

    ui->editLayout->addWidget(relevanceEdit,2,0,1,1);
    ui->editLayout->addWidget(descriptionEdit,2,1,1,1);


    ui->editLayout->addWidget(formulasymbolDefineCheck,3,0,1,2);
    ui->editLayout->addWidget(addButton,3,2,Qt::AlignRight);
    ui->editLayout->addWidget(cancelButton,3,3,Qt::AlignRight);

    //define maximum width
    previewButton->setMaximumWidth(int(0.1 * w));
    ui->filterGroup->setMinimumWidth(int(0.3 * w));

    //define policy for height
    QSizePolicy policy;
    policy.setVerticalPolicy(QSizePolicy::Minimum);
    policy.setHorizontalPolicy(QSizePolicy::Minimum);

    nameEdit->setSizePolicy(policy);
    formulaEdit->setSizePolicy(policy);
    previewButton->setSizePolicy(policy);
    descriptionEdit->setSizePolicy(policy);
    relevanceEdit->setSizePolicy(policy);
    categoryEdit->setSizePolicy(policy);
    subjectCombo->setSizePolicy(policy);
    addButton->setSizePolicy(policy);
    cancelButton->setSizePolicy(policy);
    formulasymbolDefineCheck->setSizePolicy(policy);

    setStyleSheet("QGroupBox{padding-top:0px; margin-top:0px}");


    //set checkboxes like infoList says
    ui->nameCheck->setChecked(infoList[0] != 3);
    ui->formulaCheck->setChecked(infoList[1] != 3);
    ui->descriptionCheck->setChecked(infoList[2] != 3);
    ui->subjectCheck->setChecked(infoList[3] != 3);
    ui->categoryCheck->setChecked(infoList[4] != 3);
    ui->relevanceCheck->setChecked(infoList[5] != 3);
    ui->semesterCheck->setChecked(infoList[6] != 3);
    ui->ratingCheck->setChecked(infoList[7] != 3);
    ui->dateCheck->setChecked(infoList[8] != 3);



    //set names on widgets
    nameEdit->setPlaceholderText(" Name [Ctrl + N]");
    formulaEdit->setPlaceholderText(" Formula [LaTeX]");
    descriptionEdit->setPlaceholderText(" Description [Optinal]");
    displayLabel->setText("Display");
    categoryEdit->setPlaceholderText(" Category");
    relevanceEdit->setPlaceholderText(" Relevance");
    formulasymbolDefineCheck->setText("Define Formula Symbols");
    ui->searchEdit->setPlaceholderText(" Search [Ctrl + S]");

    //set presets
    displayLabel->setAlignment(Qt::AlignCenter);
    formulasymbolDefineCheck->setChecked(true);
    ui->filterGroup->hide();
    nameEdit->setFocus();
    ui->actionLearning->setEnabled(false);

    //---------------------------------
    //set tableview and it rules
    model = new QStandardItemModel(1,9,this);
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    //tab order
    QWidget::setTabOrder(nameEdit, formulaEdit);
    QWidget::setTabOrder(formulaEdit, descriptionEdit);
    QWidget::setTabOrder(descriptionEdit, relevanceEdit);
    QWidget::setTabOrder(relevanceEdit, categoryEdit);
    QWidget::setTabOrder(categoryEdit, subjectCombo);
    QWidget::setTabOrder(subjectCombo, addButton);
    QWidget::setTabOrder(addButton, cancelButton);

    //autoreplacement for categories
    QList<QString> completionListCategories;
    for (int i = 0; i < categoriesList.size(); i++) {
        completionListCategories.append(categoriesList[i][1]);
    }
    kategorieCompleter = new QCompleter(completionListCategories, this);
    kategorieCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    kategorieCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    categoryEdit->setCompleter(kategorieCompleter);

    //set tableview headers
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name")); //↑↓
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Formula"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Subject"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Category"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Relevanz"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("Semester"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("Rating"));
    model->setHeaderData(8, Qt::Horizontal, QObject::tr("Date"));

    //subject items
    subjectCombo->addItem("General");
    subjectCombo->addItem("Physics");
    subjectCombo->addItem("Mathematics");
    subjectCombo->addItem("Astronomie");
    subjectCombo->addItem("Theoretical Physics");

    //subjectFilter items
    ui->subjectFilterCombo->addItem("--- All Subjects ---");
    ui->subjectFilterCombo->addItem("Generel");
    ui->subjectFilterCombo->addItem("Physics");
    ui->subjectFilterCombo->addItem("Mathematics");
    ui->subjectFilterCombo->addItem("Astronomie");
    ui->subjectFilterCombo->addItem("Theoretical Physics");

    //categoryFilterCombo items
    ui->categoryFilterCombo->addItem("--- All Categories ---");
    for (int i = 0; i < categoriesList.size(); i++) {
        ui->categoryFilterCombo->addItem(categoriesList[i][1]);
    }

    //toolbuttons
    ui->toggleEditButton->setIcon(QIcon("E:/Documents/Programmieren/Qt/Physik/darkstyle/icon_branch_open.png"));
    ui->toggleFilterButton->setIcon(QIcon("E:/Documents/Programmieren/Qt/Physik/darkstyle/icon_branch_closed.png"));



    //--------------------------------
    //connections

    //searchbar buttons
    connect(ui->formulaSwitchButton, SIGNAL(clicked()), this, //toggles formula- and formula-symbols-view
            SLOT(formulasymbolsGUI()));
    connect(ui->searchButton, SIGNAL(clicked()), this,
            SLOT(refreshDisplay()));
    connect(ui->toggleFilterButton, SIGNAL(clicked()), this,
            SLOT(toggleFilterButtonPressed()));
    connect(ui->toggleEditButton, SIGNAL(clicked()), this,
            SLOT(toggleEditButtonPressed()));

    //tableview sorting control
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this,
            SLOT(horizontalHeaderClicked(int)));

    //formula add control
    connect(addButton, SIGNAL(clicked()), this,
            SLOT(addButtonPressed()));
    connect(cancelButton, SIGNAL(clicked()), this,
            SLOT(cancelButtonPressed_Main()));
    connect(previewButton, SIGNAL(clicked()), this,
            SLOT(latexPreviewButtonPressed()));

    //action buttons
    connect(ui->actionEdit, SIGNAL(triggered()), this,
            SLOT(formulaEditExecuted()));
    connect(ui->actionInformation, SIGNAL(triggered()), this,
            SLOT(formulaInfomation()));
    connect(ui->actionDelete, SIGNAL(triggered()), this,
            SLOT(formulaDestroy()));
    connect(ui->actionLatexUpdateAll, SIGNAL(triggered()), this,
            SLOT(latexDialog()));
    connect(ui->actionSettings, SIGNAL(triggered()), this,
            SLOT(settingsDialog()));
    connect(ui->actionShortcuts, SIGNAL(triggered()), this,
            SLOT(keyboardShortcutDiaglog()));
    connect(ui->actionCredits, SIGNAL(triggered()), this,
            SLOT(creditsDialog()));
    connect(ui->actionLearning, SIGNAL(triggered()), this,
            SLOT(learningModeStart()));

    //display checkboxes plus confirm button
    connect(ui->nameCheck, SIGNAL(toggled(bool)), this,
            SLOT(nameCheckPressed(bool)));
    connect(ui->formulaCheck, SIGNAL(toggled(bool)), this,
            SLOT(formulaCheckPressed(bool)));
    connect(ui->descriptionCheck, SIGNAL(toggled(bool)), this,
            SLOT(descriptionCheckPressed(bool)));
    connect(ui->subjectCheck, SIGNAL(toggled(bool)), this,
            SLOT(subjectCheckPressed(bool)));
    connect(ui->categoryCheck, SIGNAL(toggled(bool)), this,
            SLOT(categoryCheckPressed(bool)));
    connect(ui->relevanceCheck, SIGNAL(toggled(bool)), this,
            SLOT(relevanceCheckPressed(bool)));
    connect(ui->semesterCheck, SIGNAL(toggled(bool)), this,
            SLOT(semesterCheckPressed(bool)));
    connect(ui->ratingCheck, SIGNAL(toggled(bool)), this,
            SLOT(ratingCheckPressed(bool)));
    connect(ui->dateCheck, SIGNAL(toggled(bool)), this,
            SLOT(dateCheckPressed(bool)));
    connect(ui->filterConfirmButton, SIGNAL(clicked()), this, //confirmation
            SLOT(filterConfirmButtonPressed()));

    //connect lineedit to autocompletion
    connect(completeButton, SIGNAL(clicked()), this,
            SLOT(autocompleteConfirm()));
    connect(formulaEdit, SIGNAL(textEdited(QString)), this,
            SLOT(autocomplete()));

    //connect lineedits' belonging button to enter
    connect(formulaEdit, SIGNAL(returnPressed()), completeButton, SIGNAL(clicked()));
    connect(ui->searchEdit, SIGNAL(returnPressed()), ui->searchButton, SIGNAL(clicked())); 
}

void MainWindow::formulasymbolsGUI() //open the formula-symbol-view
{
    //deactivate all buttons, lineedits, ... which are necessary for the formula-view
    formulasymbolsGUIActive = true;
    disconnect(ui->formulaSwitchButton, SIGNAL(clicked()), this,
            SLOT(formulasymbolsGUI()));
    connect(ui->formulaSwitchButton, SIGNAL(clicked()), this,
            SLOT(formulasGUI()));
    disconnect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this,
            SLOT(horizontalHeaderClicked(int)));
    ui->actionDelete->setEnabled(false);
    ui->actionEdit->setEnabled(false);
    ui->actionInformation->setEnabled(false);
    ui->actionLatexUpdateAll->setEnabled(false);
    ui->actionSettings->setEnabled(false);
    ui->searchButton->setEnabled(false);
    ui->searchEdit->setEnabled(false);
    ui->editGroup->setEnabled(false);
    ui->filterGroup->setEnabled(false);

    //define new tableview headers
    ui->formulaSwitchButton->setText("Symbolz");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Symbol")); //↑↓
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Name"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Unit"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Basic Unit"));

    formulasymbolsList = functions->sort(formulasymbolsList, 1, "str");

    for (int i = 4; i < 7; i++) {
        ui->tableView->setColumnHidden(i, true);
    }

    int modelSize = formulasymbolsList.size(); //Anzahl Zeilen auf Inhalt anpassen
    if (modelSize > settingsData[1].toInt()) {
        modelSize = settingsData[1].toInt();
    }
    model->setRowCount(modelSize);
    for (int i = 0; i < modelSize; i++) {
        for (int j = 0; j < 4; j++) {
            QStandardItem *clear = new QStandardItem(QString(""));
            model->setItem(i,j,clear);
        }
    }
    for (int row = 0; row < modelSize; row++) {
        for (int column = 1; column < formulasymbolsList[row].size(); column++) {
            if (column == 1) {
                QPixmap pixmap = data->readDataPixmapZeichen(formulasymbolsList[row][1]);
                QStandardItem *itemPixmap = new QStandardItem;
                itemPixmap->setData(QVariant(pixmap), Qt::DecorationRole);
                model->setItem(row, column-1, itemPixmap);
            } else {
            QStandardItem *item = new QStandardItem(QString(formulasymbolsList[row][column]));
            model->setItem(row, column-1, item);
            }
        }
    }
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);
}

void MainWindow::formulasGUI() //open the formula view
{
    //deactivate all buttons, lineedits, ... which are necessary for the formula-symbol-view
    //and activate all buttons, ... for the formula-view
    formulasymbolsGUIActive = false;
    disconnect(ui->formulaSwitchButton, SIGNAL(clicked()), this,
            SLOT(formulasGUI()));
    connect(ui->formulaSwitchButton, SIGNAL(clicked()), this,
            SLOT(formulasymbolsGUI()));
    connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this,
            SLOT(horizontalHeaderClicked(int)));
    ui->actionDelete->setEnabled(true);
    ui->actionEdit->setEnabled(true);
    ui->actionInformation->setEnabled(true);
    ui->actionLatexUpdateAll->setEnabled(true);
    ui->actionSettings->setEnabled(true);
    ui->searchButton->setEnabled(true);
    ui->searchEdit->setEnabled(true);
    ui->editGroup->setEnabled(true);
    ui->filterGroup->setEnabled(true);

    //set tableview headers
    ui->formulaSwitchButton->setText("Formelz");
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Name")); //↑↓
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Formula"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Category"));

    refreshDisplay();
}

void MainWindow::refreshDisplay() //refresh GUI
{
    searchButtonPressed();

    //apply filter to visibleList
    QString subjectFilter = ui->subjectFilterCombo->currentText();
    QString categoryFilter = ui->categoryFilterCombo->currentText();
    QList<QList<QString>> list;

    for (int i = 0; i < visibleList.size(); i++) {
        if ((visibleList[i][4] == subjectFilter || subjectFilter == "--- All Subjects ---") &&
            (visibleList[i][5] == categoryFilter || categoryFilter == "--- All Categories ---")) {
            list.append(visibleList[i]);
        }
    }
    visibleList = list;


    //sort visibleList likee infoList says
    visibleList = functions->sort(visibleList, 0, "int");
    visibleList = functions->reverseList(visibleList);
    for (int i = 0; i < infoList.size(); i++) {
        if (infoList[i] == 1 || infoList[i] == 2) { //aufsteigend sortieren
            QString type = "str";
            if (i >= 5 && i <= 7) {
                type = "int";
            } else if (i == 8) {
                type = "date";
            }
            visibleList = functions->sort(visibleList, i+1, type);
            if (infoList[i] == 2) { //absteigend sortieren
                visibleList = functions->reverseList(visibleList);
            }
        }
    }

    //adjust number of lines to content
    int modelSize = visibleList.size();
    if (modelSize > settingsData[1].toInt()) {
        modelSize = settingsData[1].toInt();
    }

    //clear tableview
    model->setRowCount(modelSize);
    for (int i = 0; i < modelSize; i++) {
        for (int j = 0; j < 9; j++) {
            QStandardItem *clear = new QStandardItem(QString(""));
            model->setItem(i,j,clear);
        }
    }

    //refill tableview
    for (int row = 0; row < modelSize; row++) {
        for (int column = 1; column < visibleList[row].size(); column++) {
            if (column == 2) {
                QPixmap pixmap = data->readDataPixmap(visibleList[row][0]);
                QStandardItem *itemPixmap = new QStandardItem;
                itemPixmap->setData(QVariant(pixmap), Qt::DecorationRole);
                model->setItem(row, column-1, itemPixmap);
            } else {
                QStandardItem *item = new QStandardItem(QString(visibleList[row][column]));
                model->setItem(row, column-1, item);
            }
        }
    }    

    //use InfoList to hide columns
    for (int i = 0; i < infoList.size(); i++) {
        ui->tableView->setColumnHidden(i, false);
        if (infoList[i] == 3) { //Spalte ausblenden
            ui->tableView->setColumnHidden(i, true);
        }
    }

    //resize rows and columns
    for (int i = 0; i < 9; i++) {
        if (i != 2) {
            ui->tableView->resizeColumnToContents(i);
       }
    }

    ui->tableView->resizeRowsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);



}

void MainWindow::settingsDialog()
{
    dialogSettings = new QDialog();
    dialogSettings->resize(int(w*0.7),int(h*0.3));
    dialogSettings->show();
    dialogSettings->setWindowTitle("Settings");

    QGridLayout *layout = new QGridLayout(dialogSettings);
    QLabel *labelSemester = new QLabel(dialogSettings);
    QLabel *labelTableSize = new QLabel(dialogSettings);
    QLabel *labelLatexSize = new QLabel(dialogSettings);
    QLabel *labelMiktexPath = new QLabel(dialogSettings);
    editSettingsSemester = new QLineEdit(dialogSettings);
    editSettingsTable = new QLineEdit(dialogSettings);
    editSettingsLatex = new QLineEdit(dialogSettings);
    editSettingsMiktexPath = new QLineEdit(dialogSettings);
    QPushButton *buttonSaveSettings = new QPushButton(dialogSettings);

    layout->addWidget(labelSemester,0,0);
    layout->addWidget(labelTableSize,1,0);
    layout->addWidget(labelLatexSize,2,0);
    layout->addWidget(labelMiktexPath,3,0);
    layout->addWidget(editSettingsSemester,0,1);
    layout->addWidget(editSettingsTable,1,1);
    layout->addWidget(editSettingsLatex,2,1);
    layout->addWidget(editSettingsMiktexPath,3,1);
    layout->addWidget(buttonSaveSettings,4,0,1,2);

    labelSemester->setText("Current Semester:");
    labelTableSize->setText("Maximum Rows:");
    labelLatexSize->setText("LaTeX Size [px]:");
    labelMiktexPath->setText("MiKTeX Path:");
    editSettingsSemester->setText(settingsData[0]);
    editSettingsTable->setText(settingsData[1]);
    editSettingsLatex->setText(settingsData[2]);
    editSettingsMiktexPath->setText(settingsData[3]);
    buttonSaveSettings->setText("Save");

    labelSemester->show();
    labelTableSize->show();
    labelLatexSize->show();
    labelMiktexPath->show();
    editSettingsSemester->show();
    editSettingsTable->show();
    editSettingsLatex->show();
    editSettingsMiktexPath->show();
    buttonSaveSettings->show();

    connect(buttonSaveSettings, SIGNAL(clicked()), this,
            SLOT(settingsSave()));
    connect(editSettingsSemester, SIGNAL(returnPressed()), buttonSaveSettings, SIGNAL(clicked()));
    connect(editSettingsTable, SIGNAL(returnPressed()), buttonSaveSettings, SIGNAL(clicked()));
    connect(editSettingsLatex, SIGNAL(returnPressed()), buttonSaveSettings, SIGNAL(clicked()));
    connect(editSettingsMiktexPath, SIGNAL(returnPressed()), buttonSaveSettings, SIGNAL(clicked()));
}
//====================================================================================
//FORMULA
//====================================================================================

void MainWindow::addButtonPressed()
{
    QString name = nameEdit->text();
    QString formula = formulaEdit->text();
    QString description = descriptionEdit->text();
    QString subject = subjectCombo->currentText();
    QString category = categoryEdit->text();
    QString relevance = relevanceEdit->text();

    renderingNecessary = true;
    if (formulaCache.size() > 2) {
        if (formula == formulaCache[2]) {
            renderingNecessary = false;
        }
    }

    QString text = name + formula + description + subject + category + relevance;

    if (name.isEmpty() || formula.isEmpty() || category.isEmpty()) {
        dialogWindow("eror 404\n"
                     "no! stop!\n"
                     "write something in the fields","error",1);
    } else if (text.contains(">") || text.contains(";")) {
        dialogWindow("eror 505\n"
                     "not this way!\n"
                     "no > or ; allowed","error",1);
    }
    else {
        formulaAddBegin(name, formula, description, subject, category, relevance);
    }
}

void MainWindow::formulaAddBegin(QString str1, QString str2, QString str3, QString str4, QString str5, QString str6)
{   //0   1     2        3            4        5         6          7         8       9     10
    //id, name, formula, description, subject, category, relevance, semester, rating, date, formulasymbol
    QString name = addButton->text(); //get text form addButton
    QString str0 = QString::number(functions->findUnusedId(formulasList)); //get a unused ID
    QString str7 = settingsData[0];
    QString str8 = QString::number(formulasymbolsSearch(str2).size());
    QString str9 = functions->getDate(); //get current data

    if (name == "Add") {
        formulaNew = true;

    } else {
        formulaNew = false;
        addButton->setText("Add");

        str0 = formulaCache[0]; //change, if search started before
        str9 = formulaCache[9];
    }
    formulaCache.clear();
    formulaCache << str0 << str1 << str2 << str3 << str4 << str5 << str6 << str7 << str8 << str9;

    if (formulasymbolDefineCheck->isChecked()) { //first define formula symbols
        formulasymbolsAddBegin();

    } else { //completed adding formula
        formulaAddEnd();
    }

}

void MainWindow::formulaAddEnd()
{
    //Categories
    bool kategorieExists = false;
    for (int i = 0; i < categoriesList.size(); i++) {
        if (categoriesList[i][1] == formulaCache[5]) {
            kategorieExists = true;
        }
    }
    //add new category with new ID
    if (kategorieExists == false) {
        QList<QString> neueKategorie;
        neueKategorie << QString::number(functions->findUnusedId(categoriesList)) << formulaCache[4];
        categoriesList.append(neueKategorie);
        ui->categoryFilterCombo->addItem(neueKategorie[1]);
    }
    //update autocompletion for categories
    QList<QString> completionListCategories;
    for (int i = 0; i < categoriesList.size(); i++) {
        completionListCategories.append(categoriesList[i][1]);
    }
    kategorieCompleter->setModel(new QStringListModel(completionListCategories, kategorieCompleter));


    //saving
    if (formulaNew) {
        formulasList.append(formulaCache);
    } else {
        formulasList[functions->getPlace(formulaCache[0], formulasList)] = formulaCache;
        if (renderingNecessary) { //neues bild rendern
        QString path = installPath + "/img/formulas";
        QDir dir(path);
        dir.remove(formulaCache[0] + ".png");
        }
    }
    cancelButtonPressed_Main(); //clear nameEdit, formulaEdit, ...

    //LaTeX
    if (renderingNecessary) {
        dialogWindow("Formula is rendering...","Status",0);
        QPixmap pix = latexRun(formulaCache[2], settingsData[2].toInt()); //LaTeX bild erstellen
        data->writeDataPixmap(pix,formulaCache[0],"formulas/"); //create picture and name it with formulaID
        dialogDestroy();
    }

    data->save(programVersion, formulasList, formulasymbolsList, categoriesList);
    visibleList = formulasList;
    formulaCache.clear();
    refreshDisplay();
}

void MainWindow::formulaEditExecuted()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();
    if (!select->selectedRows().isEmpty()) {
        int section = select->selectedRows()[0].row();

        nameEdit->setText(visibleList[section][1]);
        formulaEdit->setText(visibleList[section][2]);
        descriptionEdit->setText(visibleList[section][3]);
        subjectCombo->setCurrentText(visibleList[section][4]); //does not function properly
        categoryEdit->setText(visibleList[section][5]);
        relevanceEdit->setText(visibleList[section][6]);

        formulaCache.clear();
        for (int i = 0; i < visibleList[section].size(); i++) {
            formulaCache.append(visibleList[section][i]);
        }

        addButton->setText("Confirm");
    }
}

void MainWindow::formulaInfomation()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();
    if (!select->selectedRows().isEmpty()) {
        int section = select->selectedRows()[0].row();
        if (visibleList[section].size() == 11) { //test if symbols are connected to the formula
            QList<QString> zeichen = functions->stringToList(visibleList[section][10]);
            QList<QList<QString>> zeichenInFormulasymbols;
            int place;

            for (int i = 0; i < zeichen.size(); i++) {
                place = functions->getPlace(zeichen[i], formulasymbolsList);
                zeichenInFormulasymbols.append(formulasymbolsList[place]);
            }

            formulasymbolsGUI();

            int modelSize = zeichenInFormulasymbols.size(); //Anzahl Zeilen auf Inhalt anpassen
            if (modelSize > settingsData[1].toInt()) {
                modelSize = settingsData[1].toInt();
            }
            model->setRowCount(modelSize);
            for (int i = 0; i < modelSize; i++) {
                for (int j = 0; j < 4; j++) {
                    QStandardItem *clear = new QStandardItem(QString(""));
                    model->setItem(i,j,clear);
                }
            }
            for (int row = 0; row < zeichenInFormulasymbols.size(); row++) {
                for (int column = 1; column < zeichenInFormulasymbols[row].size(); column++) {
                    if (column == 1) {
                        QPixmap pixmap = data->readDataPixmapZeichen(zeichenInFormulasymbols[row][1]);
                        QStandardItem *itemPixmap = new QStandardItem;
                        itemPixmap->setData(QVariant(pixmap), Qt::DecorationRole);
                        model->setItem(row, column-1, itemPixmap);
                    } else {
                        QStandardItem *item = new QStandardItem(QString(zeichenInFormulasymbols[row][column]));
                        model->setItem(row, column-1, item);
                    }
                }
            }
            ui->tableView->resizeRowsToContents();
            ui->tableView->resizeColumnsToContents();
            ui->tableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);



        } else {
            dialogWindow("error 606\n"
                         "Formula symbol could not be found","error",1);
        }
    }
}

void MainWindow::formulaDestroy()
{
    QItemSelectionModel *select = ui->tableView->selectionModel();
    if (!select->selectedRows().isEmpty()) {
        int section = select->selectedRows()[0].row();

        QString id = visibleList[section][0];
        QFile file (installPath + "/img/formulas/" + id + ".png");
        file.remove();

        formulasList.removeAt(functions->getPlace(id, formulasList));
        data->save(programVersion,formulasList, formulasymbolsList, categoriesList);
        visibleList = formulasList;
        refreshDisplay();
    }
}

void MainWindow::cancelButtonPressed_Main()
{
    nameEdit->clear();
    formulaEdit->clear();
    descriptionEdit->clear();
    subjectCombo->setCurrentIndex(0);
    categoryEdit->clear();
    relevanceEdit->clear();
    displayLabel->setText("Display");
    addButton->setText("Add");
}

//====================================================================================
//FORMULA SYMBOLS
//====================================================================================

void MainWindow::formulasymbolsAddBegin() //start GUI to add formula symbols
{
    formulasymbolsDefineActive = true;
    disconnect(cancelButton, SIGNAL(clicked()), this,
               SLOT(cancelButtonPressed_Main()));
    connect(cancelButton, SIGNAL(clicked()), this,
            SLOT(cancelButtonPressed_Formulasymbols()));

    QList<QString> formulasymbolsFound = formulasymbolsSearch(formulaCache[2]);

    nameEdit->hide();
    formulaEdit->hide();
    descriptionEdit->hide();
    subjectCombo->hide();
    categoryEdit->hide();
    relevanceEdit->hide();
    displayLabel->hide();
    previewButton->hide();
    formulasymbolDefineCheck->hide();
    addButton->hide();


    zeichenFoundCombo = new QComboBox();
    zeichenAvailableCombo = new QComboBox();
    previousAuswahlButton = new QPushButton("<");
    nextAuswahlButton = new QPushButton(">");
    defineFormulasymbolsButton = new QPushButton("Define");
    notDefineFormulasymbolsButton = new QPushButton("Not Define");
    formulasymbolsNameEdit = new QLineEdit();
    einheitEdit = new QLineEdit();
    grundeinheitEdit = new QLineEdit();


    //set widget position
    ui->editLayout->addWidget(zeichenFoundCombo,0,0,1,2);
    ui->editLayout->addWidget(formulasymbolsNameEdit,0,2,1,2);

    ui->editLayout->addWidget(zeichenAvailableCombo,1,0,1,2);
    ui->editLayout->addWidget(einheitEdit,1,2,1,2);

    ui->editLayout->addWidget(previousAuswahlButton,2,0);
    ui->editLayout->addWidget(nextAuswahlButton,2,1);
    ui->editLayout->addWidget(grundeinheitEdit,2,2,1,2);

    ui->editLayout->addWidget(defineFormulasymbolsButton,3,0);
    ui->editLayout->addWidget(notDefineFormulasymbolsButton,3,1);


    //define policy for height and width
    QSizePolicy policy;
    policy.setVerticalPolicy(QSizePolicy::Minimum);
    policy.setHorizontalPolicy(QSizePolicy::Minimum);

    zeichenFoundCombo->setSizePolicy(policy);
    formulasymbolsNameEdit->setSizePolicy(policy);
    zeichenAvailableCombo->setSizePolicy(policy);
    einheitEdit->setSizePolicy(policy);
    previousAuswahlButton->setSizePolicy(policy);
    nextAuswahlButton->setSizePolicy(policy);
    grundeinheitEdit->setSizePolicy(policy);
    defineFormulasymbolsButton->setSizePolicy(policy);
    notDefineFormulasymbolsButton->setSizePolicy(policy);


    //set placeholder text
    formulasymbolsNameEdit->setPlaceholderText(" Name");
    einheitEdit->setPlaceholderText(" Unit");
    grundeinheitEdit->setPlaceholderText(" Basic Unit");
    cancelButton->setText("Cancel");

    zeichenAvailableCombo->addItem("New Symbol");

    for (int i = 0; i < formulasymbolsFound.size(); i++) {
        zeichenFoundCombo->addItem(formulasymbolsFound[i]);
    }

    connect(zeichenFoundCombo, SIGNAL(currentTextChanged(QString)), this,
            SLOT(formulasymbolSelected(QString)));
    connect(defineFormulasymbolsButton, SIGNAL(clicked()), this,
            SLOT(formulasymbolDefine()));
    connect(notDefineFormulasymbolsButton, SIGNAL(clicked()), this,
            SLOT(formulasymbolNotDefine()));

    connect(previousAuswahlButton, SIGNAL(clicked()), this,
            SLOT(formulasymbolPreviousAuswahl()));
    connect(nextAuswahlButton, SIGNAL(clicked()), this,
            SLOT(formulasymbolNextAuswahl()));

    formulasymbolSelected(formulasymbolsFound[0]);
}

//search for formula symbols in a formula
QList<QString> MainWindow::formulasymbolsSearch(QString formula)
{
    QString zeichen = " 1234567890+-*/^_{}()[]='.,;";

    QList<QString> list;
    bool valid;
    bool command = false;
    QString commandName;
    bool commandBegin = false;
    bool subscript = false;

    for (int i = 0; i < formula.size(); ++i) {
        valid = true;
        for (int k = 0; k < zeichen.size(); k++) {
            if (formula[i] == zeichen[k]) {
                valid = false;
            }
        }
        //-------------------------------------------------------------
        if (formula[i] == "\\" || formula[i] == "_") { //Kommando startet
            command = true;
            commandName.clear();
        }
        commandName.append(formula[i]);
        //--------------------------------------------------------------
        if (valid == true && command == false && subscript == false && commandBegin == false) { //Hauptabfrage
            list.append(QString(formula[i]));
        }
        //---------------------------------------------------------------
        if (command == true && (formula[i] == "{" || formula[i] == "(" || formula[i] == " ")) {
            command = false;
            if (commandName == "\\begin{" || commandName == "\\end{" || commandName == "\\text{" || commandName == "_{") {
                commandBegin = true;
            }
        }
        if (functions->inList(commandName, greekList)) {
            list.append(QString(commandName.remove(0,1)));
        }

        if (commandBegin == true && formula[i] == "}") {
            commandBegin = false;
        }
        //---------------------------------------------------------------
    }
    return list;
}

void MainWindow::formulasymbolSelected(QString zeichen)
{
    disconnect(zeichenAvailableCombo, SIGNAL(currentIndexChanged(int)), this,
               SLOT(formulasymbolUnitSelected(int)));

    formulasymbolsNameEdit->clear();
    einheitEdit->clear();
    grundeinheitEdit->clear();
    zeichenAvailableCombo->clear();
    currentFormulasymbolId.clear();

    QList<int> found;
    for (int i = 0; i < formulasymbolsList.size(); i++) {
        if (formulasymbolsList[i][1] == zeichen) {
            found.append(i);
        }
    }


    zeichenAvailableCombo->addItem("New Symbol");
    for (int j = 0; j < found.size(); j++) {
        zeichenAvailableCombo->addItem(QString::number(j+1) + ". Selection");
        currentFormulasymbolId.append(formulasymbolsList[found[j]][0]);
    }

    if (found.size() > 0) {
        zeichenAvailableCombo->setCurrentText("1. Selection");
        formulasymbolUnitSelected(1);
    } else {
        formulasymbolUnitSelected(0);
    }

    connect(zeichenAvailableCombo, SIGNAL(currentIndexChanged(int)), this,
            SLOT(formulasymbolUnitSelected(int)));
}

void MainWindow::formulasymbolPreviousAuswahl()
{
    int index = zeichenAvailableCombo->currentIndex();
    index -= 1;
    if (index < 0) {
        index = 0;
    }

    formulasymbolUnitSelected(index);
}

void MainWindow::formulasymbolNextAuswahl()
{
    int index = zeichenAvailableCombo->currentIndex();
    int count = zeichenAvailableCombo->count() - 1;
    index += 1;
    if (index > count) {
        index = count;
    }
    formulasymbolUnitSelected(index);
}

void MainWindow::formulasymbolUnitSelected(int auswahl)
{
    zeichenAvailableCombo->setCurrentIndex(auswahl);
    if (auswahl > 0) {
        formulasymbolExists = true;
        selectedFormulasymbol = currentFormulasymbolId[auswahl-1];

        int place = functions->getPlace(selectedFormulasymbol, formulasymbolsList);

        formulasymbolsNameEdit->setText(formulasymbolsList[place][2]);
        einheitEdit->setText(formulasymbolsList[place][3]);
        grundeinheitEdit->setText(formulasymbolsList[place][4]);
        selectedFormulasymbolId = formulasymbolsList[place][0];
        formulasymbolsNameEdit->setReadOnly(true);
        einheitEdit->setReadOnly(true);
        grundeinheitEdit->setReadOnly(true);
        defineFormulasymbolsButton->setText("Define");

    } else {
        formulasymbolExists = false;

        formulasymbolsNameEdit->clear();
        einheitEdit->clear();
        grundeinheitEdit->clear();
        formulasymbolsNameEdit->setReadOnly(false);
        einheitEdit->setReadOnly(false);
        grundeinheitEdit->setReadOnly(false);
        defineFormulasymbolsButton->setText("Add and Define");
   }
}

void MainWindow::formulasymbolDefine()
{
    if (formulasymbolExists) {
        formulasymbolConnect(selectedFormulasymbolId);
    } else {
        formulasymbolAddAndDefine();
    }

}

void MainWindow::formulasymbolAddAndDefine()
{
    QString str0 = QString::number(functions->findUnusedId(formulasymbolsList));
    QString str1 = zeichenFoundCombo->currentText();
    QString str2 = formulasymbolsNameEdit->text();
    QString str3 = einheitEdit->text();
    QString str4 = grundeinheitEdit->text();
    QList<QString> list;
    list << str0 << str1 << str2 << str3 << str4;

    QString text = str2 + str3 + str4;
    if (str2.isEmpty() || str3.isEmpty() || str4.isEmpty()) {
        dialogWindow("error 101\n"
                     "Are you stupid?\n"
                     "Do not pass empty fields!\n"
                     "Steve Altaa","error",1);
    } else if (text.contains(">") || text.contains(";")) {
    dialogWindow("eror 202\n"
                 "Really?!\n"
                 "Just do not use > or ;","error",1);
    }
    else {
        formulasymbolsList.append(list);
        formulasymbolConnect(str0);
    }
}

void MainWindow::formulasymbolConnect(QString id) //delete formulasymbol from combobox and update it
{
    if (formulaCache.size() == 11 && !formulaCache[10].contains(id)) {
        formulaCache[10].append("," + id);
    } else if (formulaCache.size() == 10) {
        formulaCache.append(id);
    }

    zeichenFoundCombo->removeItem(zeichenFoundCombo->currentIndex());
    zeichenFoundCombo->setCurrentIndex(0);

    if (zeichenFoundCombo->currentText().isEmpty()) {
        formulasymbolAddEnd();
        formulaAddEnd();
    }
}

void MainWindow::formulasymbolNotDefine()
{
    zeichenFoundCombo->removeItem(zeichenFoundCombo->currentIndex());
    zeichenFoundCombo->setCurrentIndex(0);

    if (zeichenFoundCombo->currentText().isEmpty()) {
        formulasymbolAddEnd();
        formulaAddEnd();
    }
}

void MainWindow::formulasymbolAddEnd()
{
    disconnect(cancelButton, SIGNAL(clicked()), this,
               SLOT(cancelButtonPressed_Formulasymbols()));
    connect(cancelButton, SIGNAL(clicked()), this,
            SLOT(cancelButtonPressed_Main()));

    ui->editLayout->removeWidget(zeichenFoundCombo);
    ui->editLayout->removeWidget(zeichenAvailableCombo);
    ui->editLayout->removeWidget(previousAuswahlButton);
    ui->editLayout->removeWidget(nextAuswahlButton);
    ui->editLayout->removeWidget(defineFormulasymbolsButton);
    ui->editLayout->removeWidget(notDefineFormulasymbolsButton);
    ui->editLayout->removeWidget(formulasymbolsNameEdit);
    ui->editLayout->removeWidget(einheitEdit);
    ui->editLayout->removeWidget(grundeinheitEdit);

    delete zeichenFoundCombo;
    delete zeichenAvailableCombo;
    delete previousAuswahlButton;
    delete nextAuswahlButton;
    delete defineFormulasymbolsButton;
    delete notDefineFormulasymbolsButton;
    delete formulasymbolsNameEdit;
    delete einheitEdit;
    delete grundeinheitEdit;

    nameEdit->show();
    formulaEdit->show();
    descriptionEdit->show();
    subjectCombo->show();
    categoryEdit->show();
    relevanceEdit->show();
    displayLabel->show();
    previewButton->show();
    formulasymbolDefineCheck->show();
    addButton->show();
    cancelButton->setText("Clear");

    nameEdit->setText(formulaCache[1]);
    formulaEdit->setText(formulaCache[2]);
    descriptionEdit->setText(formulaCache[3]);
    subjectCombo->setCurrentText(formulaCache[4]); //should take formulaCache aswell
    categoryEdit->setText(formulaCache[5]);
    relevanceEdit->setText(formulaCache[6]);


    nameEdit->setFocus();
    formulasymbolsDefineActive = false;
}

void MainWindow::cancelButtonPressed_Formulasymbols()
{
    formulasymbolAddEnd();
}

//====================================================================================
//LEARNING MODE
//====================================================================================

void MainWindow::learningModeStart()
{
    ui->tableGroup->hide();
    ui->editGroup->hide();

    QComboBox *learningSemesterCombo = new QComboBox(this);
    QComboBox *learningKategorieCombo = new QComboBox(this);

    QPushButton *learningConfirmButton = new QPushButton(this);

    ui->mainLayout->addWidget(learningSemesterCombo,0,0);
    ui->mainLayout->addWidget(learningKategorieCombo,0,1);
    ui->mainLayout->addWidget(learningConfirmButton,1,1);


}











//====================================================================================
//KLFBackend
//====================================================================================

QPixmap MainWindow::latexRun(QString string, int size)
{
    bool ok = KLFBackend::detectSettings(&settings, settingsData[3]);

    if (!ok) {
        dialogWindow("shit...\n"
                     "LaTeX, dvips and gs not found.\n",
                     "error",1);
    }

    input.latex = string;
    input.mathmode = "\\[ ... \\]";
    input.preamble = "\\usepackage{amssymb,amsmath,mathrsfs,physics}\n";
    input.fg_color = qRgb(255, 255, 255); // white
    input.bg_color = qRgba(0, 0, 0, 0); //transparent
    input.dpi = size;


    KLFBackend::klfOutput out = KLFBackend::getLatexFormula(input, settings);
    if (out.status != 0 && ok) {
        dialogWindow("error 505\n"
                     "deadbeat...","error",1);
        return QPixmap();
    }
    return QPixmap::fromImage((out.result));
}

void MainWindow::latexPreviewButtonPressed()
{
    displayLabel->setText("Formula is rendering...");
    QString str = formulaEdit->text();
    int size = int(displayLabel->width() * 0.4);
    QPixmap pix = latexRun(str,size);

    displayLabel->setPixmap(pix);
}

void MainWindow::latexDialog()
{
    dialogWindow("Are you sure?\n"
                 "Depending on the size of the formula collection,\n"
                 "this operation could take a little longer\n",
                 "Dialog",2);
}

void MainWindow::latexUpdateAll()
{
    dialogDestroy();
    dialogWindow("Formulas are rendering...","Status",0);
    QString path = installPath + "/img/formulas";
    QDir dir(path);
    dir.setNameFilters(QList<QString>() << "*.*");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }

    QPixmap pix;
    for (int i = 0; i < formulasList.size(); i++) {
        pix = latexRun(formulasList[i][2], settingsData[2].toInt());
        data->writeDataPixmap(pix, formulasList[i][0],"formulas/");
    }

    dialogDestroy();
    refreshDisplay();
}

void MainWindow::latexUpdateAllFormulasymbols()
{
    QString list1 = "abcdefghijklmnopqrstuvwxyz";
    QString list2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QList<QString> wholeList;

    for (int i = 0; i < list1.size(); i++) {
        wholeList.append(QString(list1[i]) + "_small");
        wholeList.append(QString(list2[i]) + "_big");
    }
    for (int i = 0; i < greekList.size(); i++) {
        if (list1.contains(greekList[i][1])) {
            wholeList.append(greekList[i] + "_small");
        } else {
            wholeList.append(greekList[i] + "_big");
        }
    }

    dialogWindow("Formula symbols are rendering...","Status",0);
    QString path = installPath + "/img/formulasymbols";

    QDir dir(path);

    dir.setNameFilters(QList<QString>() << "*.*");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }

    QPixmap pix;
    QString zeichen;
    for (int i = 0; i < wholeList.size(); i++) {
        zeichen.clear();
        for (int j = 0; j < wholeList[i].size(); j++) {
            if (wholeList[i][j] == "_") {
                break;
            }
            zeichen.append(wholeList[i][j]);
        }
        pix = latexRun(zeichen, settingsData[2].toInt());
        data->writeDataPixmap(pix, wholeList[i],"formulasymbols/");
    }

    dialogDestroy();
}

//====================================================================================
//FILTER
//====================================================================================




void MainWindow::toggleFilterButtonPressed()
{
    if (filterGroupShow) {
        ui->filterGroup->hide();   
        ui->toggleFilterButton->setIcon(QIcon("E:/Documents/Programmieren/Qt/Physik/darkstyle/icon_branch_closed.png"));
        filterGroupShow = false;
    } else {
        ui->filterGroup->show();
        ui->toggleFilterButton->setIcon(QIcon("E:/Documents/Programmieren/Qt/Physik/darkstyle/icon_branch_open.png"));
        filterGroupShow = true;
    }

}

void MainWindow::toggleEditButtonPressed()
{
    if (editGroupShow) {
        ui->editGroup->hide();
        ui->toggleEditButton->setIcon(QIcon("E:/Documents/Programmieren/Qt/Physik/darkstyle/icon_branch_closed.png"));
        editGroupShow = false;
    } else {
        ui->editGroup->show();
        ui->toggleEditButton->setIcon(QIcon("E:/Documents/Programmieren/Qt/Physik/darkstyle/icon_branch_open.png"));
        editGroupShow = true;
    }

}

void MainWindow::filterConfirmButtonPressed()
{
    refreshDisplay();
}

void MainWindow::searchButtonPressed()
{
    QString searching = ui->searchEdit->text().toLower();
    QList<int> matches;
    bool alreadyAdded;
    visibleList.clear();

    for (int i = 0; i < formulasList.size(); i++) {
        alreadyAdded = false;
        for (int j = 1; j < formulasList[i].size(); j++) {
            if ((formulasList[i][j].toLower().contains(searching) || searching.contains(formulasList[i][j].toLower()))
                 && formulasList[i][j] != ""
                 && !alreadyAdded) {
                matches.append(i);
                alreadyAdded = true;
            }
        }
    }

    for (int k = 0; k < matches.size(); k++) {
        visibleList.append(formulasList[matches[k]]);
    }  
}

//====================================================================================
//DIALOG
//====================================================================================

void MainWindow::dialogWindow(QString text, QString title, int type) //type: 0: no Button , 1: ok Button , 2: yes/no Button
{
    dialog = new QDialog();
    dialog->resize(int(w*0.5),int(h*0.3));
    dialog->show();
    dialog->setWindowTitle(title);


    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QLabel *label = new QLabel(dialog);

    layout->addWidget(label);
    label->setText(text);
    label->show();

    if (type == 1) {
        QPushButton *okButton = new QPushButton(dialog);
        layout->addWidget(okButton);
        okButton->setText("Ok");
        okButton->show();
        connect(okButton, SIGNAL(clicked()), this, SLOT(dialogDestroy()));
    } else if (type == 2) {
        QHBoxLayout *hLayout = new QHBoxLayout(dialog);
        QPushButton *yesButton = new QPushButton(dialog);
        QPushButton *noButton = new QPushButton(dialog);
        layout->addLayout(hLayout);
        hLayout->addWidget(yesButton);
        hLayout->addWidget(noButton);
        yesButton->setText("Yes");
        noButton->setText("No");
        yesButton->show();
        noButton->show();
        connect(noButton, SIGNAL(clicked()), this, SLOT(dialogDestroy()));
        connect(yesButton, SIGNAL(clicked()), this, SLOT(latexUpdateAll()));
    }
}

void MainWindow::keyboardShortcutDiaglog()
{
    dialog = new QDialog();
    dialog->resize(int(w*0.5),int(h*0.3));
    dialog->show();
    dialog->setWindowTitle("Shortcuts");


    QVBoxLayout *vlayout = new QVBoxLayout(dialog);

    QHBoxLayout *hlayout1 = new QHBoxLayout(dialog);
    QGroupBox *group1 = new QGroupBox(dialog);
    QLabel *labelLeft1 = new QLabel(group1);
    QLabel *labelRight1 = new QLabel(group1);

    QHBoxLayout *hlayout2 = new QHBoxLayout(dialog);
    QGroupBox *group2 = new QGroupBox(dialog);
    QLabel *labelLeft2 = new QLabel(group2);
    QLabel *labelRight2 = new QLabel(group2);

    QHBoxLayout *hlayout3 = new QHBoxLayout(dialog);
    QGroupBox *group3 = new QGroupBox(dialog);
    QLabel *labelLeft3 = new QLabel(group2);
    QLabel *labelRight3 = new QLabel(group2);

    QPushButton *button = new QPushButton(dialog);

    dialog->setLayout(vlayout);
    vlayout->addWidget(group1);
    vlayout->addWidget(group2);
    vlayout->addWidget(group3);
    vlayout->addWidget(button);

    group1->setLayout(hlayout1);
    group2->setLayout(hlayout2);
    group3->setLayout(hlayout3);

    hlayout1->addWidget(labelLeft1);
    hlayout1->addWidget(labelRight1);
    hlayout2->addWidget(labelLeft2);
    hlayout2->addWidget(labelRight2);
    hlayout3->addWidget(labelLeft3);
    hlayout3->addWidget(labelRight3);

    labelLeft1->show();
    labelRight1->show();
    labelLeft2->show();
    labelRight2->show();
    labelLeft3->show();
    labelRight3->show();
    button->show();

    group1->setTitle("General");
    group2->setTitle("Formula Symbols");
    group3->setTitle("Display");

    labelLeft1->setText("Ctrl + I \n"
                        "Ctrl + E \n"
                        "Ctrl + Del \n"
                        "Ctrl + N \n"
                        "Ctrl + M \n"
                        "Ctrl + F \n"
                        "Ctrl + S \n"
                        "Ctrl + ,");
    labelLeft2->setText("Left Arrow \n"
                        "Right Arrow");
    labelLeft3->setText("Ctrl + 1 \n"
                        "Ctrl + 2 \n"
                        "Ctrl + 3 \n"
                        "Ctrl + 4 \n"
                        "Ctrl + 5 \n"
                        "Ctrl + 6 \n"
                        "Ctrl + 7 \n"
                        "Ctrl + 8 \n"
                        "Ctrl + 9 \n"
                        "Ctrl + 0");

    labelRight1->setText("formula information \n"
                         "edit formula \n"
                         "delete formula \n"
                         "new formula \n"
                         "confirm new formula \n"
                         "toggle define formula symbols \n"
                         "search \n"
                         "settings");
    labelRight2->setText("previous selection \n"
                         "next selection");
    labelRight3->setText("toggle name \n"
                         "toggle formula \n"
                         "toggle description \n"
                         "toggle subject \n"
                         "toggle category \n"
                         "toggle relevance \n"
                         "toggle semester \n"
                         "toggle rating \n"
                         "toggle date \n"
                         "show all");

    button->setText("Ok");
    connect(button, SIGNAL(clicked()), this, SLOT(dialogDestroy()));
}

void MainWindow::creditsDialog()
{
    QString date = QDateTime::currentDateTime().toString("dd.MM.yyyy");
    dialogWindow("Formelz " + programVersion + " \n"
                 "\n"
                 "Based on the KLatexFormula 4.0.0 from Philippe Faist \n"
                 "Stylesheet \"Qt-Frameless-Window-DarkStyle\" from Juergen Skrotzky \n"
                 "\n"
                 "Created on " + date + " by Janik Kreit \n"
                 "janikkreit@outlook.de \n"
                 "\n"
                 "The program is provided AS IS with NO WARRANTY \n"
                 "OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, \n"
                 "MERCHANTABILITY AND FITNESS FOR \n"
                 "A PARTICULAR PURPOSE",
                 "About Formelz",1);
}


void MainWindow::dialogDestroy()
{
    dialog->close();
}


//====================================================================================
//ELSE
//====================================================================================


void MainWindow::horizontalHeaderClicked(int section)
{
    for(int i = 0; i < infoList.size(); i++) {
        if (infoList[i] != 3 && i != section) {
            infoList[i] = 0; //alle bereiche (außer die sortieren und ausgeblendeten) auf null setzen
        }
    }

    if (infoList[section] == 1) {
        infoList[section] = 2;
    } else {
        infoList[section] = 1;
    }

    refreshDisplay();

}

void MainWindow::autocomplete()
{
    QString text = formulaEdit->text();
    if (text.size() > latestFormulaText.size()) {
        QString command;
        for (int i = 0; i < text.size(); i++) {
            command.append(text[i]);
            if (text[i] == " ") {
                command.clear();
            }
        }
        QString commandBegin;
        QString commandEnd;
        for (int i = 0; i < latexCommandsList.size(); i++) {
            commandBegin = latexCommandsList[i].left(command.size());
            commandEnd = latexCommandsList[i].right(latexCommandsList[i].size() - command.size());
            if (command == commandBegin && command.size() != 0) {
                formulaEdit->setText(text + commandEnd);
                formulaEdit->setSelection(text.size(), commandEnd.size());
                i = latexCommandsList.size();
            }

        }
    }
    latestFormulaText = text;
}

void MainWindow::autocompleteConfirm()
{
    QString text = formulaEdit->text();
    int position = formulaEdit->cursorPosition();
    if (formulaEdit->hasSelectedText()) {
        formulaEdit->deselect();
        if (text.right(3) == "}{}") {
            formulaEdit->cursorBackward(false,3);
        } else if (text.right(1) == "}") {
            formulaEdit->cursorBackward(false,1);
        }
    } else if (text[position] == "}") {
        formulaEdit->cursorForward(false,2);
    }
}


void MainWindow::settingsSave()
{
    QString str0 = editSettingsSemester->text();
    QString str1 = editSettingsTable->text();
    QString str2 = editSettingsLatex->text();
    QString str3 = editSettingsMiktexPath->text();
    if (str0.isEmpty() || str1.isEmpty() || str2.isEmpty() || str3.isEmpty()) {
        dialogWindow("error 808\n"
                     "Do not try to destroy the program\n"
                     "Do not leave the field empty",
                     "error",1);
    } else if (str0.toInt() == 0 || str1.toInt() == 0 || str2.toInt() == 0) {
        dialogWindow("Seriously?\n"
                     "Just. Do. Your. Fucking. Job!",
                     "Steve Altaaa",1);
    } else {
        dialogSettings->close();
        settingsData[0] = str0;
        settingsData[1] = str1;
        settingsData[2] = str2;
        settingsData[3] = str3;
        data->saveSettings(settingsData);
        refreshDisplay();
    }
}

//Checkboxes
void MainWindow::nameCheckPressed(bool checked)
{
    if (checked) {
        infoList[0] = 0;
    } else {
        infoList[0] = 3;
    }
}

void MainWindow::formulaCheckPressed(bool checked)
{
    if (checked) {
        infoList[1] = 0;
    } else {
        infoList[1] = 3;
    }
}

void MainWindow::descriptionCheckPressed(bool checked)
{
    if (checked) {
        infoList[2] = 0;
    } else {
        infoList[2] = 3;
    }
}

void MainWindow::subjectCheckPressed(bool checked)
{
    if (checked) {
        infoList[3] = 0;
    } else {
        infoList[3] = 3;
    }
}

void MainWindow::categoryCheckPressed(bool checked)
{
    if (checked) {
        infoList[4] = 0;
    } else {
        infoList[4] = 3;
    }
}

void MainWindow::relevanceCheckPressed(bool checked)
{
    if (checked) {
        infoList[5] = 0;
    } else {
        infoList[5] = 3;
    }
}

void MainWindow::semesterCheckPressed(bool checked)
{
    if (checked) {
        infoList[6] = 0;
    } else {
        infoList[6] = 3;
    }
}

void MainWindow::ratingCheckPressed(bool checked)
{
    if (checked) {
        infoList[7] = 0;
    } else {
        infoList[7] = 3;
    }
}

void MainWindow::dateCheckPressed(bool checked)
{
    if (checked) {
        infoList[8] = 0;
    } else {
        infoList[8] = 3;
    }
}



void MainWindow::p()
{
    qDebug() << QTime::currentTime();
}


//====================================================================================
//KEYBOARD
//====================================================================================

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    if (formulasymbolsDefineActive && !formulasymbolsGUIActive) {
        switch (e->key()) {
            case (Qt::Key_Left):
                formulasymbolPreviousAuswahl();
                break;

            case (Qt::Key_Right):
                formulasymbolNextAuswahl();
                break;
        }
    }

    if (!formulasymbolsDefineActive && !formulasymbolsGUIActive) {
        switch (e->key()) {
            case (Qt::Key_E): //edit formula
                if (e->modifiers() == Qt::ControlModifier)
                    formulaEditExecuted();
                break;

            case (Qt::Key_I): //call formulainformation
                if (e->modifiers() == Qt::ControlModifier)
                    formulaInfomation();
                break;

            case (Qt::Key_N): //set focus on nameEdit
                if (e->modifiers() == Qt::ControlModifier) {
                    nameEdit->setFocus();
                    nameEdit->selectAll();
                }
                break;

            case (Qt::Key_M): //confirm new formula
                if (e->modifiers() == Qt::ControlModifier) {
                    addButtonPressed();
                }
                break;

            case (Qt::Key_S): //toggle formulasymbolDefineCheck
                if (e->modifiers() == Qt::ControlModifier) {
                    ui->searchEdit->setFocus();
                    ui->searchEdit->selectAll();
                }
                break;

            case (Qt::Key_F): //set focus on searchEdit
                if (e->modifiers() == Qt::ControlModifier)
                    formulasymbolDefineCheck->toggle();
                break;

            case (Qt::Key_Delete): //delete formula
                if(e->modifiers() & Qt::ControlModifier) {
                    formulaDestroy();}
                break;

            case (Qt::Key_P): //call preview
                if (e->modifiers() == Qt::ControlModifier)
                    latexPreviewButtonPressed();
                break;


            case (Qt::Key_Escape): //call preview
                cancelButtonPressed_Main();
                break;

            case (Qt::Key_1): //toggle name
                if (e->modifiers() == Qt::ControlModifier) {
                    ui->nameCheck->toggle();
                    nameCheckPressed(ui->nameCheck->isChecked());
                    refreshDisplay();
                }
                break;

            case (Qt::Key_2): //toggle formula
                if (e->modifiers() == Qt::ControlModifier) {
                    ui->formulaCheck->toggle();
                    formulaCheckPressed(ui->formulaCheck->isChecked());
                    refreshDisplay();
                }
                break;

            case (Qt::Key_3): //toggle description
                if (e->modifiers() == Qt::ControlModifier) {
                    ui->descriptionCheck->toggle();
                    descriptionCheckPressed(ui->descriptionCheck->isChecked());
                    refreshDisplay();
                }
                break;

            case (Qt::Key_4): //toggle subject
                if (e->modifiers() == Qt::ControlModifier) {
                    ui->subjectCheck->toggle();
                    subjectCheckPressed(ui->subjectCheck->isChecked());
                    refreshDisplay();
                }
                break;

            case (Qt::Key_5): //toggle category
                if (e->modifiers() == Qt::ControlModifier) {
                    ui->categoryCheck->toggle();
                    categoryCheckPressed(ui->categoryCheck->isChecked());
                    refreshDisplay();
                }
                break;

            case (Qt::Key_6): //toggle relevance
                if (e->modifiers() == Qt::ControlModifier) {
                    ui->relevanceCheck->toggle();
                    relevanceCheckPressed(ui->relevanceCheck->isChecked());
                    refreshDisplay();
                }
                break;

            case (Qt::Key_7): //toggle semester
                if (e->modifiers() == Qt::ControlModifier) {
                    ui->semesterCheck->toggle();
                    semesterCheckPressed(ui->semesterCheck->isChecked());
                    refreshDisplay();
                }
                break;

            case (Qt::Key_8): //toggle rating
             if (e->modifiers() == Qt::ControlModifier) {
                    ui->ratingCheck->toggle();
                    ratingCheckPressed(ui->ratingCheck->isChecked());
                    refreshDisplay();
                }
                break;


             case (Qt::Key_9): //toggle date
                if (e->modifiers() == Qt::ControlModifier) {
                    ui->dateCheck->toggle();
                    dateCheckPressed(ui->dateCheck->isChecked());
                    refreshDisplay();
                }
                break;



             case (Qt::Key_0): //toggle datum
                if (e->modifiers() == Qt::ControlModifier) {
                    ui->nameCheck->setChecked(true);
                    ui->formulaCheck->setChecked(true);
                    ui->descriptionCheck->setChecked(true);
                    ui->subjectCheck->setChecked(true);
                    ui->categoryCheck->setChecked(true);
                    ui->relevanceCheck->setChecked(true);
                    ui->semesterCheck->setChecked(true);
                    ui->ratingCheck->setChecked(true);
                    ui->dateCheck->setChecked(true);
                    for (int i = 0; i < 9; i++) {
                        infoList[i] = 0;
                    }
                    refreshDisplay();
                }
                break;
        }
    }

    switch (e->key()) {
        case (Qt::Key_Comma): //call settings
            if (e->modifiers() == Qt::ControlModifier)
                settingsDialog();
            break;
    }
}



//====================================================================================
//END
//====================================================================================

MainWindow::~MainWindow()
{
    QString str;
    for (int i = 0; i < infoList.size(); i++) {
        str.append(QString::number(infoList[i]));
    }
    settingsData[5] = str;
    data->saveSettings(settingsData);

    data->backup();

    dialogDestroy();
    delete ui;
}
