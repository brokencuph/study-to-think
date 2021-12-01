#ifndef ATTENDANCESCOREDIALOG_H
#define ATTENDANCESCOREDIALOG_H
#include "student.h"
#include "grading_scheme.h"
#include <QDialog>
#include<QStyledItemDelegate>

namespace Ui {
class AttendanceScoreDialog;
}

class AttendanceScoreDialog : public QDialog
{
    Q_OBJECT

public:
    bool willKeep = true;

    explicit AttendanceScoreDialog(QWidget *parent , const std::vector<Student>* vStudent, ItemAttendance* scoreStore);
    
    ~AttendanceScoreDialog();

private:
    Ui::AttendanceScoreDialog *ui;
    const std::vector<Student>* vStudent;
    ItemAttendance *scoreStore;
};




class ComboxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ComboxDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
        const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
};

#endif // ATTDENCEENTER_H