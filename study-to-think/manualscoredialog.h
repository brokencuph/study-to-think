#ifndef MANUALSCOREDIALOG_H
#define MANUALSCOREDIALOG_H

#include <vector>
#include <QDialog>
#include "student.h"
#include "grading_scheme.h"

namespace Ui {
class ManualScoreDialog;
}

class ManualScoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManualScoreDialog(QWidget *parent, const std::vector<Student>* vStudent, ItemManual* scoreStore);
    ~ManualScoreDialog();

private:
    Ui::ManualScoreDialog *ui;
    const std::vector<Student>* vStudent;
    ItemManual* scoreStore;
};

#endif // MANUALSCOREDIALOG_H
