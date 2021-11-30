#include "attendancescoredialog.h"
#include "ui_attendancescoredialog.h"
#include "student.h"
#include "cli_main.h"
#include <QStandardItemModel>
#include <QMessageBox>
#include <algorithm>
#include <QStringList>
#include <QLabel>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include<string>
namespace attendanceNamespace {
    int First_enter = 0;
    int Accept_switch = 0;
    int lessonNumber = 0;
}
AttendanceScoreDialog::AttendanceScoreDialog(QWidget *parent, const std::vector<Student>* _vStudent) :
    QDialog(parent),
    ui(new Ui::AttendanceScoreDialog)
{
    if (attendanceNamespace::Accept_switch) {
        ui->setupUi(this);
        QStringList attendanceFormList;
        QStringList ClassIndexList;
        attendanceFormList << "Normal" << "Late" << "Earlyleave" << "Absent";
        ui->comboBox->addItems(attendanceFormList);
        ui->comboBox->addItems(attendanceFormList);
        for (int i = 1; i < attendanceNamespace::lessonNumber + 1; i++)
        {
            std::string addString = "lesson " + std::to_string(i);
            QString qAddString = addString.c_str();
            ClassIndexList << qAddString;

        }

        ui->comboBox_2->addItems(ClassIndexList);
    }
    if (!attendanceNamespace::First_enter)
    {

        const std::vector<Student>& vStudent = *_vStudent;
        QDialog dialog(this);
        QFormLayout form(&dialog);
        form.addRow(new QLabel("Please enter the number of lessons of this course:"));
        QLineEdit* qlineedit1 = new QLineEdit(&dialog);
        form.addRow(qlineedit1);
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
            Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
        attendanceNamespace::Accept_switch = dialog.exec();
        if (attendanceNamespace::Accept_switch) {
            attendanceNamespace::Accept_switch = 1;
            attendanceNamespace::First_enter++; // will not enter next window until enter the number of class
            ui->setupUi(this);
            QStringList attendanceFormList;
            QStringList ClassIndexList;
            attendanceFormList << "Normal" << "Late" << "Earlyleave" << "Absent";
            ui->comboBox->addItems(attendanceFormList);
            attendanceNamespace::lessonNumber = stoi(qlineedit1->text().toUtf8().toStdString());
            for (int i = 1; i < attendanceNamespace::lessonNumber+1; i++)
            {
                std::string addString = "lesson " + std::to_string(i);
                QString qAddString = addString.c_str();
                ClassIndexList << qAddString;
                
            }
            
            ui->comboBox_2->addItems(ClassIndexList);
        }
        else
        {
            
            this->close();
        }
        
    }
    
    
    
    
}

AttendanceScoreDialog::~AttendanceScoreDialog()
{
    delete ui;
}
