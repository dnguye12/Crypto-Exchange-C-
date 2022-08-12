#include "coinmarketapi.h"

#include <QUrlQuery>
#include <QPair>

QNetworkRequest CoinMarketApi::returnRequest(QString functions, QList<QPair<QString, QString>> queries) {
    QNetworkRequest request = *new QNetworkRequest();

    QString url = "https://sandbox-api.coinmarketcap.com" + functions;
    request.setUrl(url);
    QUrlQuery query{url};
    for(int i = 0; i < queries.size(); i++) {
        QPair<QString, QString> pair = queries.at(i);
        query.addQueryItem(pair.first, pair.second);
    }

    request.setRawHeader("Accepts", "application/json");
    request.setRawHeader("X-CMC_PRO_API_KEY", apiKey.toUtf8());
    return request;
}
