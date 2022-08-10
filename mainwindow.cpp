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
        qDebug() << reply->errorString();
        return;
    }

    QLineSeries* series = returnSerie(reply);
    drawChartLine(series);


}

QLineSeries* MainWindow::returnSerie(QNetworkReply *reply) {
    QJsonParseError jsonError;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);

    QLineSeries *series = new QLineSeries();

    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return series;
    }



    if(jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();
        QJsonArray jsonPrice = jsonObj["prices"].toArray();

        QList<double> values = {};


        for(int i = 0; i < jsonPrice.size(); i++) {
            values.append(jsonPrice[i].toArray()[1].toDouble());
        }

        for(int i = 0; i < values.size(); i++) {
            series->append(i, values[i]);
        }
    }
    return series;
}

void MainWindow::drawChartLine(QLineSeries* series) {
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Simple line chart example");

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

