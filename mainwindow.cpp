#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "coin.h"
#include "coinpage.h"

#include <QApplication>
#include <QtCore>
#include <QtGui>


#include <QNetworkReply>

#include <QChart>
#include <QLineSeries>
#include <QPieSeries>

#include <QSizePolicy>
#include <QScreen>

#include <QCategoryAxis>

#include <QTime>

#include <algorithm>
#include <sstream>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //centerScreen();

    manager = new QNetworkAccessManager();
    ImgManager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerFinished(QNetworkReply*)));
    QObject::connect(ImgManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerImgFinished(QNetworkReply*)));


    connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    connect(ImgManager, SIGNAL(finished(QNetworkReply*)), &imgloop, SLOT(quit()));
    connect(manager, SIGNAL(finished(QNetworkReply*)), &coinLoop, SLOT(quit()));

    requestTrendings();
    loop.exec();
    requestHeader();
    loop.exec();
    requestGainers();
    loop.exec();
    requestLosers();
    loop.exec();
    requestMain();
    loop.exec();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::centerScreen() {
    QScreen *screen = ui->centralwidget->screen();
    QRect  screenGeometry = screen->geometry();
    int sheight = screenGeometry.height();
    int swidth = screenGeometry.width();
    int height = ui->centralwidget->height();
    int width = ui->centralwidget->width();
    move(  (swidth - width) / 2 ,(sheight - height) / 2 );
}

void MainWindow::resetChoices() {
    reqHeader = false;
    reqTrendings = false;
    reqGainers = false;
    reqLosers = false;
    reqMain = false;
    reqCoin = false;
    reqCoinPage = false;
}

void MainWindow::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error: " << reply->error() <<
                    ", Message: " << reply->errorString() <<
                    ", Code: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    }

    if(reqTrendings) {
        updateTrendings(reply);
        resetChoices();
    }
    if(reqHeader) {
        QMap<QString, double> totalMarket = coinMarketApi->getTotalCap(reply);
        updateHeader(totalMarket);
    }

    if(reqGainers) {
        updateGainers(reply);
    }

    if(reqLosers) {
        updateLosers(reply);
    }

    if(reqMain) {
        updateMain(reply);
    }
    if(reqCoinPage) {
        CoinPage *cp = new CoinPage();
        cp->constructor(reply);
        this->hide();
        cp->show();
    }
    resetChoices();
}

QString MainWindow::doubleFormat(double n) {
    QLocale locale(QLocale::English);
    QString helper = locale.toString(n, 'f', 3);
    return helper;
}

void MainWindow::percentChangeHeader(QString item, double n) {
    if(item == "marketCap") {
        if(n > 0) {
            ui->marketCapChange->setStyleSheet("color: rgb(61, 174, 35)");
            ui->marketCapChange->setText("▲ " + QString::number(n, 'f', 2) + "%");
        }else if(n == 0) {
            ui->marketCapChange->setStyleSheet("color: rgb(255, 216, 0)");
            ui->marketCapChange->setText("- 0%");
        }
        else {
            ui->marketCapChange->setStyleSheet("color: rgb(208, 2, 27)");
            ui->marketCapChange->setText("▼ " + QString::number(n, 'f', 2) + "%");
        }

    }

    if(item == "volumn") {
        if(n >= 0) {
            ui->volumnChange->setStyleSheet("color: rgb(61, 174, 35)");
            ui->volumnChange->setText("▲ " + QString::number(n, 'f', 2) + "%");
        }else if(n == 0) {
            ui->volumnChange->setStyleSheet("color: rgb(255, 216, 0)");
            ui->volumnChange->setText("- 0%");
        }

        else {
            ui->volumnChange->setStyleSheet("color: rgb(208, 2, 27)");
            ui->volumnChange->setText("▼ " + QString::number(n, 'f', 2) + "%");
        }

    }
}

void MainWindow::requestHeader() {
    QUrl url("https://sandbox-api.coinmarketcap.com/v1/global-metrics/quotes/latest");
    request.setRawHeader("X-CMC_PRO_API_KEY", "a5089d27-78ec-4e30-8498-61007f62a309");
    request.setRawHeader("Accept", "application/json");
    request.setUrl(url);

    resetChoices();
    reqHeader = true;
    manager->get(request);
}

