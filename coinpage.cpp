#include "coinpage.h"
#include "ui_coinpage.h"

CoinPage::CoinPage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CoinPage)
{
    ui->setupUi(this);
}

CoinPage::~CoinPage()
{
    delete ui;
}

void CoinPage::constructor(QNetworkReply *reply) {
    qDebug() << "gay";
}
