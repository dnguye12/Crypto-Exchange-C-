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

    void constructor(QNetworkReply *reply);

private:
    Ui::CoinPage *ui;
};

#endif // COINPAGE_H