void MainWindow::updateHeader(QMap<QString, double> info) {

    ui->cryptoCountAmount->setText(QString::number(info["active_cryptocurrencies"]));
    ui->exchangeCountAmount->setText(QString::number(info["active_exchanges"]));
    ui->marketCap_2->setText("$" + doubleFormat(info["total_market_cap"]));
    percentChangeHeader("marketCap", info["total_market_cap_yesterday_percentage_change"]);
    ui->volumnAmount->setText("$" + doubleFormat(info["total_volume_24h"]));
    percentChangeHeader("volumn", info["total_volume_24h_yesterday_percentage_change"]);
}

void MainWindow::requestTrendings() {
    QUrl url("https://api.coingecko.com/api/v3/search/trending");
    request.setUrl(url);
    resetChoices();
    reqTrendings = true;
    manager->get(request);
}

void MainWindow::resetTrendingImg() {
    trendingImg1 = false;
    trendingImg2 = false;
    trendingImg3 = false;
    trendingPercent1 = false;
    trendingPercent2 = false;
    trendingPercent3 = false;
}

void MainWindow::managerImgFinished(QNetworkReply *reply) {


    QPixmap pixmap;
    double n;
    if(trendingImg1 or trendingImg2 or trendingImg3) {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Error in" << reply->url() << ":" << reply->errorString();
            return;
        }
        QByteArray img = reply->readAll();
        pixmap.loadFromData(img);
    }else {
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);
        if(jsonError.error != QJsonParseError::NoError) {
            qDebug() << "fromJson failed: " << jsonError.errorString();
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        n =  jsonObj["market_data"].toObject()["price_change_percentage_24h"].toDouble();
    }

    if(trendingImg1) {
        ui->trendingImage1->setPixmap(pixmap);
    }
    if(trendingImg2) {
        ui->trendingImage2->setPixmap(pixmap);
    }
    if(trendingImg3) {
        ui->trendingImage3->setPixmap(pixmap);
    }
    if(trendingPercent1) {
        if(n > 0) {
            ui->trendingPercent1->setStyleSheet("color: rgb(61, 174, 35)");
            ui->trendingPercent1->setText("▲ " + QString::number(n, 'f', 2) + "%");
        }else if(n == 0) {
            ui->trendingPercent1->setStyleSheet("color: rgb(255, 216, 0)");
            ui->trendingPercent1->setText("- 0%");
        }
        else {
            ui->trendingPercent1->setStyleSheet("color: rgb(208, 2, 27)");
            ui->trendingPercent1->setText("▼ " + QString::number(n, 'f', 2) + "%");
        }
    }
    if(trendingPercent2) {
        if(n > 0) {
            ui->trendingPercent2->setStyleSheet("color: rgb(61, 174, 35)");
            ui->trendingPercent2->setText("▲ " + QString::number(n, 'f', 2) + "%");
        }else if(n == 0) {
            ui->trendingPercent2->setStyleSheet("color: rgb(255, 216, 0)");
            ui->trendingPercent2->setText("- 0%");
        }
        else {
            ui->trendingPercent2->setStyleSheet("color: rgb(208, 2, 27)");
            ui->trendingPercent2->setText("▼ " + QString::number(n, 'f', 2) + "%");
        }
    }
    if(trendingPercent3) {
        if(n > 0) {
            ui->trendingPercent3->setStyleSheet("color: rgb(61, 174, 35)");
            ui->trendingPercent3->setText("▲ " + QString::number(n, 'f', 2) + "%");
        }else if(n == 0) {
            ui->trendingPercent3->setStyleSheet("color: rgb(255, 216, 0)");
            ui->trendingPercent3->setText("- 0%");
        }
        else {
            ui->trendingPercent3->setStyleSheet("color: rgb(208, 2, 27)");
            ui->trendingPercent3->setText("▼ " + QString::number(n, 'f', 2) + "%");
        }
    }
    resetTrendingImg();
}

