#ifndef COINPAGE_H
#define COINPAGE_H

#include <QMainWindow>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
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

    QNetworkRequest request;

    void constructor(QString coinUrl, QNetworkAccessManager *manager);

private:
    Ui::CoinPage *ui;
};

#endif // COINPAGE_H
