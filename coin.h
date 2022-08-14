#ifndef COIN_H
#define COIN_H

#include <QString>
#include <QList>

#include <QObject>

class Coin : public QObject
{
    Q_OBJECT

public:
    Coin(QString id,QString symbol,QString name,QString desc,QStringList iconLinks, double price,double marketCap,int marketCapRank,QList<double> sparkLine);

    //functions
    QString getCoinUrl(QString id, bool local = false, bool ticker = false, bool market_data = true, bool com_data = true, bool dev_data = false, bool sparkline = true);
private:
    QString id;
    QString symbol;
    QString name;
    QString desc;
    QStringList iconLinks;

    double price;
    double marketCap;
    int marketCapRank;

    QList<double> sparkLine;
};

#endif // COIN_H
