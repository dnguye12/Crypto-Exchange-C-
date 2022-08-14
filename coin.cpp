#include "coin.h"

Coin::Coin(QString id,QString symbol,QString name,QString desc,QStringList iconLinks, double price,double marketCap,int marketCapRank,QList<double> sparkLine) {
    this->id = id;
    this->symbol = symbol;
    this->name = name;
    this->desc = desc;
    this->iconLinks = iconLinks;
    this->price = price;
    this->marketCap = marketCap;
    this->marketCapRank = marketCapRank;
    this->sparkLine = sparkLine;
}

QString Coin::getCoinUrl(QString id, bool local, bool ticker, bool market_data, bool com_data, bool dev_data, bool sparkline)
{
    QString url = "https://api.coingecko.com/api/v3/coins/";
    url += (id+"?");
    url += ("localization=" + QString(local ? "true" : "false") + "&");
    url += ("tickers=" + QString(ticker ? "true" : "false") + "&");
    url += ("market_data=" + QString(market_data ? "true" : "false") + "&");
    url += ("community_data=" + QString(com_data ? "true" : "false") + "&");
    url += ("developer_data=" + QString(dev_data ? "true" : "false") + "&");
    url += ("sparkline=" + QString(sparkline ? "true" : "false"));

    return url;
}
