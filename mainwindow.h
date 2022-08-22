#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QEventLoop>

#include "coinmarketapi.h"

#include <QDateTime>

#include <QSplineSeries>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void managerFinished(QNetworkReply *reply);

    void managerImgFinished(QNetworkReply *reply);

    void on_pushButton_clicked();

    void on_changeTimeToday_clicked();

    void on_changeTime1W_clicked();

    void on_changeTime1M_clicked();

    void on_changeTime3M_clicked();

    void on_changeTime6M_clicked();

    void on_changeTime12M_clicked();

    void on_changeTimeYTD_clicked();

private:
    //attributes
    Ui::MainWindow *ui;

    QNetworkAccessManager *manager;
    QNetworkAccessManager *ImgManager;
    QNetworkRequest request;
    QEventLoop loop;
    QEventLoop imgloop;
    QEventLoop coinLoop;

    QString timeSpan;

    CoinMarketApi* coinMarketApi;
    QString apiKey = "b54bcf4d-1bca-4e8e-9a24-22ff2c3d462c";

    //functions
    void centerScreen();

    QSplineSeries * returnSerie(QNetworkReply *reply);

    void resetChoices();

    QString doubleFormat(double n);

    //request choices
    bool reqHeader = false;
    bool reqTrendings = false;
    bool reqGainers = false;
    bool reqLosers = false;
    bool reqMain = false;
    bool reqCoin = false;

    //headers
    void requestHeader();
    void updateHeader(QMap<QString, double> info);
    void percentChangeHeader(QString item, double n);

    //trending
    bool trendingImg1 = false;
    bool trendingImg2 = false;
    bool trendingImg3 = false;
    bool trendingPercent1 = false;
    bool trendingPercent2 = false;
    bool trendingPercent3 = false;
    void resetTrendingImg();

    void requestTrendings();
    void updateTrendings(QNetworkReply *reply);
    void requestGainers();
    void updateGainers(QNetworkReply *reply);
    void requestLosers();
    void updateLosers(QNetworkReply *reply);

    //main table
    void requestMain();
    void updateMain(QNetworkReply *reply);
    void drawMainRow(QJsonObject coin);
    void drawMainRowChart(QJsonObject coin);

    //coin manager
    void openCoinPage(QMouseEvent* event);


    void drawChartLine(QSplineSeries* series);

    QString getLink(QString id, QString vs_currency, QString days);

    int timeSpanDateCount(QString timeSpan);



};
#endif // MAINWINDOW_H
