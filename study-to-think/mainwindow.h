#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include "qt_defs.h"
#include "db_access.h"
#include "student.h"

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

    void studentTableGridEdited(QStandardItem* item);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<DbSession> currentDb;
    std::vector<Student> vStudent;
};
#endif // MAINWINDOW_H