void MainWindow::updateTrendings(QNetworkReply *reply) {
    connect(ui->trendingHolders, &ClickableWidgetTrending::clicked, this, &MainWindow::openCoinPage);
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);
    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return;
    }
    QJsonArray jsonArray =  jsonDoc.object()["coins"].toArray();



    ui->trendingName1->setText(jsonArray[0].toObject()["item"].toObject()["name"].toString());
    ui->trendingSymbol1->setText(jsonArray[0].toObject()["item"].toObject()["symbol"].toString());
    trendingImg1 = true;
    QUrl url1(jsonArray[0].toObject()["item"].toObject()["thumb"].toString());
    request.setUrl(url1);
    ImgManager->get(request);
    imgloop.exec();

    trendingPercent1 = true;
    QUrl urlP1("https://api.coingecko.com/api/v3/coins/" + jsonArray[0].toObject()["item"].toObject()["id"].toString() + "?localization=false&tickers=false&market_data=true&community_data=false&developer_data=false&sparkline=false");
    request.setUrl(urlP1);
    ImgManager->get(request);
    imgloop.exec();
    trendingUrl1  = "https://api.coingecko.com/api/v3/coins/" + jsonArray[0].toObject()["item"].toObject()["id"].toString() + "?localization=false&tickers=false&market_data=true&community_data=true&developer_data=true&sparkline=false";

    ui->trendingName2->setText(jsonArray[1].toObject()["item"].toObject()["name"].toString());
    ui->trendingSymbol2->setText(jsonArray[1].toObject()["item"].toObject()["symbol"].toString());
    trendingImg2 = true;
    QUrl url2(jsonArray[1].toObject()["item"].toObject()["thumb"].toString());
    request.setUrl(url2);
    ImgManager->get(request);
    imgloop.exec();

    trendingPercent2 = true;
    QUrl urlP2("https://api.coingecko.com/api/v3/coins/" + jsonArray[1].toObject()["item"].toObject()["id"].toString() + "?localization=false&tickers=false&market_data=true&community_data=false&developer_data=false&sparkline=false");
    request.setUrl(urlP2);
    ImgManager->get(request);
    imgloop.exec();
    trendingUrl2  = "https://api.coingecko.com/api/v3/coins/" + jsonArray[1].toObject()["item"].toObject()["id"].toString() + "?localization=false&tickers=false&market_data=true&community_data=true&developer_data=true&sparkline=false";


    ui->trendingName3->setText(jsonArray[2].toObject()["item"].toObject()["name"].toString());
    ui->trendingSymbol3->setText(jsonArray[2].toObject()["item"].toObject()["symbol"].toString());
    trendingImg3 = true;
    QUrl url3(jsonArray[2].toObject()["item"].toObject()["thumb"].toString());
    request.setUrl(url3);
    ImgManager->get(request);
    imgloop.exec();
    trendingPercent3 = true;
    QUrl urlP3("https://api.coingecko.com/api/v3/coins/" + jsonArray[2].toObject()["item"].toObject()["id"].toString() + "?localization=false&tickers=false&market_data=true&community_data=false&developer_data=false&sparkline=false");
    request.setUrl(urlP3);
    ImgManager->get(request);
    imgloop.exec();
    trendingUrl3  = "https://api.coingecko.com/api/v3/coins/" + jsonArray[2].toObject()["item"].toObject()["id"].toString() + "?localization=false&tickers=false&market_data=true&community_data=true&developer_data=true&sparkline=false";
}

void MainWindow::openCoinPage(QMouseEvent* event) {
    QRect widgetRect1 = ui->trendingHolders->geometry();
    int h = widgetRect1.height();
    int y = event->position().y();
    resetChoices();
    reqCoinPage = true;
    if(y <= 1.0 * h / 3) {
        request.setUrl(QUrl(trendingUrl1));
        manager->get(request);
        loop.exec();
    }
    if(y <= 1.0 * h / 3 * 2) {
        request.setUrl(QUrl(trendingUrl2));
        manager->get(request);
        loop.exec();
    }
    if(y <= h) {
        request.setUrl(QUrl(trendingUrl3));
        manager->get(request);
        loop.exec();
    }
}

void MainWindow::requestGainers() {
    QUrl url("https://sandbox-api.coinmarketcap.com/v1/cryptocurrency/trending/gainers-losers");

    QUrlQuery querry{url};
    querry.addQueryItem("sort_dir", "desc");

    request.setRawHeader("X-CMC_PRO_API_KEY", "a5089d27-78ec-4e30-8498-61007f62a309");
    request.setRawHeader("Accept", "application/json");
    request.setUrl(url);

    resetChoices();
    reqGainers = true;

    manager->get(request);
}

