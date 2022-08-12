#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QtCore>
#include <QtGui>

#include <QNetworkReply>
#include <QChart>
#include <QLineSeries>

#include <QSizePolicy>
#include <QScreen>

#include <QCategoryAxis>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    centerScreen();

    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerFinished(QNetworkReply*)));
    on_changeTimeToday_clicked();
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

void MainWindow::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Error: " << reply->error() <<
                               ", Message: " << reply->errorString() <<
                               ", Code: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    }

    QSplineSeries* series = returnSerie(reply);
    //drawChartLine(series);


}

QSplineSeries * MainWindow::returnSerie(QNetworkReply *reply) {
    QJsonParseError jsonError;
    qDebug().noquote() << reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);

    QSplineSeries *series = new QSplineSeries();

    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return series;
    }

    if(jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();
        qDebug() << jsonObj.keys().join(", ");
        /*
        QJsonArray jsonPrice = jsonObj["prices"].toArray();

        QList<double> values = {};


        for(int i = 0; i < jsonPrice.size(); i++) {
            series->append(jsonPrice[i].toArray()[0].toDouble(),jsonPrice[i].toArray()[1].toDouble());
        }*/
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
    qDebug() << labelsFont.family();
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
{/*
    timeSpan = "1d";
    request.setUrl(QUrl("https://api.coingecko.com/api/v3/coins/bitcoin/market_chart?vs_currency=usd&days=1"));
    manager->get(request);*/


    //curl -H "X-CMC_PRO_API_KEY: b54bcf4d-1bca-4e8e-9a24-22ff2c3d462c" -H "Accept: application/json" -d "start=1&limit=5000&convert=USD" -G https://sandbox-api.coinmarketcap.com/v1/cryptocurrency/listings/latest
    //QUrl url("https://sandbox-api.coinmarketcap.com/v1/cryptocurrency/listings/latest");
    //QUrlQuery querry(url);
    //querry.addQueryItem("")

    //request.setRawHeader("X-CMC_PRO_API_KEY", "b54bcf4d-1bca-4e8e-9a24-22ff2c3d462c");
    //request.setRawHeader("Accept", "application/json");
    QUrl url("https://sandbox-api.coinmarketcap.com/v1/global-metrics/quotes/historical");


QUrlQuery querry{url};
querry.addQueryItem("time_end", QString::number(QDateTime::currentMSecsSinceEpoch()));
querry.addQueryItem("count", "1");
querry.addQueryItem("interval", "daily");
querry.addQueryItem("aux", "total_volume_24h");

request.setUrl(url);


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

