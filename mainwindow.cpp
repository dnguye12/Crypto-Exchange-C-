#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "coin.h"

#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QEventLoop>

#include <QNetworkReply>

#include <QChart>
#include <QLineSeries>

#include <QSizePolicy>
#include <QScreen>

#include <QCategoryAxis>

#include <QTime>

#include <algorithm>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    centerScreen();

    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerFinished(QNetworkReply*)));

    QEventLoop loop;
    //connect(myObject, SIGNAL(theSignalToWaitFor()), &loop, SLOT(quit()));
    //connect(timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));
    //loop.exec(); //blocks untill either theSignalToWaitFor or timeout was fired
    connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    requestTrendings();
    loop.exec();
    requestHeader();
    loop.exec();
    requestGainers();
    loop.exec();
    requestLosers();
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
}

void MainWindow::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error: " << reply->error() <<
                    ", Message: " << reply->errorString() <<
                    ", Code: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    }
    //QSplineSeries* series = returnSerie(reply);
    //drawChartLine(series);
    //qDebug() << reply->readAll();
    if(reqTrendings) {
        updateTrendings(reply);
        resetChoices();
    }
    if(reqHeader) {
        QMap<QString, double> totalMarket = coinMarketApi->getTotalCap(reply);
        updateHeader(totalMarket);
        resetChoices();
    }

    if(reqGainers) {
        updateGainers(reply);
        resetChoices();
    }

    if(reqLosers) {
        updateLosers(reply);
        resetChoices();
    }
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
    QUrl url("https://sandbox-api.coinmarketcap.com/v1/cryptocurrency/trending/latest");

    QUrlQuery querry{url};
    querry.addQueryItem("limit", "3");

    request.setRawHeader("X-CMC_PRO_API_KEY", "a5089d27-78ec-4e30-8498-61007f62a309");
    request.setRawHeader("Accept", "application/json");
    request.setUrl(url);

    resetChoices();
    reqTrendings = true;

    manager->get(request);
}
void MainWindow::updateTrendings(QNetworkReply *reply) {
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);
    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject jsonData1 = jsonObj["data"].toObject();
    QJsonArray jsonData2 =  jsonData1["data"].toArray();
    double n;

    ui->trendingName1->setText(jsonData2[0].toObject()["name"].toString());
    ui->trendingSymbol1->setText(jsonData2[0].toObject()["symbol"].toString());
    n = jsonData2[0].toObject()["quote"].toObject()["USD"].toObject()["percent_change_24h"].toDouble();
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

    ui->trendingName2->setText(jsonData2[1].toObject()["name"].toString());
    ui->trendingSymbol2->setText(jsonData2[1].toObject()["symbol"].toString());
    ui->trendingName1->setText(jsonData2[0].toObject()["name"].toString());
    ui->trendingSymbol1->setText(jsonData2[0].toObject()["symbol"].toString());
    n = jsonData2[1].toObject()["quote"].toObject()["USD"].toObject()["percent_change_24h"].toDouble();
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

    ui->trendingName3->setText(jsonData2[2].toObject()["name"].toString());
    ui->trendingSymbol3->setText(jsonData2[2].toObject()["symbol"].toString());
    ui->trendingName1->setText(jsonData2[0].toObject()["name"].toString());
    ui->trendingSymbol1->setText(jsonData2[0].toObject()["symbol"].toString());
    n = jsonData2[2].toObject()["quote"].toObject()["USD"].toObject()["percent_change_24h"].toDouble();
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
    sort(values.begin(), values.end());

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

    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);

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

