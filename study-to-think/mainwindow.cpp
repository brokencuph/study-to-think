#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QVariant>
#include <algorithm>
#include <QInputDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QList>
#include <QLineEdit>
#include <QChart>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include<iostream>
#include <fstream>
#include<set>
#include<iterator>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "manualscoredialog.h"
#include"attendancescoredialog.h"
#include "aboutdialog.h"
#include "db_access.h"
#include "stat_utils.h"
#include "csv_io.h"


static const QStringList overviewNames =
{
    QObject::tr("Distribution"),
    QObject::tr("Statistics")
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentDb(nullptr)
{
    ui->setupUi(this);
    
    ui->comboOverview->addItems(overviewNames);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::selectDbForOpen);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newDb);
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::uiUpdateForClosing);
    connect(ui->actionExport, &QAction::triggered, this, &MainWindow::exportDb);
    connect(ui->actionImport, &QAction::triggered, this, &MainWindow::importDb);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(ui->toolButtonStudentAdd, &QAbstractButton::clicked, this, &MainWindow::uiAddStudent);
    connect(ui->toolButtonStudentRemove, &QAbstractButton::clicked, this, &MainWindow::uiRemoveStudent);
    connect(ui->toolButtonSchemeAdd, &QAbstractButton::clicked, this, &MainWindow::uiAddScheme);
    connect(ui->toolButtonSchemeRemove, &QAbstractButton::clicked, this, &MainWindow::uiRemoveScheme);
    connect(this, &MainWindow::totalScoreUpdated, this, &MainWindow::updateOverviewTab);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectDbForOpen(bool checked)
{
    this->uiUpdateForClosing();
    try
    {
        auto fileName = QFileDialog::getOpenFileName(
            this, tr("Open DB"), QString(), tr("SQLite DB File (*.db)"));
        //QMessageBox::information(this, "hahaha", fileName);
        if (fileName == "")
        {
            return;
        }
        this->currentDb = std::make_unique<DbSession>(fileName.toUtf8().constData());
        this->uiUpdateForOpeningDb();
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, tr("Error"), e.what());
    }
}

void MainWindow::newDb(bool checked)
{
    try
    {
        auto fileName = QFileDialog::getSaveFileName(this,
            tr("Choose DB File Location"), QString(), tr("SQLite DB File (*.db)"));
        if (fileName == "")
        {
            return;
        }
        if (QFile::exists(fileName))
        {
            QFile::remove(fileName);
        }
        this->currentDb = std::make_unique<DbSession>(fileName.toUtf8().constData());
        this->uiUpdateForOpeningDb();
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, tr("Error"), e.what());
    }
}

template <class T>
static inline void writeToCsvFile(DbSession* currentDb, const char* fileName)
{
    using namespace stt::csv_io;
    std::string csvData = dbToCsv<T>(currentDb);
    std::string csvInLocal = QString::fromUtf8(csvData.c_str()).toLocal8Bit().toStdString();
    FILE* file = fopen(fileName, "w");
    fputs(csvInLocal.c_str(), file);
    fclose(file);
}

void MainWindow::exportDb(bool checked)
{
    try
    {
        auto path = QFileDialog::getExistingDirectory(this, tr("Choose a folder to save..."));
        if (path == "")
        {
            return;
        }
        writeToCsvFile<Student>(currentDb.get(), (path.toLocal8Bit().toStdString() + "/student.csv").c_str());
        writeToCsvFile<RatingItem>(currentDb.get(), (path.toLocal8Bit().toStdString() + "/rating_item.csv").c_str());
        writeToCsvFile<StudentGradeDBO>(currentDb.get(), (path.toLocal8Bit().toStdString() + "/grade.csv").c_str());
        QMessageBox::information(this, tr("Success"), tr("Export successfully."));
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, tr("Error"), e.what());
    }
}

template <class T>
static inline void readFromCsvFile(DbSession* db, const char* path)
{
    using namespace stt::csv_io;
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    std::string str = ss.str();
    std::string strInUtf8 = QString::fromLocal8Bit(str.c_str()).toUtf8().toStdString();
    csvToDb<T>(strInUtf8.substr(strInUtf8.find('\n') + 1), db);
}

