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
int First_enter = 0;
AttendanceScoreDialog::AttendanceScoreDialog(QWidget *parent, const std::vector<Student>* _vStudent) :
    QDialog(parent),
    ui(new Ui::AttendanceScoreDialog)
{
    const std::vector<Student>& vStudent = *_vStudent;
    if (!First_enter)
    {
        QDialog dialog(this);
        QFormLayout form(&dialog);
        form.addRow(new QLabel("Please enter the number of class of this course:"));
        QLineEdit* qlineedit1 = new QLineEdit(&dialog);
        form.addRow( qlineedit1);
        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
            Qt::Horizontal, &dialog);
        form.addRow(&buttonBox);
        QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
        QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
        if (dialog.exec() == QDialog::Accepted) {
            First_enter++; // will not enter next window until enter the number of class
            ui->setupUi(this);
            QStringList attendanceFormList;
            QStringList ClassIndexList;
            attendanceFormList << "Normal" << "Late" << "Earlyleave" << "Absent";
            ui->comboBox->addItems(attendanceFormList);
        }
       
        
    }
    ui->setupUi(this);
    QStringList attendanceFormList;
    QStringList ClassIndexList;
    attendanceFormList << "Normal" << "Late" << "Earlyleave" << "Absent";
    ui->comboBox->addItems(attendanceFormList);
    
    
}

AttendanceScoreDialog::~AttendanceScoreDialog()
{
    delete ui;
}
