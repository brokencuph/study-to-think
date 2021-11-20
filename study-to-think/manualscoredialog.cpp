#include <QStandardItemModel>
#include "manualscoredialog.h"
#include "ui_manualscoredialog.h"


ManualScoreDialog::ManualScoreDialog(QWidget* parent,
    const std::vector<Student>* _vStudent, ItemManual* scoreStore) :
    QDialog(parent),
    ui(new Ui::ManualScoreDialog),
    vStudent(_vStudent),
    scoreStore(scoreStore)
{
    ui->setupUi(this);
    const std::vector<Student>& vStudent = *_vStudent;
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
        item[2] = new QStandardItem(std::to_string(scoreStore->studentScores.at(vStudent[i].id)).c_str());
        //item[2]->setData(QVariant::fromValue(&vStudent[i]));
        model->setItem(i, 0, item[0]);
        model->setItem(i, 1, item[1]);
        model->setItem(i, 2, item[2]);
    }
    ui->tableView->setModel(model);
}

ManualScoreDialog::~ManualScoreDialog()
{
    delete ui;
}
