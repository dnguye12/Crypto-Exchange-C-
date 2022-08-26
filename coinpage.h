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


namespace Ui {
class CoinPage;
}

class CoinPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit CoinPage(QWidget *parent = nullptr);
    ~CoinPage();

    void constructor(QNetworkReply *reply);

private:
    Ui::CoinPage *ui;

    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QEventLoop loop;

    QJsonObject helperObj;

    bool reqIcon = false;
    bool reqChart = false;

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

private slots:
    void managerFinished(QNetworkReply* reply);

    void activateComboBox(int index);
    void on_changeTimeToday_2_clicked();
    void on_changeTimeYTD_2_clicked();
    void on_changeTime1W_2_clicked();
    void on_changeTime1M_2_clicked();
    void on_changeTime3M_2_clicked();
    void on_changeTime6M_2_clicked();
    void on_changeTime12M_2_clicked();
};

#endif // COINPAGE_H