void MainWindow::importDb(bool checked)
{
    using namespace stt::csv_io;
    try
    {
        auto path = QFileDialog::getExistingDirectory(this, tr("Choose a folder to save..."));
        if (path == "")
        {
            return;
        }
        readFromCsvFile<Student>(currentDb.get(), (path.toLocal8Bit().toStdString() + "/student.csv").c_str());
        readFromCsvFile<RatingItem>(currentDb.get(), (path.toLocal8Bit().toStdString() + "/rating_item.csv").c_str());
        readFromCsvFile<StudentGradeDBO>(currentDb.get(), (path.toLocal8Bit().toStdString() + "/grade.csv").c_str());
        uiUpdateForClosing(false);
        uiUpdateForOpeningDb();
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, tr("Error"), e.what());
    }
}

void MainWindow::uiUpdateForOpeningDb()
{
    this->vScheme.clear();
    this->currentDb->retrieveAll(this->vScheme);
    QStandardItemModel* schemeModel = new QStandardItemModel(this->vScheme.size(), 1);
    for (int i = 0; i < this->vScheme.size(); i++)
    {
        QStandardItem* item = new QStandardItem(
            QString((vScheme[i].name +
                " (" + ItemInfo::typeNames[vScheme[i].item->getCurrentItemType()]
                + ", " + std::to_string(vScheme[i].weight) + "%)").c_str()));
        item->setEditable(false);
        schemeModel->setItem(i, item);
    }
    ui->listScheme->setModel(schemeModel);
    connect(ui->listScheme, &QListView::doubleClicked, this, &MainWindow::uiEditScheme);
    ui->toolButtonSchemeAdd->setEnabled(true);
    ui->toolButtonSchemeRemove->setEnabled(true);


    this->vStudent.clear();
    this->currentDb->retrieveAll(this->vStudent);
    this->vGrade.clear();
    currentDb->retrieveAll(vGrade);
    syncRatingItems();

    QStandardItemModel* stuModel = new QStandardItemModel(this->vStudent.size(), 4);
    for (int i = 0; i < this->vStudent.size(); i++)
    {
        QStandardItem* item[4];
        item[0] = new QStandardItem(QString(vStudent[i].id.c_str()));
        item[0]->setData(QString(vStudent[i].id.c_str()), Qt::UserRole);
        item[0]->setEditable(false);
        item[1] = new QStandardItem(QString(vStudent[i].name.c_str()));
        item[1]->setData(QString(vStudent[i].name.c_str()), Qt::UserRole);
        item[2] = new QStandardItem(QString(vStudent[i].extraInfo.c_str()));
        item[2]->setData(QString(vStudent[i].extraInfo.c_str()), Qt::UserRole);
        //item[3] = new QStandardItem(QString(std::to_string(vStudent[i].getTotalScore(vScheme)).c_str()));
        //item[3]->setEditable(false);
        stuModel->setItem(i, 0, item[0]);
        stuModel->setItem(i, 1, item[1]);
        stuModel->setItem(i, 2, item[2]);
        //stuModel->setItem(i, 3, item[3]);
    }
    stuModel->setHeaderData(0, Qt::Horizontal, tr("Student ID"));
    stuModel->setHeaderData(1, Qt::Horizontal, tr("Student Name"));
    stuModel->setHeaderData(2, Qt::Horizontal, tr("Information"));
    stuModel->setHeaderData(3, Qt::Horizontal, tr("Total Score"));
    connect(stuModel, &QStandardItemModel::itemChanged, this, &MainWindow::studentTableGridEdited);
    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(stuModel);
    proxyModel->setSortRole(Qt::UserRole);
    proxyModel->setFilterKeyColumn(-1);
    proxyModel->setFilterWildcard(ui->lineStudentFilter->text());
    connect(ui->lineStudentFilter, &QLineEdit::textChanged, proxyModel, &QSortFilterProxyModel::setFilterWildcard);
    ui->tableStudent->setModel(proxyModel);
    ui->tableStudent->setSortingEnabled(true);
    ui->tableStudent->sortByColumn(0, Qt::AscendingOrder);
    ui->toolButtonStudentAdd->setEnabled(true);
    ui->toolButtonStudentRemove->setEnabled(true);
    updateTotalScore();
    updateOverviewTab();
    connect(ui->comboOverview, &QComboBox::currentIndexChanged, this, &MainWindow::updateOverviewTab);
    ui->actionImport->setEnabled(true);
    ui->actionExport->setEnabled(true);
}

