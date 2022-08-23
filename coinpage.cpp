#include "coinpage.h"
#include "ui_coinpage.h"

#include <QNetworkReply>

#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

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
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);
    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    ui->CoinName->setText(jsonObj["name"].toString());
    ui->CoinSymbol->setText(jsonObj["symbol"].toString());
    ui->CoinRank->setText("Rank #" + QString::number(jsonObj["market_cap_rank"].toInteger()));
    if(jsonObj["categories"].toArray().size() == 0) {
        ui->CoinType->setVisible(false);
    }else {
    ui->CoinType->setText(jsonObj["categories"].toArray()[0].toString());
    }

    ui->CoinPriceSmall->setText(ui->CoinName->text() + " Price (" + ui->CoinSymbol->text() + ")");
    ui->CoinPrice->setText("$" + QString::number(jsonObj["market_data"].toObject()["current_price"].toObject()["usd"].toDouble(), 'f', 2));

    //color: white;\nbackground-color: rgb(128, 138, 157);\nborder-radius: 5px;\npadding: 10px 15px;\nmargin-left: 15px;
    if(jsonObj["market_data"].toObject()["price_change_percentage_24h"].toDouble() >= 0) {
        ui->CoinChange->setStyleSheet("color: white;\nbackground-color: rgb(61, 174, 35);\nborder-radius: 5px;\npadding: 10px 15px;\nmargin-left: 15px;");
        ui->CoinChange->setText("▲" + QString::number(jsonObj["market_data"].toObject()["price_change_percentage_24h"].toDouble(), 'f', 2) + "%");
    }else {
        ui->CoinChange->setStyleSheet("color: white;\nbackground-color: rgb(208, 2, 27);\nborder-radius: 5px;\npadding: 10px 15px;\nmargin-left: 15px;");
        ui->CoinChange->setText("▼" + QString::number(jsonObj["market_data"].toObject()["price_change_percentage_24h"].toDouble(), 'f', 2) + "%");
    }
    ui->CoinLow->setText("Low(24h):$" + QString::number(jsonObj["market_data"].toObject()["low_24h"].toObject()["usd"].toDouble(), 'f', 2));
    ui->CoinHigh->setText("High(24h):$" + QString::number(jsonObj["market_data"].toObject()["high_24h"].toObject()["usd"].toDouble(), 'f', 2));
}
