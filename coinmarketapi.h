#ifndef COINMARKETAPI_H
#define COINMARKETAPI_H

#include <QNetworkReply>
#include <QSplineSeries>

class CoinMarketApi
{
public:
    CoinMarketApi();

    //QSplineSeries * returnSplineSerie(QNetworkReply *reply);

    QMap<QString, double> getTotalCap(QNetworkReply *reply);
private:


};

#endif // COINMARKETAPI_H