void MainWindow::uiUpdateForClosing(bool closeDb)
{
    qDeleteAll(ui->widgetOverview->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));
    qDeleteAll(ui->widgetOverview->findChildren<QLayout*>("", Qt::FindDirectChildrenOnly));
    ui->actionExport->setEnabled(false);
    ui->actionImport->setEnabled(false);
    disconnect(ui->comboOverview, nullptr, nullptr, nullptr);
    disconnect(ui->lineStudentFilter, nullptr, nullptr, nullptr);
    ui->toolButtonStudentAdd->setEnabled(false);
    ui->toolButtonStudentRemove->setEnabled(false);
    ui->toolButtonSchemeAdd->setEnabled(false);
    ui->toolButtonSchemeRemove->setEnabled(false);
    QSortFilterProxyModel* proxyModel = static_cast<QSortFilterProxyModel*>(ui->tableStudent->model());
    if (proxyModel != nullptr)
    {
        disconnect(static_cast<QStandardItemModel*>(proxyModel->sourceModel()), nullptr, nullptr, nullptr);
    }
    disconnect(ui->listScheme, nullptr, nullptr, nullptr);
    auto modelStudent = ui->tableStudent->model();
    auto modelScheme = ui->listScheme->model();
    ui->tableStudent->setModel(nullptr);
    ui->listScheme->setModel(nullptr);
    delete modelStudent;
    delete modelScheme;
    if (closeDb)
    {
        this->currentDb = nullptr;
    }
}

void MainWindow::uiAddStudent(bool)
{
    QDialog dialog(this);
    QFormLayout form(&dialog);
    form.addRow(new QLabel("User input:"));
    // Value1
    QString value1 = QString("Student ID: ");
    QLineEdit* qlineedit1 = new QLineEdit(&dialog);
    form.addRow(value1, qlineedit1);
    // Value2
    QString value2 = QString("Student Name: ");
    QLineEdit* qlineedit2 = new QLineEdit(&dialog);
    form.addRow(value2, qlineedit2);
    // Add Cancel and OK button
gotoTagRestudent:
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Process when OK button is clicked
    if (dialog.exec() == QDialog::Accepted) {
        if (!DbSession::checkStringLiteral(qlineedit2->text().toUtf8().toStdString()))
        {
            QMessageBox::critical(this, tr("Invalid Input"), tr("Input could not contain single quote."));

            goto gotoTagRestudent;
        }
        if (!DbSession::checkStringLiteral(qlineedit1->text().toUtf8().toStdString()))
        {
            QMessageBox::critical(this, tr("Invalid Input"), tr("Input could not contain single quote."));

            goto gotoTagRestudent;
        }
        QSortFilterProxyModel* proxyModel = static_cast<QSortFilterProxyModel*>(ui->tableStudent->model());
        QStandardItemModel* x = static_cast<QStandardItemModel*>(proxyModel->sourceModel());

        Student stu(qlineedit2->text().toUtf8().toStdString(), qlineedit1->text().toUtf8().toStdString());
        auto it = std::find_if(vStudent.begin(), vStudent.end(), [&stu](const Student& exist)
            { return exist.id == stu.id; });
        if (it != vStudent.end())
        {
            QMessageBox::critical(this, tr("Error"), tr("Student ID exists."));
            size_t rowNum = it - vStudent.begin();
            size_t proxyRowNum = proxyModel->mapFromSource(x->index(rowNum, 0)).row();
            ui->tableStudent->selectRow(proxyRowNum);
            return;
        }
        this->vStudent.push_back(stu);
        this->currentDb->insert(stu);
        QStandardItem* item[3];
        item[0] = new QStandardItem(QString(stu.id.c_str()));
        item[0]->setData(QString(stu.id.c_str()), Qt::UserRole);
        item[0]->setEditable(false);
        item[1] = new QStandardItem(QString(stu.name.c_str()));
        item[1]->setData(QString(stu.name.c_str()), Qt::UserRole);
        item[2] = new QStandardItem(QString(stu.extraInfo.c_str()));
        item[2]->setData(QString(stu.extraInfo.c_str()), Qt::UserRole);
        
        x->appendRow(QList<QStandardItem*>(item, item + 3));
    }
    syncRatingItems();
    updateTotalScore();
    updateOverviewTab();
}

