#ifndef ATTENDANCESCOREDIALOG_H
#define ATTENDANCESCOREDIALOG_H
#include "student.h"
#include "grading_scheme.h"
#include <QDialog>

namespace Ui {
class AttendanceScoreDialog;
}

class AttendanceScoreDialog : public QDialog
{
    Q_OBJECT

public:
    bool willKeep = true;

    explicit AttendanceScoreDialog(QWidget *parent , const std::vector<Student>* vStudent);
    
    ~AttendanceScoreDialog();

private:
    Ui::AttendanceScoreDialog *ui;
    const std::vector<Student>* vStudent;
};

#endif // ATTDENCEENTER_H
