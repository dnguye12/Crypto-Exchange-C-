#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QtCore>
#include <QtGui>

#include <QNetworkReply>
#include <QChart>
#include <QLineSeries>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    //QString answer = reply->readAll();
    //qDebug().noquote() << answer;
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);

    QChart *chart = new QChart();
    QLineSeries *series = new QLineSeries();

    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return ;
    }

    if(jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();
        QJsonObject jsonChild1 = jsonObj[jsonObj.keys().at(1)].toObject();

        QList<double> values = {};
        for(int i = 0; i < jsonChild1.keys().size(); i++) {
            QJsonObject jsonChild2 =  jsonChild1[jsonChild1.keys().at(i)].toObject();
            qDebug() << i << ": "<< jsonChild2["4. close"].toString();
            values.append(jsonChild2["4. close"].toString().toDouble());
        }

        for(int k = 0; k < (values.size()/2); k++)  values.swapItemsAt(k,values.size()-(1+k));

        for(int i = 0; i < values.size(); i++) {
            series->append(i, values[i]);
        }
    }

    chart->legend()->hide();
    chart->addSeries(series);
        chart->createDefaultAxes();
        chart->setTitle("Simple line chart example");

        ui->chartView->setChart(chart);
          ui->chartView->setRenderHint(QPainter::Antialiasing);




}


void MainWindow::on_pushButton_clicked()
{
    request.setUrl(QUrl("https://www.alphavantage.co/query?function=CRYPTO_INTRADAY&symbol=BTC&market=USD&interval=15min&outputsize=full&apikey=UFJXTOHHD9UJ43XH&datatype=json"));
    manager->get(request);

}