void MainWindow::uiRemoveStudent(bool)
{
    std::set<int, std::greater<int> > s1;
    std::set<int, std::greater<int>>::iterator s1_it;
    s1_it = s1.begin();
    std::vector<Student>::iterator v_it_1;
    auto selected = ui->tableStudent->selectionModel()->selectedIndexes();
    auto proxyModel = static_cast<QSortFilterProxyModel*>(ui->tableStudent->model());
    for (const auto& x : selected)
    {
        auto y = proxyModel->mapToSource(x);
        s1.insert(y.row());
    }
    for (auto i = s1.begin(); i != s1.end(); i++)
    {
        auto rowIndex = *i;
        QStandardItemModel* x = static_cast<QStandardItemModel*>(proxyModel->sourceModel());
        x->removeRow(rowIndex);
        this->currentDb->removeByKey(vStudent[rowIndex]);
        v_it_1 = vStudent.begin();
        vStudent.erase(v_it_1 + rowIndex);

    }
    updateOverviewTab();
}

void MainWindow::uiAddScheme(bool)
{
    QDialog dialog(this);
    QFormLayout form(&dialog);
    QStringList items;
    items << "Attendance" << "Manual";
    QString dlgTitle = "Selection";
    QString txtLabel = "Which one.";
    int     curIndex = 0;
    bool    editable = true;
    bool    ok = false;
    QInputDialog* qid = new QInputDialog;
    QString value3 = qid->getItem(this, dlgTitle, txtLabel, items, curIndex, false, &ok);
    

    // Add Cancel and OK button
    if (ok)
    {
        
        form.addRow(new QLabel("User input:"));
        // Value1
        QString value1 = QString("Percentage: ");
        QLineEdit* qlineedit1 = new QLineEdit(&dialog);
       
        form.addRow(value1, qlineedit1);
        // Value2
        QString value2 = QString("Input Name: ");
        QLineEdit* qlineedit2 = new QLineEdit(&dialog);
        
        form.addRow(value2, qlineedit2);
    gotoTagReinput:
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
            Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
        if (dialog.exec() == QDialog::Accepted) {

            RatingItem sch;
            if (qlineedit1->text().toInt()<=0 || qlineedit1->text().toInt() >=101)
            {
                QMessageBox::critical(this, tr("Invalid Input"), tr("number Input must be a positive integer from 1 to 100"));
                
                goto gotoTagReinput;
            }
            if (!DbSession::checkStringLiteral(qlineedit2->text().toUtf8().toStdString()))
            {
                QMessageBox::critical(this, tr("Invalid Input"), tr("Input could not contain single quote."));
                
                goto gotoTagReinput;
            }
            sch.weight = qlineedit1->text().toInt();
            sch.name = qlineedit2->text().toUtf8().toStdString();
            auto it = std::find_if(vScheme.begin(), vScheme.end(), [&sch](const RatingItem& exist)
                {
                    return exist.name == sch.name;
                });
            QStandardItemModel* x = static_cast<QStandardItemModel*>(ui->listScheme->model());
            if (it != vScheme.end())
            {
                QMessageBox::critical(this, tr("Error"), tr("Rating item name exists."));
                size_t rowNum = it - vScheme.begin();
                ui->listScheme->setCurrentIndex(x->index(rowNum, 0));
                return;
            }
            if (value3 == "Attendance")
                sch.item = std::make_unique<ItemAttendance>();
            else
                sch.item = std::make_unique<ItemManual>();

            currentDb->insert(sch);
            sch.item->setItemName(sch.name);
            std::string temp1 = qlineedit2->text().toUtf8().toStdString() + " (" + value3.toUtf8().toStdString()
                + ", " + std::to_string(sch.weight) + "%)";
            QString appendString = QString(temp1.c_str());
            QStandardItem* appendStandardItem = new QStandardItem(appendString);
            appendStandardItem->setEditable(false);
            x->appendRow(appendStandardItem);
            vScheme.push_back(std::move(sch));
        }

    }
    syncRatingItems();
    updateTotalScore();
    updateOverviewTab();
}

