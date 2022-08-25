#ifndef COINPAGE_H
#define COINPAGE_H

#include <QMainWindow>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>

#include <QComboBox>


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

    //functions
    void section1(QJsonObject jsonObj);

    void section2(QJsonObject jsonObj);

    void section2Links(QJsonObject jsonObj);
    QString linkShort(QString link);
    void setUpComboBox(QJsonObject jsonObj, QString section, QComboBox* cb);

private slots:
    void managerFinished(QNetworkReply* reply);

    void TestFunc(QMouseEvent *event);
};

#endif // COINPAGE_H
