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
    
    QObject::connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::selectDbForOpen);
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
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, tr("Error"), e.what());
    }
}

void MainWindow::studentTableGridEdited(QStandardItem* item)
{
    // TODO: finish it
    Student* thisStu = item->data().value<Student*>();
    switch (item->column())
    {
    case STUDENT_COLUMN_ID:
        // not possible
        break;
    case STUDENT_COLUMN_NAME:
        thisStu->name = item->text().toUtf8().toStdString();
        this->currentDb->updateByKey(*thisStu);
        break;
    case STUDENT_COLUMN_INFORMATION:
        thisStu->extraInfo = item->text().toUtf8().toStdString();
        this->currentDb->updateByKey(*thisStu);
        break;
    default:
        break;
    }
}