void MainWindow::updateGainers(QNetworkReply *reply) {
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);
    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject jsonData1 = jsonObj["data"].toObject();
    QJsonArray jsonData2 =  jsonData1["data"].toArray();

    QMap<double, QPair<QString, QString>> coins;
    QList<double> values;
    for(int i = 0; i < jsonData2.size(); i++) {
        QJsonObject usd =  jsonData2[i].toObject()["quote"].toObject()["USD"].toObject();
        values.append(usd["percent_change_24h"].toDouble());
        QString name = jsonData2[i].toObject()["name"].toString();
        QString sym = jsonData2[i].toObject()["symbol"].toString();
        coins[usd["percent_change_24h"].toDouble()] = QPair<QString, QString>(name, sym);
    }
    sort(values.begin(), values.end(), greater<>());

    ui->gainerName1->setText(coins[values[0]].first);
    ui->gainerSymbol1->setText(coins[values[0]].second);
    ui->trendingGainer1->setText("▲ " + QString::number(values[0], 'f', 2) + "%");
    ui->trendingGainer1->setStyleSheet("color: rgb(61, 174, 35)");

    ui->gainerName2->setText(coins[values[1]].first);
    ui->gainerSymbol2->setText(coins[values[1]].second);
    ui->trendingGainer2->setText("▲ " + QString::number(values[1], 'f', 2) + "%");
    ui->trendingGainer2->setStyleSheet("color: rgb(61, 174, 35)");

    ui->gainerName3->setText(coins[values[2]].first);
    ui->gainerSymbol3->setText(coins[values[2]].second);
    ui->trendingGainer3->setText("▲ " + QString::number(values[2], 'f', 2) + "%");
    ui->trendingGainer3->setStyleSheet("color: rgb(61, 174, 35)");
}

void MainWindow::requestLosers() {
    QUrl url("https://sandbox-api.coinmarketcap.com/v1/cryptocurrency/trending/gainers-losers");

    QUrlQuery querry{url};
    querry.addQueryItem("sort_dir", "asc");

    request.setRawHeader("X-CMC_PRO_API_KEY", "a5089d27-78ec-4e30-8498-61007f62a309");
    request.setRawHeader("Accept", "application/json");
    request.setUrl(url);

    resetChoices();
    reqLosers = true;

    manager->get(request);
}
void MainWindow::updateLosers(QNetworkReply *reply) {
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);
    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject jsonData1 = jsonObj["data"].toObject();
    QJsonArray jsonData2 =  jsonData1["data"].toArray();

    QMap<double, QPair<QString, QString>> coins;
    QList<double> values;
    for(int i = 0; i < jsonData2.size(); i++) {
        QJsonObject usd =  jsonData2[i].toObject()["quote"].toObject()["USD"].toObject();
        values.append(usd["percent_change_24h"].toDouble());
        QString name = jsonData2[i].toObject()["name"].toString();
        QString sym = jsonData2[i].toObject()["symbol"].toString();
        coins[usd["percent_change_24h"].toDouble()] = QPair<QString, QString>(name, sym);
    }
    sort(values.begin(), values.end(), greater<>());

    ui->loserName1->setText(coins[values[0]].first);
    ui->loserSymbol1->setText(coins[values[0]].second);
    ui->loserPercent1->setText("▼ " + QString::number(values[0], 'f', 2) + "%");
    ui->loserPercent1->setStyleSheet("color: rgb(208, 2, 27)");

    ui->loserName2->setText(coins[values[1]].first);
    ui->loserSymbol2->setText(coins[values[1]].second);
    ui->loserPercent2->setText("▼ " + QString::number(values[1], 'f', 2) + "%");
    ui->loserPercent2->setStyleSheet("color: rgb(208, 2, 27)");

    ui->loserName3->setText(coins[values[2]].first);
    ui->loserSymbol3->setText(coins[values[2]].second);
    ui->loserPercent3->setText("▼ " + QString::number(values[2], 'f', 2) + "%");
    ui->loserPercent3->setStyleSheet("color: rgb(208, 2, 27)");
}


void MainWindow::requestMain() {
    QUrl url("https://api.coingecko.com/api/v3/coins/markets?vs_currency=usd&order=market_cap_desc&per_page=30&page=1&sparkline=true&price_change_percentage=1h%2C24h%2C7d");

    request.setUrl(url);

    resetChoices();
    reqMain = true;
    manager->get(request);
}

