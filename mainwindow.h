#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QNetworkAccessManager>
#include <QNetworkRequest>

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
    QNetworkRequest request;

    QString timeSpan;

    CoinMarketApi* coinMarketApi;
    QString apiKey = "b54bcf4d-1bca-4e8e-9a24-22ff2c3d462c";

    //functions
    void centerScreen();

    QSplineSeries * returnSerie(QNetworkReply *reply);

    void drawChartLine(QSplineSeries* series);

    QString getLink(QString id, QString vs_currency, QString days);

    int timeSpanDateCount(QString timeSpan);

};
#endif // MAINWINDOW_H
