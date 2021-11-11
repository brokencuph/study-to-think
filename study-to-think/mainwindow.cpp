#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>

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
            stuModel->setItem(i, 0,
                new QStandardItem(QString(vStudent[i].id.c_str())));
            stuModel->setItem(i, 1,
                new QStandardItem(QString(vStudent[i].name.c_str())));
            stuModel->setItem(i, 2,
                new QStandardItem(QString(vStudent[i].extraInfo.c_str())));
        }
        stuModel->setHeaderData(0, Qt::Horizontal, QString("Student ID"));
        stuModel->setHeaderData(1, Qt::Horizontal, QString("Student Name"));
        stuModel->setHeaderData(2, Qt::Horizontal, QString("Information"));
        ui->tableStudent->setModel(stuModel);
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, tr("Error"), e.what());
    }
}

