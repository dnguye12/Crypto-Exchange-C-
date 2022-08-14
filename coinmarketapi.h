#ifndef COINMARKETAPI_H
#define COINMARKETAPI_H

#include "coin.h"

#include <QNetworkReply>
#include <QSplineSeries>



class CoinMarketApi
{
public:
    CoinMarketApi();

    //QSplineSeries * returnSplineSerie(QNetworkReply *reply);

    QMap<QString, double> getTotalCap(QNetworkReply *reply);

    QList<Coin> getGainers(QNetworkReply *reply);
private:


};

#endif // COINMARKETAPI_H
