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
#include <QLineEdit>
#include<iostream>

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
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, tr("Error"), e.what());
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
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Process when OK button is clicked
    if (dialog.exec() == QDialog::Accepted) {
        
        Student stu(qlineedit1->text().toUtf8().toStdString(), qlineedit2->text().toUtf8().toStdString());
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

     
}

void MainWindow::uiRemoveStudent(bool)
{
    //Student stu;
    //stu.id = "111";
    //this->currentDb->removeByKey(stu);
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

