#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>
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
#include<set>
#include<iterator>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "manualscoredialog.h"
#include"attendancescoredialog.h"
#include "db_access.h"
#include "stat_utils.h"
#include <QMap>

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
        item[0]->setData(QVariant::fromValue(&vStudent[i]));
        item[0]->setEditable(false);
        item[1] = new QStandardItem(QString(vStudent[i].name.c_str()));
        item[1]->setData(QVariant::fromValue(&vStudent[i]));
        item[2] = new QStandardItem(QString(vStudent[i].extraInfo.c_str()));
        item[2]->setData(QVariant::fromValue(&vStudent[i]));
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
    ui->tableStudent->setModel(stuModel);
    ui->toolButtonStudentAdd->setEnabled(true);
    ui->toolButtonStudentRemove->setEnabled(true);
    updateTotalScore();
    updateOverviewTab();
    connect(ui->comboOverview, &QComboBox::currentIndexChanged, this, &MainWindow::updateOverviewTab);
}

void MainWindow::uiUpdateForClosing()
{
    disconnect(ui->comboOverview, nullptr, nullptr, nullptr);
    ui->toolButtonStudentAdd->setEnabled(false);
    ui->toolButtonStudentRemove->setEnabled(false);
    ui->toolButtonSchemeAdd->setEnabled(false);
    ui->toolButtonSchemeRemove->setEnabled(false);
    disconnect(static_cast<QStandardItemModel*>(ui->tableStudent->model()), nullptr, nullptr, nullptr);
    disconnect(ui->listScheme, nullptr, nullptr, nullptr);
    auto modelStudent = ui->tableStudent->model();
    auto modelScheme = ui->listScheme->model();
    ui->tableStudent->setModel(nullptr);
    ui->listScheme->setModel(nullptr);
    delete modelStudent;
    delete modelScheme;
    this->currentDb = nullptr;
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
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Process when OK button is clicked
    if (dialog.exec() == QDialog::Accepted) {
        
        Student stu(qlineedit2->text().toUtf8().toStdString(), qlineedit1->text().toUtf8().toStdString());
        this->vStudent.push_back(stu);
        this->currentDb->insert(stu);
        QStandardItemModel* x = static_cast<QStandardItemModel*>(ui->tableStudent->model());
        QStandardItem* item[3];
        item[0] = new QStandardItem(QString(stu.id.c_str()));
        item[0]->setData(QVariant::fromValue(&vStudent[vStudent.size()-1]));
        item[0]->setEditable(false);
        item[1] = new QStandardItem(QString(stu.name.c_str()));
        item[1]->setData(QVariant::fromValue(&vStudent[vStudent.size() - 1]));
        item[2] = new QStandardItem(QString(stu.extraInfo.c_str()));
        item[2]->setData(QVariant::fromValue(&vStudent[vStudent.size() - 1]));
        
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
    for (const auto& x : selected)
    {

        s1.insert(x.row());
    }
    for (auto i = s1.begin(); i != s1.end(); i++)
    {
        auto rowIndex = *i;
        QStandardItemModel* x = static_cast<QStandardItemModel*>(ui->tableStudent->model());
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
    form.addRow(new QLabel("User input:"));
    // Value1
    QString value1 = QString("Percentage: ");
    QLineEdit* qlineedit1 = new QLineEdit(&dialog);
    form.addRow(value1, qlineedit1);
    // Value2
    QString value2 = QString("Input Name: ");
    QLineEdit* qlineedit2 = new QLineEdit(&dialog);
    form.addRow(value2, qlineedit2);
    QStringList items;
    items << "Attendance" << "Manual" ;
    QString dlgTitle = "Selection";
    QString txtLabel = "Which one.";
    int     curIndex = 0;
    bool    editable = true;
    bool    ok = false;
    QString value3 = QInputDialog::getItem(this, dlgTitle, txtLabel, items, curIndex, editable, &ok);
    
    //QLineEdit* qlineedit3 = new QLineEdit(&dialog);
    //form.addRow(value3, qlineedit3);
    // Add Cancel and OK button
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Process when OK button is clicked
    if (dialog.exec() == QDialog::Accepted) {

        RatingItem sch;
        sch.weight = qlineedit1->text().toInt();
        sch.name = qlineedit2->text().toUtf8().toStdString();
        if (value3 == "Attendance")
            sch.item = std::make_unique<ItemAttendance>();
        else
            sch.item = std::make_unique<ItemManual>();
 
        currentDb->insert(sch);
        vScheme.push_back(std::move(sch));
        QStandardItemModel* x = static_cast<QStandardItemModel*>(ui->listScheme->model());
        std::string temp1 = qlineedit2->text().toUtf8().toStdString() + " (" + value3.toUtf8().toStdString()
            + ", " + std::to_string(sch.weight) + "%)";
        QString *appendString=new QString(temp1.c_str());
        QStandardItem* appendStandardItem = new QStandardItem(*appendString);
        
        x->appendRow(appendStandardItem);
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
            item->setText(vStudent[rowIndex].name.c_str()); ;
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
    auto model = static_cast<QStandardItemModel*>(ui->tableStudent->model());
    for (size_t i = 0; i < vStudent.size(); i++)
    {
        QStandardItem* item = new QStandardItem(QString(std::to_string(vStudent[i].getTotalScore(vScheme)).c_str()));
        item->setEditable(false);
        model->setItem(i, 3, item);
    }
    emit totalScoreUpdated();
}

void MainWindow::syncRatingItems()
{
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
        "F\n(0-49)", "D", "C-", "C", "C+", "B-", "B", "B+", "A-", "A", "A+"
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
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->tableStudent->model());
        for (size_t i = 0; i < vStudent.size(); i++)
        {
            int score = stoi(model->item(i, 3)->text().toStdString());
            if (score < 0)
            {
                continue;
            }
            size_t offset = std::lower_bound(std::begin(seps), std::end(seps), score) - 1 - std::begin(seps);
            ans[offset]++;
        }
        return ans;
    };
    auto barData = countRange();
    barSet->append(QList<qreal>(barData.cbegin(), barData.cend()));
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
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->tableStudent->model());
    std::vector<int> totalScores;
    for (size_t i = 0; i < vStudent.size(); i++)
    {
        totalScores.push_back(
            std::stoi(model->item(i, 3)->text().toStdString())
        );
    }
    QFormLayout* formLayout = new QFormLayout(ui->widgetOverview);
    QLabel* labelAvg = new QLabel(qAverageScore(totalScores.cbegin(), totalScores.cend()), ui->widgetOverview);
    formLayout->addRow(tr("Average Score:"), labelAvg);
}

