#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include "qt_defs.h"
#include "db_access.h"
#include "student.h"

#define STUDENT_COLUMN_ID 0
#define STUDENT_COLUMN_NAME 1
#define STUDENT_COLUMN_INFORMATION 2

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void selectDbForOpen(bool checked);

    void newDb(bool checked);

    void uiUpdateForOpeningDb();

    void uiUpdateForClosing();

    void uiAddStudent(bool);

    void uiRemoveStudent(bool);

    void uiEditScheme(const QModelIndex&);

    void studentTableGridEdited(QStandardItem* item);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<DbSession> currentDb;
    std::vector<Student> vStudent;
    std::vector<RatingItem> vScheme;
    std::vector<StudentGradeDBO> vGrade;
};
#endif // MAINWINDOW_H
