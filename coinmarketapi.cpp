#include "coinmarketapi.h"

#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>


CoinMarketApi::CoinMarketApi() {

}

QMap<QString, double> CoinMarketApi::getTotalCap(QNetworkReply *reply) {
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);

    QMap<QString, double> res;
    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return res;
    }

    if(jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();

        QJsonObject jsonData = jsonObj["data"].toObject();
        res["active_cryptocurrencies"] = jsonData["active_cryptocurrencies"].toDouble();
        res["active_exchanges"] = jsonData["active_exchanges"].toDouble();

        QJsonObject jsonPrice1 = jsonData["quote"].toObject()["USD"].toObject();
        res["total_market_cap"] = jsonPrice1["total_market_cap"].toDouble();
        res["total_market_cap_yesterday_percentage_change"] = jsonPrice1["total_market_cap_yesterday_percentage_change"].toDouble();
        res["total_volume_24h"] = jsonPrice1["total_volume_24h"].toDouble();
        res["total_volume_24h_yesterday_percentage_change"] = jsonPrice1["total_volume_24h_yesterday_percentage_change"].toDouble();
    }
    return res;
}

QList<Coin> getGainers(QNetworkReply *reply) {

}
