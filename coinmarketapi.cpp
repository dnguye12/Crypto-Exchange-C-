#include "coinmarketapi.h"

#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>


CoinMarketApi::CoinMarketApi() {

}

QSplineSeries * CoinMarketApi::returnSplineSerie(QNetworkReply *reply) {
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);

    QSplineSeries *series = new QSplineSeries();

    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return series;
    }

    if(jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();

        QJsonObject jsonData = jsonObj["data"].toObject();

        QJsonArray jsonPrice = jsonData["quotes"].toArray();

        for(int i = 0; i < jsonPrice.size(); i++) {
            QJsonObject jsonQuote = jsonPrice[i].toObject();
            QString timeHelper = jsonQuote["timestamp"].toString();
            QDateTime dateHeler = QDateTime::fromString(timeHelper, Qt::ISODate);
            qDebug() << dateHeler.toString();
        }
        /*
        QList<double> values = {};


        for(int i = 0; i < jsonPrice.size(); i++) {
            series->append(jsonPrice[i].toArray()[0].toDouble(),jsonPrice[i].toArray()[1].toDouble());
        }*/
    }
    return series;
}
