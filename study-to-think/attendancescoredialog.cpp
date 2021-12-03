#include "attendancescoredialog.h"
#include "ui_attendancescoredialog.h"
#include "mainwindow.h"
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
//namespace attendanceNamespace {
//    int First_enter = 0;
//    int Accept_switch = 0;
//   
//}

static const QStringList attendanceFormList
{
    "Normal",
    "Late",
    "Early Leave",
    "Absent"
};

AttendanceScoreDialog::AttendanceScoreDialog(QWidget *parent, const std::vector<Student>* _vStudent, ItemAttendance *scoreStore, RatingItem* item, DbSession* currentDb) :
    QDialog(parent),
    vStudent(_vStudent),
    scoreStore(scoreStore),
    ratingItem(item),
    currentDb(currentDb),
    ui(new Ui::AttendanceScoreDialog)
{
    ui->setupUi(this);
    const std::vector<Student>& vStudent = *_vStudent;
    //QStringList attendanceFormList;
    if (scoreStore->getSessionNumber()==0) // current session number invalid
    {
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
        int Accept_switch = dialog.exec();
        if (Accept_switch) {
            //Accept_switch = 1;
            //First_enter++; // will not enter next window until enter the number of class
            //ui->setupUi(this);
            //QStringList attendanceFormList;
            QStringList ClassIndexList;
            //attendanceFormList << "Normal" << "Late" << "Earlyleave" << "Absent";
            scoreStore->modifySessionNumber(stoi(qlineedit1->text().toUtf8().toStdString()));
            currentDb->updateByKey(*item);
        }
        else
        {
            willKeep = false;
            return;
        }
        
    }
    QStringList ClassIndexList;
    //attendanceFormList << "Normal" << "Late" << "Earlyleave" << "Absent";
    for (int i = 1; i < scoreStore->getSessionNumber() + 1; i++)
    {
        std::string addString = "Lesson " + std::to_string(i);
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
            [ui->comboBox_2->currentIndex()];

        //item[2] = new QStandardItem(checkInTypeNames[choice]);
        //item[2]->setData(QVariant::fromValue(&vStudent[i]));

        model->setData(model->index(i, 2), tr(checkInTypeNames[choice]), Qt::EditRole);
        //model->setData(model->index(i, 2), tr(checkInTypeNames[choice]), Qt::DisplayRole);
        model->setItem(i, 0, item[0]);
        model->setItem(i, 1, item[1]);
        //model->setItem(i, 2, item[2]);

    }
    ui->tableView->setModel(model);
    ComboxDelegate* delegate = new ComboxDelegate;
    ui->tableView->setItemDelegate(delegate);
    connect(ui->comboBox_2, &QComboBox::currentIndexChanged, this, &AttendanceScoreDialog::updateModel);
    connect(model, &QAbstractItemModel::dataChanged, this, &AttendanceScoreDialog::updateAttendance);
    connect(this, &AttendanceScoreDialog::scoreChanged,
        static_cast<MainWindow*>(this->parent()), &MainWindow::updateTotalScore);
}

AttendanceScoreDialog::~AttendanceScoreDialog()
{
    delete ui;
}

void AttendanceScoreDialog::updateModel(int index)
{
    const StudentVector& vStudent = *this->vStudent;
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->tableView->model());
    for (int i = 0; i < vStudent.size(); i++)
    {
        int choice = (int)scoreStore->studentAttendance.at(vStudent[i].id)
            [index];

        //item[2] = new QStandardItem(checkInTypeNames[choice]);
        //item[2]->setData(QVariant::fromValue(&vStudent[i]));

        model->setData(model->index(i, 2), tr(checkInTypeNames[choice]), Qt::EditRole);
        //model->setData(model->index(i, 2), tr(checkInTypeNames[choice]), Qt::DisplayRole);
    }

}

void AttendanceScoreDialog::updateAttendance(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{
    assert(topLeft == bottomRight);
    CheckInType choice = checkInTypeIds.at(topLeft.data(Qt::EditRole).toString().toUtf8().toStdString());
    int row = topLeft.row();
    const Student& stu = (*vStudent)[row];
    scoreStore->studentAttendance[stu.id][ui->comboBox_2->currentIndex()] = choice;
    StudentGradeDBO dbo;
    dbo.studentId = stu.id;
    dbo.itemName = scoreStore->getItemName();
    dbo.scoreRepr = ItemAttendance::scoreRepr(scoreStore->studentAttendance[stu.id]);
    currentDb->upsert(dbo);
    emit scoreChanged();
}   

QWidget* ComboxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QComboBox* editor = new QComboBox(parent);
    QStringList attendanceFormList;
    attendanceFormList << "Normal" << "Late" << "Early Leave" << "Absent";
    editor->addItems(attendanceFormList);
    return editor;

}

void ComboxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    auto value = index.data(Qt::EditRole).toString();
    QComboBox * comboBox=static_cast<QComboBox*>(editor);
    comboBox->setCurrentText(value);
}

void ComboxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);

    auto value = comboBox->currentText();
    QVariant var = value;
    model->setData(index, var, Qt::EditRole);
    //model->setData(index, tr(checkInTypeNames[value]), Qt::DisplayRole);
}

void ComboxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}


ComboxDelegate::ComboxDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
    
}