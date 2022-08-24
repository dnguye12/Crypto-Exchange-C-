#ifndef COINPAGE_H
#define COINPAGE_H

#include <QMainWindow>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>

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

private slots:
    void managerFinished(QNetworkReply* reply);
};

#endif // COINPAGE_H
