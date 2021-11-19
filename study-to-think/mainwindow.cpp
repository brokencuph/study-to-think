#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QVariant>
#include <algorithm>

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "db_access.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentDb(nullptr)
{
    ui->setupUi(this);
    
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::selectDbForOpen);
    connect(ui->toolButtonStudentAdd, &QAbstractButton::clicked, this, &MainWindow::uiAddStudent);
    connect(ui->toolButtonStudentRemove, &QAbstractButton::clicked, this, &MainWindow::uiRemoveStudent);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::selectDbForOpen(bool checked)
{
    try
    {
        auto fileName = QFileDialog::getOpenFileName(
            this, tr("Open DB"), QString(), tr("SQLite DB File (*.db)"));
        QMessageBox::information(this, "hahaha", fileName);
        this->currentDb = std::make_unique<DbSession>(fileName.toStdString().c_str());
        this->vStudent.clear();
        this->currentDb->retrieveAll(this->vStudent);
        QStandardItemModel* stuModel = new QStandardItemModel(this->vStudent.size(), 3);
        for (int i = 0; i < this->vStudent.size(); i++)
        {
            QStandardItem* item[3];
            item[0] = new QStandardItem(QString(vStudent[i].id.c_str()));
            item[0]->setData(QVariant::fromValue(&vStudent[i]));
            item[0]->setEditable(false);
            item[1] = new QStandardItem(QString(vStudent[i].name.c_str()));
            item[1]->setData(QVariant::fromValue(&vStudent[i]));
            item[2] = new QStandardItem(QString(vStudent[i].extraInfo.c_str()));
            item[2]->setData(QVariant::fromValue(&vStudent[i]));
            stuModel->setItem(i, 0, item[0]);
            stuModel->setItem(i, 1, item[1]);
            stuModel->setItem(i, 2, item[2]);
        }
        stuModel->setHeaderData(0, Qt::Horizontal, QString("Student ID"));
        stuModel->setHeaderData(1, Qt::Horizontal, QString("Student Name"));
        stuModel->setHeaderData(2, Qt::Horizontal, QString("Information"));
        
        connect(stuModel, &QStandardItemModel::itemChanged, this, &MainWindow::studentTableGridEdited);
        ui->tableStudent->setModel(stuModel);
        ui->toolButtonStudentAdd->setEnabled(true);
        ui->toolButtonStudentRemove->setEnabled(true);

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
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, tr("Error"), e.what());
    }
}

void MainWindow::uiAddStudent(bool)
{
    
    //Student stu;
    //this->vStudent.push_back(stu);
    //this->currentDb->insert(stu);
    //QStandardItemModel* x = static_cast<QStandardItemModel*>(ui->tableStudent->model());
}

void MainWindow::uiRemoveStudent(bool)
{
    //Student stu;
    //stu.id = "111";
    //this->currentDb->removeByKey(stu);
}

void MainWindow::uiEditScheme(const QModelIndex& idx)
{
    QMessageBox::information(this, "msg", std::to_string(idx.row()).c_str());
}

void MainWindow::studentTableGridEdited(QStandardItem* item)
{
    Student* thisStu = item->data().value<Student*>();
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
            item->setText(thisStu->name.c_str());
            return;
        }
        thisStu->name = updatedStr;
        this->currentDb->updateByKey(*thisStu);
        break;
    case STUDENT_COLUMN_INFORMATION:
        updatedStr = item->text().toUtf8().toStdString();
        if (!DbSession::checkStringLiteral(updatedStr))
        {
            QMessageBox::critical(this, tr("Invalid Input"), tr("Input could not contain single quote."));
            item->setText(thisStu->extraInfo.c_str());
            return;
        }
        thisStu->extraInfo = updatedStr;
        this->currentDb->updateByKey(*thisStu);
        break;
    default:
        break;
    }
}