void MainWindow::updateMain(QNetworkReply *reply) {
    //ui->tableWidget->clearContents();
    ui->tableWidget->horizontalHeader()->setFixedHeight(50);
    QJsonParseError jsonError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);
    QMap<double, QString> coins;
    QList<double> values;

    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return;
    }

    QJsonArray jsonArr = jsonDoc.array();
    //QDir dir("F:/School/C++/QT/build-CryptoExchange-Desktop_Qt_6_2_3_MinGW_64_bit-Debug/image", {"*.png"});
    QDir dir(":/image", {"*.png"});
    for(const QString & filename: dir.entryList()){
        qDebug() << ": " << filename;
        dir.remove(filename);
    }
    ui->tableWidget->setColumnWidth(0, 148);
    ui->tableWidget->setColumnWidth(1, 168);
    ui->tableWidget->setColumnWidth(2, 136);
    ui->tableWidget->setColumnWidth(3, 136);
    ui->tableWidget->setColumnWidth(4, 136);
    ui->tableWidget->setColumnWidth(5, 168);
    ui->tableWidget->setColumnWidth(6, 168);
    ui->tableWidget->setColumnWidth(7, 168);
    for(int i = 0; i < jsonArr.size(); i++) {
        ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
         ui->tableWidget->setRowHeight(ui->tableWidget->rowCount() - 1, 64);
        QJsonObject coin = jsonArr[i].toObject();
        drawMainRowChart(coin);
        drawMainRow(coin);
    }
    ui->helperChart->setVisible(false);
}

void MainWindow::drawMainRow(QJsonObject coin) {
    QTableWidgetItem *name = new QTableWidgetItem(tr(coin["name"].toString().toLocal8Bit()));
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, name);


    QLocale locale(QLocale::English);

    QTableWidgetItem *price = new QTableWidgetItem("$" + locale.toString(coin["current_price"].toDouble(), 'f', 3));
    price->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, price);

    double helper;
    helper = coin["price_change_percentage_1h_in_currency"].toDouble();
    QTableWidgetItem *oneH = new QTableWidgetItem(QString::number(helper, 'f', 2));
    if(helper < 0) {
        oneH->setForeground(QBrush(QColor(208, 2, 27)));
        oneH->setText("▼" + QString::number(helper, 'f', 2) + "%");
    }else if(helper == 0) {
        oneH->setForeground(QBrush(QColor(255, 216, 0)));
        oneH->setText("-0%");
    }else {
        oneH->setForeground(QBrush(QColor(61, 174, 35)));
        oneH->setText("▲" + QString::number(helper, 'f', 2) + "%");
    }
    oneH->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, oneH);

    helper = coin["price_change_percentage_24h_in_currency"].toDouble();
    QTableWidgetItem *dayH = new QTableWidgetItem(QString::number(helper, 'f', 2));
    if(helper < 0) {
        dayH->setForeground(QBrush(QColor(208, 2, 27)));
        dayH->setText("▼" + QString::number(helper, 'f', 2) + "%");
    }else if(helper == 0) {
        dayH->setForeground(QBrush(QColor(255, 216, 0)));
        dayH->setText("-0%");
    }else {
        dayH->setForeground(QBrush(QColor(61, 174, 35)));
        dayH->setText("▲" + QString::number(helper, 'f', 2) + "%");
    }
    dayH->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 3, dayH);

    helper = coin["price_change_percentage_7d_in_currency"].toDouble();
    QTableWidgetItem *weekH = new QTableWidgetItem(QString::number(helper, 'f', 2));
    if(helper < 0) {
        weekH->setForeground(QBrush(QColor(208, 2, 27)));
        weekH->setText("▼" + QString::number(helper, 'f', 2) + "%");
    }else if(helper == 0) {
        weekH->setForeground(QBrush(QColor(255, 216, 0)));
        weekH->setText("-0%");
    }else {
        weekH->setForeground(QBrush(QColor(61, 174, 35)));
        weekH->setText("▲" + QString::number(helper, 'f', 2) + "%");
    }
    weekH->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 4, weekH);

    helper = coin["market_cap"].toDouble();
    QTableWidgetItem *cap = new QTableWidgetItem("$"+locale.toString(helper, 'f', 3));
    cap->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 5, cap);

    helper = coin["total_volume"].toDouble();
    QTableWidgetItem *vol = new QTableWidgetItem("$"+locale.toString(helper, 'f', 3));
    vol->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 6, vol);

    QPixmap pixmap("image/" + coin["id"].toString() + ".png");
    QIcon icon = *new QIcon(pixmap);

    QTableWidgetItem *chart = new QTableWidgetItem();
    QBrush brush = *new QBrush(pixmap);
    chart->setBackground(brush);

    chart->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 7, chart);
}

