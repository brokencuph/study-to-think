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

static const QStringList attendanceEmojis // encoded in UTF-8
{
    "\xe2\x9c\x93", // a tick mark, U+2713
    "\xf0\x9f\x95\x98", // clock face 9 o'clock, U+1F558
    "\xf0\x9f\x95\xa4", // clock face 9:30, U+1F564
    "\xe2\x9d\x8c", // a cross mark, U+274C
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
    setUpdatesEnabled(false);
    const std::vector<Student>& vStudent = *_vStudent;
    //QStringList attendanceFormList;
    if (scoreStore->getSessionNumber()==0) // current session number invalid
    {
        QDialog dialog(this);
        QFormLayout form(&dialog);
        form.addRow(new QLabel("Please enter the number of lessons of this course:"));
        QLineEdit* qlineedit1 = new QLineEdit(&dialog);
        form.addRow(qlineedit1);
gotoNumberOfLesson:
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
           
            if (qlineedit1->text().toInt() <= 0 || qlineedit1->text().toInt() >= 101)
            {
                QMessageBox::critical(this, tr("Invalid Input"), tr("The number of lesson should be in range of [1,100]!"));

                goto gotoNumberOfLesson;
            }
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
    QStandardItemModel* model = new QStandardItemModel(vStudent.size(), 4);
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
        model->setData(model->index(i, 2), tr(checkInTypeNames[choice]), Qt::EditRole);
        //model->setData(model->index(i, 3), QString::fromStdString(scoreStore->getScore(vStudent[i]).toString()));
        //model->setData(model->index(i, 2), tr(checkInTypeNames[choice]), Qt::DisplayRole);
        model->setItem(i, 0, item[0]);
        model->setItem(i, 1, item[1]);
        item[2] = new QStandardItem(QString::fromStdString(scoreStore->getScore(vStudent[i]).toString()));
        item[2]->setEditable(false);
        model->setItem(i, 3, item[2]);

    }
    ui->tableView->setModel(model);
    ComboxDelegate* delegate = new ComboxDelegate;
    ui->tableView->setItemDelegate(delegate);
    connect(ui->comboBox_2, &QComboBox::currentIndexChanged, this, &AttendanceScoreDialog::updateModel);
    connect(model, &QAbstractItemModel::dataChanged, this, &AttendanceScoreDialog::updateAttendance);
    connect(this, &AttendanceScoreDialog::scoreChanged,
        static_cast<MainWindow*>(this->parent()), &MainWindow::updateTotalScore);

    QStandardItemModel* modelOverview = new QStandardItemModel(vStudent.size(), 3 + scoreStore->getSessionNumber());
    QStringList overviewHeader = { tr("Student ID"), tr("Student Name") };
    for (int i = 1; i <= scoreStore->getSessionNumber(); i++)
    {
        overviewHeader.append(std::to_string(i).c_str());
    }
    overviewHeader.append(tr("Score"));
    modelOverview->setHorizontalHeaderLabels(overviewHeader);
    for (size_t i = 0; i < vStudent.size(); i++)
    {
        QStandardItem* item[3];
        item[0] = new QStandardItem(QString(vStudent[i].id.c_str()));
        //item[0]->setData(QVariant::fromValue(&vStudent[i]));
        item[0]->setEditable(false);
        item[1] = new QStandardItem(QString(vStudent[i].name.c_str()));
        //item[1]->setData(QVariant::fromValue(&vStudent[i]));
        item[1]->setEditable(false);
        modelOverview->setItem(i, 0, item[0]);
        modelOverview->setItem(i, 1, item[1]);
        item[2] = new QStandardItem(QString::fromStdString(scoreStore->getScore(vStudent[i]).toString()));
        item[2]->setEditable(false);
        modelOverview->setItem(i, 2 + scoreStore->getSessionNumber(), item[2]);
        for (int j = 0; j < scoreStore->getSessionNumber(); j++)
        {
            QStandardItem* item = new QStandardItem;
            CheckInType choice = scoreStore->studentAttendance.at(vStudent[i].id)[j];
            QString dispText = attendanceEmojis[(int)choice];
            item->setForeground(Qt::darkGreen);
            item->setText(dispText);
            item->setToolTip(checkInTypeNames[(int)choice]);
            item->setEditable(false);
            modelOverview->setItem(i, j + 2, item);
        }
    }
    ui->tableOverview->setModel(modelOverview);
    ui->tableOverview->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    setUpdatesEnabled(true);
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
    if (topLeft.column() != 2)
    {
        return;
    }
    CheckInType choice = checkInTypeIds.at(topLeft.data(Qt::EditRole).toString().toUtf8().toStdString());
    int row = topLeft.row();
    const Student& stu = (*vStudent)[row];
    scoreStore->studentAttendance[stu.id][ui->comboBox_2->currentIndex()] = choice;
    StudentGradeDBO dbo;
    dbo.studentId = stu.id;
    dbo.itemName = scoreStore->getItemName();
    dbo.scoreRepr = ItemAttendance::scoreRepr(scoreStore->studentAttendance[stu.id]);
    currentDb->upsert(dbo);
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->tableView->model());
    model->item(row, 3)->setText(QString::fromStdString(scoreStore->getScore(stu).toString()));

    QStandardItemModel* overviewModel = static_cast<QStandardItemModel*>(ui->tableOverview->model());
    overviewModel->item(row, 2 + ui->comboBox_2->currentIndex())->setText(attendanceEmojis[(int)choice]);
    overviewModel->item(row, 2 + ui->comboBox_2->currentIndex())->setToolTip(checkInTypeNames[(int)choice]);
    overviewModel->item(row, 2 + scoreStore->getSessionNumber())->setText(
        QString::fromStdString(scoreStore->getScore(stu).toString())
    );
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