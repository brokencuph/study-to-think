#include <QFormLayout>
#include <QLayout>
#include <QLabel>

#include "stt_config.h"
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QFormLayout* layout = new QFormLayout(this);
    QLabel* logoLabel = new QLabel(this);
    logoLabel->setPixmap(QPixmap("imgs/logo.png"));
    layout->addRow(logoLabel);
    QLabel* nameLabel = new QLabel(this);
    nameLabel->setText(tr("Student Course Assessment System"));
    layout->addRow(nameLabel);
    QLabel* versionLabel = new QLabel(this);
    versionLabel->setText(tr("Version: ") + STT_VERSION_STRING);
    layout->addRow(versionLabel);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