void MainWindow::drawMainRowChart(QJsonObject coin) {
    QJsonArray arr = coin["sparkline_in_7d"].toObject()["price"].toArray();
    QSplineSeries *series = new QSplineSeries();

    for(int i = 0; i < arr.size(); i++) {
        series->append(i, arr[i].toDouble());
    }

    QChart *chart = new QChart();
    chart->legend()->hide();


    //chart line
    QPen pen;
    if(series->at(0).y() <= series->at(series->count()-1).y()) {
        pen.setColor(QColor(61, 174, 35));
    }else {
        pen.setColor(QColor(208, 2, 27));
    }
    pen.setWidth(2);
    series->setPen(pen);

    //2 Axis
    QCategoryAxis *axisX = new QCategoryAxis();
    QCategoryAxis *axisY = new QCategoryAxis();

    axisY->setGridLineVisible(false);
    axisY->setLineVisible(false);
    axisY->setVisible(true);
    axisX->setVisible(true);

    //Set chart
    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignRight);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chart->setBackgroundVisible(false);
    chart->setPlotArea(QRectF(0,0,168, 50));
    ui->helperChart->setChart(chart);
    ui->helperChart->setRenderHint(QPainter::Antialiasing);

    QPixmap pixMap = ui->helperChart->grab(ui->helperChart->sceneRect().toRect());
    pixMap.save("image/" + coin["id"].toString() +  ".png");

}


QSplineSeries * MainWindow::returnSerie(QNetworkReply *reply) {
    QJsonParseError jsonError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);

    QSplineSeries *series = new QSplineSeries();

    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return series;
    }



    if(jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();
        QJsonArray jsonPrice = jsonObj["prices"].toArray();

        QList<double> values = {};


        for(int i = 0; i < jsonPrice.size(); i++) {
            series->append(jsonPrice[i].toArray()[0].toDouble(),jsonPrice[i].toArray()[1].toDouble());
        }
    }
    return series;
}

void MainWindow::drawChartLine(QSplineSeries* series) {

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Simple line chart example");

    //chart line
    QPen pen;
    if(series->at(0).y() <= series->at(series->count()-1).y()) {
        pen.setColor(QColor(61, 174, 35));
    }else {
        pen.setColor(QColor(208, 2, 27));
    }
    pen.setWidth(2);
    series->setPen(pen);

    //2 Axis
    QCategoryAxis *axisX = new QCategoryAxis();
    QCategoryAxis *axisY = new QCategoryAxis();

    QPen axisPen(QColor(237,237,237));
    axisPen.setWidth(2);
    axisX->setLinePen(axisPen);
    axisY->setLinePen(axisPen);

    QFont labelsFont;
    labelsFont.setPixelSize(12);
    labelsFont.setWeight(QFont::Bold);
    axisX->setLabelsFont(labelsFont);
    axisY->setLabelsFont(labelsFont);
    QBrush labelBrush(QColor(119,121,123));
    axisX->setLabelsBrush(labelBrush);
    axisY->setLabelsBrush(labelBrush);


    double minX = series->at(0).x();
    double maxX = series->at(0).x();
    double minY = series->at(0).y();
    double maxY = series->at(0).y();
    for(int i = 1; i < series->count(); i++) {
        if(series->at(i).y() > maxY) {
            maxY = series->at(i).y();
        }
        if(series->at(i).y() < minY) {
            minY = series->at(i).y();
        }
        if(series->at(i).x() > maxX) {
            maxX = series->at(i).x();
        }
        if(series->at(i).x() < minX) {
            minX = series->at(i).x();
        }
    }

    //Axis X range
    double gap = (maxX - minX) / 6;
    for(double i = minX + gap; i < maxX; i+=gap) {
        QDateTime timeStamp;
        timeStamp.setMSecsSinceEpoch(i);
        QString helper="";
        if(timeSpan != "1d") {
            QDate dateStamp = timeStamp.date();
            helper= ( QString::number(dateStamp.day()) + "/" + QString::number(dateStamp.month()) );
        }else {
            QTime timeHelper = timeStamp.time();
            helper = ( QString::number(timeHelper.hour()) + ":" + QString::number(timeHelper.minute()) );
        }
        axisX->append(helper, i);
    }
    axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    //Axis Y range
    double gapY = (maxY - minY) / 4;
    axisY->append("", minY - gapY / 2);
    axisY->append(QString::number(minY), minY);
    axisY->append(QString::number(minY + gapY), (minY + gapY));
    axisY->append(QString::number(minY + gapY * 2), (minY + gapY * 2));
    axisY->append(QString::number(minY + gapY * 3), (minY + gapY * 3));
    axisY->append(QString::number(maxY), maxY);
    axisY->append("", maxY + gapY / 2);

    axisY->setRange(minY - gapY / 2, maxY + gapY / 2);
    axisY->setGridLineVisible(false);
    axisY->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    //Set chart
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignRight);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::on_pushButton_clicked() {}