void MainWindow::uiRemoveScheme(bool)
{
    std::set<int, std::greater<int> > s1;
    std::set<int, std::greater<int>>::iterator s1_it;
    s1_it = s1.begin();
    std::vector<RatingItem>::iterator v_it_1;
    auto selected = ui->listScheme->selectionModel()->selectedIndexes();
    for (const auto& x : selected)
    {
        s1.insert(x.row());
    }
    for (auto i = s1.begin(); i != s1.end(); i++)
    {
        auto rowIndex = *i;
        QStandardItemModel* x = static_cast<QStandardItemModel*>(ui->listScheme->model());
        x->removeRow(rowIndex);
        this->currentDb->removeByKey(vScheme[rowIndex]);
        v_it_1 = vScheme.begin();
        vScheme.erase(v_it_1 + rowIndex);

    }
    syncRatingItems();
    updateTotalScore();
    updateOverviewTab();

}

void MainWindow::uiEditScheme(const QModelIndex& idx)
{
    //QMessageBox::information(this, "msg", std::to_string(idx.row()).c_str());
    RatingItem& item = this->vScheme[idx.row()];
    switch (item.item->getCurrentItemType())
    {
    case 0:
    {
        //QMessageBox::critical(this, tr("Error"), tr("Not implemented"));
        AttendanceScoreDialog dialog(this, &vStudent, static_cast<ItemAttendance*>(item.item.get()), &item, currentDb.get());
        if (dialog.willKeep)
        {
            dialog.exec();
        }
        break; 
    }
    case 1:
        {
            ManualScoreDialog dialog(this, &vStudent, static_cast<ItemManual*>(item.item.get()), item.name, this->currentDb.get());
            dialog.exec();
            //QMessageBox::information(this, tr("Info"), tr("finished"));
        }
        break;
    }
}

void MainWindow::studentTableGridEdited(QStandardItem* item)
{
    int rowIndex = item->index().row();
    //std::cout << rowIndex;
    //Student* thisStu = item->data().value<Student*>();
    std::string updatedStr;
    switch (item->column())
    {
    case STUDENT_COLUMN_ID:
        // not possible
        break;
    case STUDENT_COLUMN_NAME:
        updatedStr = item->text().toUtf8().toStdString();
        if (!DbSession::checkStringLiteral(updatedStr))
        {
            QMessageBox::critical(this, tr("Invalid Input"), tr("Input could not contain single quote."));
            item->setText(vStudent[rowIndex].name.c_str());
            item->setData(QString(vStudent[rowIndex].name.c_str()), Qt::UserRole);
            return;
        }
        vStudent[rowIndex].name = updatedStr;
        this->currentDb->updateByKey(vStudent[rowIndex]);
        break;
    case STUDENT_COLUMN_INFORMATION:
        updatedStr = item->text().toUtf8().toStdString();
        if (!DbSession::checkStringLiteral(updatedStr))
        {
            QMessageBox::critical(this, tr("Invalid Input"), tr("Input could not contain single quote."));
            item->setText(vStudent[rowIndex].extraInfo.c_str());
            item->setData(QString(vStudent[rowIndex].extraInfo.c_str()), Qt::UserRole);
            return;
        }
        vStudent[rowIndex].extraInfo = updatedStr;
        this->currentDb->updateByKey(vStudent[rowIndex]);
        break;
    default:
        break;
    }
}

void MainWindow::updateTotalScore()
{
    auto proxyModel = static_cast<QSortFilterProxyModel*>(ui->tableStudent->model());
    auto model = static_cast<QStandardItemModel*>(proxyModel->sourceModel());
    for (size_t i = 0; i < vStudent.size(); i++)
    {
        auto totalScore = vStudent[i].getTotalScore(vScheme);
        QStandardItem* item = new QStandardItem(QString(totalScore.toString().c_str()));
        item->setData(totalScore.toDouble(), Qt::UserRole);
        item->setEditable(false);
        model->setItem(i, 3, item);
    }
    emit totalScoreUpdated();
}

void MainWindow::syncRatingItems()
{
    vGrade.clear();
    currentDb->retrieveAll(vGrade);
    for (auto& item : vScheme)
    {
        item.item->setStudents(&vStudent);
        item.item->fillScoreFromDb(vGrade);
    }
}

void MainWindow::updateOverviewTab()
{
    int selectedIndex = ui->comboOverview->currentIndex();
    setCursor(QCursor(Qt::BusyCursor));
    ui->widgetOverview->setUpdatesEnabled(false);
    qDeleteAll(ui->widgetOverview->findChildren<QWidget*>("", Qt::FindDirectChildrenOnly));
    qDeleteAll(ui->widgetOverview->findChildren<QLayout*>("", Qt::FindDirectChildrenOnly));
    switch (selectedIndex)
    {
    case 0:
        showChart();
        break;
    case 1:
        showStatistics();
        break;
    default:
        throw std::invalid_argument("Not implemented");
    }
    ui->widgetOverview->setUpdatesEnabled(true);
    setCursor(QCursor(Qt::ArrowCursor));
}

