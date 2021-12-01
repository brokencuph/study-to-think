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
#include <QMessageBox>
#include <QStandardItemModel>
#include<string>
namespace attendanceNamespace {
    int First_enter = 0;
    int Accept_switch = 0;
   
}
AttendanceScoreDialog::AttendanceScoreDialog(QWidget *parent, const std::vector<Student>* _vStudent, ItemAttendance *scoreStore) :
    QDialog(parent),
    vStudent(_vStudent),
    scoreStore(scoreStore),
    ui(new Ui::AttendanceScoreDialog)
{
    if (attendanceNamespace::Accept_switch) {
        ui->setupUi(this);
        QStringList attendanceFormList;
        QStringList ClassIndexList;
        attendanceFormList << "Normal" << "Late" << "Earlyleave" << "Absent";
        ui->comboBox->addItems(attendanceFormList);
        ui->comboBox->addItems(attendanceFormList);
        for (int i = 1; i < scoreStore->getSessionNumber() + 1; i++)
        {
            std::string addString = "lesson " + std::to_string(i);
            QString qAddString = addString.c_str();
            ClassIndexList << qAddString;

        }

        ui->comboBox_2->addItems(ClassIndexList);
    }
    if (scoreStore->getSessionNumber()==15)
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
            scoreStore->modifySessionNumber(stoi(qlineedit1->text().toUtf8().toStdString())) ;
            for (int i = 1; i < scoreStore->getSessionNumber() +1; i++)
            {
                std::string addString = "lesson " + std::to_string(i);
                QString qAddString = addString.c_str();
                ClassIndexList << qAddString;
                
            }
            
            ui->comboBox_2->addItems(ClassIndexList);
            QStandardItemModel* model = new QStandardItemModel(vStudent.size(), 3);
            for (int i = 0; i < vStudent.size(); i++)
            {
                QStandardItem* item[3];
                item[0] = new QStandardItem(QString(vStudent[i].id.c_str()));
                //item[0]->setData(QVariant::fromValue(&vStudent[i]));
                item[0]->setEditable(false);
                item[1] = new QStandardItem(QString(vStudent[i].name.c_str()));
                //item[1]->setData(QVariant::fromValue(&vStudent[i]));
                item[1]->setEditable(false);
                int choice = (int)scoreStore->studentAttendance.at(vStudent[i].id)
                    [ui->comboBox->currentIndex()];

                item[2] = new QStandardItem(checkInTypeNames[choice]);
                //item[2]->setData(QVariant::fromValue(&vStudent[i]));

                model->setData(model->index(i, 2), QVariant(choice));
                model->setItem(i, 0, item[0]);
                model->setItem(i, 1, item[1]);
                model->setItem(i, 2, item[2]);

            }
            ui->tableView->setModel(model);
            ComboxDelegate *delegate=new ComboxDelegate;
            ui->tableView->setItemDelegate(delegate);
        }
        else
        {
            willKeep = false;
        }
        
    }
    
    
    
    
}

AttendanceScoreDialog::~AttendanceScoreDialog()
{
    delete ui;
}

QWidget* ComboxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QComboBox* editor = new QComboBox(parent);
    QStringList attendanceFormList;
    attendanceFormList << "Normal" << "Late" << "Earlyleave" << "Absent";
    editor->addItems(attendanceFormList);

    return editor;

}

void ComboxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QComboBox * comboBox=static_cast<QComboBox*>(editor);
    comboBox->setCurrentIndex(value);
}

void ComboxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);

    int value = comboBox->currentIndex();
    model->setData(index, value, Qt::EditRole);

}

void ComboxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}


ComboxDelegate::ComboxDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{

}