int MainWindow::timeSpanDateCount(QString timeSpan) {
    QDateTime now = QDateTime::currentDateTimeUtc();
    int monthToAdd = 0;
    if(timeSpan == "1d" or timeSpan == "1w") {
        return -1;
    }else if(timeSpan=="ytd") {
        int currentYear = now.date().year();
        QDate ytd{currentYear, 1, 1};
        return -1 * now.daysTo(ytd.startOfDay());

    }else if(timeSpan=="12m") {
        monthToAdd = 12;
    }
    else {
        monthToAdd = timeSpan.at(0).digitValue();
    }
    QDateTime span = now.addMonths(-1 * monthToAdd);
    return span.daysTo(now);
}

QString MainWindow::getLink(QString id, QString vs_currency, QString days) {
    QString url = "https://api.coingecko.com/api/v3/coins/" + id + "/market_chart?vs_currency=" + vs_currency + "&days=" + days;
    //https://api.coingecko.com/api/v3/coins/bitcoin/market_chart?vs_currency=usd&days=1
    return url;
}


void MainWindow::on_changeTimeToday_clicked()
{

    if(timeSpan == "1d") {
        return;
    }
    timeSpan = "1d";
    request.setUrl(QUrl("https://api.coingecko.com/api/v3/coins/bitcoin/market_chart?vs_currency=usd&days=1"));

    manager->get(request);
}


void MainWindow::on_changeTime1W_clicked()
{
    if(timeSpan == "1w") {
        return;
    }
    timeSpan = "1w";
    request.setUrl(QUrl("https://api.coingecko.com/api/v3/coins/bitcoin/market_chart?vs_currency=usd&days=7"));
    manager->get(request);
}




void MainWindow::on_changeTime1M_clicked()
{
    if(timeSpan == "1m") {
        return;
    }
    timeSpan = "1m";
    int days = timeSpanDateCount(timeSpan);
    QString url = getLink("bitcoin", "usd", QString::number(days));
    request.setUrl(QUrl(url));
    manager->get(request);
}


void MainWindow::on_changeTime3M_clicked()
{
    if(timeSpan == "3m") {
        return;
    }
    timeSpan = "3m";
    int days = timeSpanDateCount(timeSpan);
    QString url = getLink("bitcoin", "usd", QString::number(days));
    request.setUrl(QUrl(url));
    manager->get(request);
}


void MainWindow::on_changeTime6M_clicked()
{
    if(timeSpan == "6m") {
        return;
    }
    timeSpan = "6m";
    int days = timeSpanDateCount(timeSpan);
    QString url = getLink("bitcoin", "usd", QString::number(days));
    request.setUrl(QUrl(url));
    manager->get(request);
}


void MainWindow::on_changeTime12M_clicked()
{
    if(timeSpan == "12m") {
        return;
    }
    timeSpan = "12m";
    int days = timeSpanDateCount(timeSpan);
    QString url = getLink("bitcoin", "usd", QString::number(days));
    request.setUrl(QUrl(url));
    manager->get(request);
}


void MainWindow::on_changeTimeYTD_clicked()
{
    if(timeSpan == "ytd") {
        return;
    }
    timeSpan = "ytd";
    int days = timeSpanDateCount(timeSpan);
    QString url = getLink("bitcoin", "usd", QString::number(days));
    request.setUrl(QUrl(url));
    manager->get(request);
}