void MainWindow::showChart()
{
    static QStringList categories
    {
        "F", "D", "C-", "C", "C+", "B-", "B", "B+", "A-", "A", "A+"
    };
    QHBoxLayout* layout = new QHBoxLayout(ui->widgetOverview);
    QChartView* view = new QChartView(ui->widgetOverview);
    layout->addWidget(view);
    QBarSeries* series = new QBarSeries();
    QBarSet* barSet = new QBarSet(tr("Total Score"));
    auto countRange = [this]
    {
        static const int seps[] = { 0, 50, 53, 58, 63, 68, 72, 78, 83, 88, 93 };
        std::vector<int> ans(11);
        QSortFilterProxyModel* proxyModel = static_cast<QSortFilterProxyModel*>(ui->tableStudent->model());
        QStandardItemModel* model = static_cast<QStandardItemModel*>(proxyModel->sourceModel());
        for (size_t i = 0; i < vStudent.size(); i++)
        {
            auto scoreRepr = model->item(i, 3)->text().toStdString();
            if (!isdigit(scoreRepr[0]))
            {
                continue;
            }
            int score = stoi(scoreRepr);
            if (score < 0)
            {
                continue;
            }
            int offset = std::upper_bound(std::begin(seps), std::end(seps), score) - 1 - std::begin(seps);
            ans[offset < 0 ? 0 : offset]++;
        }
        return ans;
    };
    auto barData = countRange();
    barSet->append(QList<qreal>(barData.cbegin(), barData.cend()));
    connect(barSet, &QBarSet::clicked, this, &MainWindow::selectStudentAtLevel);
    series->append(barSet);
    view->chart()->addSeries(series);
    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    view->chart()->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, 1 + *std::max_element(barData.cbegin(), barData.cend()));
    view->chart()->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void MainWindow::showStatistics()
{
    using namespace stt::stat_utils;
    QSortFilterProxyModel* proxyModel = static_cast<QSortFilterProxyModel*>(ui->tableStudent->model());
    QStandardItemModel* model = static_cast<QStandardItemModel*>(proxyModel->sourceModel());
    std::vector<int> totalScores;
    for (size_t i = 0; i < vStudent.size(); i++)
    {
        auto scoreRepr = model->item(i, 3)->text().toStdString();
        if (!isdigit(scoreRepr[0]))
        {
            continue;
        }
        totalScores.push_back(
            std::stoi(scoreRepr)
        );
    }
    QFormLayout* formLayout = new QFormLayout(ui->widgetOverview);
    QLabel* labelAvg = new QLabel(qAverageScore(totalScores.cbegin(), totalScores.cend()), ui->widgetOverview);
    formLayout->addRow(tr("Average Score:"), labelAvg);
}

void MainWindow::selectStudentAtLevel(int level)
{
    static const int seps[] = { 0, 50, 53, 58, 63, 68, 72, 78, 83, 88, 93 };
    QSortFilterProxyModel* proxyModel = static_cast<QSortFilterProxyModel*>(ui->tableStudent->model());
    ui->tableStudent->clearSelection();
    QItemSelectionModel* selectionModel = ui->tableStudent->selectionModel();
    for (size_t i = 0; i < proxyModel->rowCount(); i++)
    {
        std::string scoreText = proxyModel->data(proxyModel->index(i, 3)).toString().toStdString();
        if (scoreText.empty() || !isdigit(scoreText[0]))
        {
            continue;
        }
        int scoreValue = std::stoi(scoreText);
        int category;
        if (scoreValue == 0)
        {
            category = 0;
        }
        else
        {
            category = std::upper_bound(std::begin(seps), std::end(seps), scoreValue) - 1 - std::begin(seps);
        }
        if (category == level)
        {
            selectionModel->select(QItemSelection(proxyModel->index(i, 0), proxyModel->index(i, 0)),
                QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    }
    ui->tabWidget->setCurrentIndex(0);
    ui->tableStudent->setFocus();
}

void MainWindow::showAboutDialog(bool checked)
{
    AboutDialog dialog;
    dialog.exec();
}

