#include "discoverwindow.h"
#include "ui_discoverwindow.h"

DiscoverWindow::DiscoverWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DiscoverWindow)
{
    ui->setupUi(this);
}

DiscoverWindow::~DiscoverWindow()
{
    delete ui;
}
