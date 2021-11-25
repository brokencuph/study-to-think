#include <QStandardItemModel>
#include <QMessageBox>
#include <algorithm>
#include <cctype>
#include "manualscoredialog.h"
#include "ui_manualscoredialog.h"


ManualScoreDialog::ManualScoreDialog(QWidget* parent,
    const std::vector<Student>* _vStudent, ItemManual* scoreStore, std::string itemName,
    DbSession* currentDb) :
    QDialog(parent),
    ui(new Ui::ManualScoreDialog),
    vStudent(_vStudent),
    scoreStore(scoreStore),
    itemName(std::move(itemName)),
    currentDb(currentDb)
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
    connect(model, &QStandardItemModel::itemChanged, this, &ManualScoreDialog::scoreTableGridEdited);
    ui->tableView->setModel(model);
}

ManualScoreDialog::~ManualScoreDialog()
{
    delete ui;
}

void ManualScoreDialog::scoreTableGridEdited(QStandardItem* item)
{
    int rowIndex = item->index().row();
    auto model = static_cast<QStandardItemModel*>(ui->tableView->model());
    auto stuId = model->item(rowIndex, 0)->text().toUtf8().toStdString();
    //auto stuName = model->item(rowIndex, 1)->text().toUtf8().toStdString();
    auto updatedText = item->text().toUtf8().toStdString();
    if (!std::all_of(updatedText.begin(), updatedText.end(), std::isdigit))
    {
        QMessageBox::critical(this, tr("Invalid Input"), tr("Input could not contain non-digit."));
        item->setText(std::to_string(scoreStore->studentScores[stuId]).c_str());
        return;
    }
    int newScore = atoi(updatedText.c_str());
    if (newScore < 0 || newScore > 100)
    {
        QMessageBox::critical(this, tr("Invalid Input"), tr("Score must be in [0,100]."));
        item->setText(std::to_string(std::max(std::min(newScore, 100), 0)).c_str());
        return;
    }
    scoreStore->studentScores[stuId] = newScore;
    StudentGradeDBO gradeDbo;
    gradeDbo.studentId = stuId;
    gradeDbo.itemName = this->itemName;
    gradeDbo.scoreRepr = std::to_string(newScore);
    currentDb->upsert(gradeDbo);
}
