#ifndef COINPAGE_H
#define COINPAGE_H

#include <QMainWindow>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>

#include <QComboBox>

#include <QJsonObject>

#include <QSplineSeries>
#include "callout.h"
#include "mainwindow.h"

namespace Ui {
class CoinPage;
}

class CoinPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit CoinPage(QWidget *parent = nullptr);
    ~CoinPage();

    void constructor(QNetworkReply *reply, MainWindow* parent);

private:
    Ui::CoinPage *ui;

    MainWindow* mw;

    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QEventLoop loop;
    QEventLoop loopNews;
    QEventLoop loopNewsImg;

    QJsonObject helperObj;

    bool reqIcon = false;
    bool reqChart = false;
    bool searchNews = false;
    bool reqNews = false;
    bool reqNewsImage = false;

    //functions
    void resetReq();
    void section1(QJsonObject jsonObj);

    void section2(QJsonObject jsonObj);

    void section2Links(QJsonObject jsonObj);
    QString linkShort(QString link);
    void setUpComboBox(QJsonObject jsonObj, QString section, QComboBox* cb);

    //charts
    QString coinId;
    QString timeSpan;
    int timeSpanDateCount(QString timeSpan);
    QSplineSeries* returnSerie(QNetworkReply *reply);
    void drawChartLine(QSplineSeries* series);
    Callout *m_tooltip;

    //Coin Description
    void CoinDescSection(QJsonObject jsonObj);

    //News
    bool img1 = false;
    bool img2 = false;
    bool img3 = false;
    void searchNewsFunction(QJsonObject jsonObj);
    void requestNews(QNetworkReply* reply);
    void updateNews(QNetworkReply* reply);
    void updateNewsImage(QNetworkReply* reply);

private slots:
    void chartCallOut(const QPointF &point, bool state);
    void managerFinished(QNetworkReply* reply);

    void activateComboBox(int index);
    void on_changeTimeToday_2_clicked();
    void on_changeTimeYTD_2_clicked();
    void on_changeTime1W_2_clicked();
    void on_changeTime1M_2_clicked();
    void on_changeTime3M_2_clicked();
    void on_changeTime6M_2_clicked();
    void on_changeTime12M_2_clicked();
    void on_pushButton_clicked();
};

#endif // COINPAGE_H
