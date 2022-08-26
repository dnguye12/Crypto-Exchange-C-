#include "coinpage.h"
#include "ui_coinpage.h"

#include <QJsonParseError>
#include <QJsonArray>

#include <QListView>
#include <QDesktopServices>

CoinPage::CoinPage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CoinPage)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(managerFinished(QNetworkReply*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

}

CoinPage::~CoinPage()
{
    delete ui;
}

void CoinPage::TestFunc(QMouseEvent *event) {
}

void CoinPage::constructor(QNetworkReply *reply) {
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll(), &jsonError);
    if(jsonError.error != QJsonParseError::NoError) {
        qDebug() << "fromJson failed: " << jsonError.errorString();
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    section1(jsonObj);
    section2(jsonObj);
    section2Links(jsonObj);
}

void CoinPage::section1(QJsonObject jsonObj) {
    ui->CoinName->setText(jsonObj["name"].toString());
    ui->CoinSymbol->setText(jsonObj["symbol"].toString().toUpper());
    ui->CoinRank->setText("Rank #" + QString::number(jsonObj["market_cap_rank"].toInteger()));
    if(jsonObj["categories"].toArray().size() == 0) {
        ui->CoinType->setVisible(false);
    }else {
        ui->CoinType->setText(jsonObj["categories"].toArray()[0].toString());
    }

    ui->CoinPriceSmall->setText(ui->CoinName->text() + " Price (" + ui->CoinSymbol->text() + ")");
    double price = jsonObj["market_data"].toObject()["current_price"].toObject()["usd"].toDouble();
    if(price < 1) {
        ui->CoinPrice->setText("$" + QString::number(price, 'f', 6));
    }else {
        ui->CoinPrice->setText("$" + QString::number(price, 'f', 2));
    }

    if(jsonObj["market_data"].toObject()["price_change_percentage_24h"].toDouble() >= 0) {
        ui->CoinChange->setStyleSheet("color: white;\nbackground-color: rgb(61, 174, 35);\nborder-radius: 5px;\npadding: 10px 15px;\nmargin-left: 15px;");
        ui->CoinChange->setText("▲" + QString::number(jsonObj["market_data"].toObject()["price_change_percentage_24h"].toDouble(), 'f', 2) + "%");
    }else {
        ui->CoinChange->setStyleSheet("color: white;\nbackground-color: rgb(208, 2, 27);\nborder-radius: 5px;\npadding: 10px 15px;\nmargin-left: 15px;");
        ui->CoinChange->setText("▼" + QString::number(jsonObj["market_data"].toObject()["price_change_percentage_24h"].toDouble(), 'f', 2) + "%");
    }
    ui->CoinLow->setText("Low(24h):$" + QString::number(jsonObj["market_data"].toObject()["low_24h"].toObject()["usd"].toDouble(), 'f', 2));
    ui->CoinHigh->setText("High(24h):$" + QString::number(jsonObj["market_data"].toObject()["high_24h"].toObject()["usd"].toDouble(), 'f', 2));

    request.setUrl(QUrl(jsonObj["image"].toObject()["large"].toString()));
    manager->get(request);
    loop.exec();
}

void CoinPage::section2(QJsonObject jsonObj) {
    QLocale locale(QLocale::English);
    ui->CoinMarketCap->setText("$" + locale.toString(jsonObj["market_data"].toObject()["market_cap"].toObject()["usd"].toDouble(), 'f', 0));

    if(jsonObj["market_data"].toObject()["market_cap_change_percentage_24h"].toDouble() >= 0) {
        ui->CoinMarketCapChange->setStyleSheet("color: rgb(61, 174, 35);");
        ui->CoinMarketCapChange->setText("▲ " + QString::number(jsonObj["market_data"].toObject()["market_cap_change_percentage_24h"].toDouble(), 'f', 2) + "%");
    }else {
        ui->CoinMarketCapChange->setStyleSheet("color: rgb(208, 2, 27);");
        ui->CoinMarketCapChange->setText("▼ " + QString::number(jsonObj["market_data"].toObject()["market_cap_change_percentage_24h"].toDouble(), 'f', 2) + "%");
    }

    ui->CoinFullyDiluted->setText("$" + locale.toString(jsonObj["market_data"].toObject()["fully_diluted_valuation"].toObject()["usd"].toDouble(), 'f', 0));

    ui->CoinVolume->setText("$" + locale.toString(jsonObj["market_data"].toObject()["total_volume"].toObject()["usd"].toDouble(), 'f', 0));
    ui->CoinVolumeDivided->setText(locale.toString(jsonObj["market_data"].toObject()["total_volume"].toObject()["usd"].toDouble() / jsonObj["market_data"].toObject()["market_cap"].toObject()["usd"].toDouble(), 'f', 5));

    ui->CoinSupply->setText(locale.toString(jsonObj["market_data"].toObject()["circulating_supply"].toDouble(), 'f', 0) + " " + jsonObj["symbol"].toString().toUpper());
    ui->CoinSupplyTotal->setText(locale.toString(jsonObj["market_data"].toObject()["total_supply"].toDouble(), 'f', 0));
    if(jsonObj["market_data"].toObject()["max_supply"].isNull()) {
        ui->CoinSupplyMax->setText("--");
    }else {
        ui->CoinSupplyMax->setText(locale.toString(jsonObj["market_data"].toObject()["max_supply"].toDouble(), 'f', 0));
    }
}

void CoinPage::setUpComboBox(QJsonObject jsonObj, QString section, QComboBox* cb) {
    QJsonArray arr = jsonObj["links"].toObject()[section].toArray();
    if(arr.size() == 0 or arr[0].toString() == "") {
        cb->setVisible(false);
    }else {
        for(int i = 0; i < arr.size(); i++) {
            if(arr[i].toString() == "") {
                return;
            }
            cb->addItem(linkShort(arr[i].toString()));
        }
    }
}

QString CoinPage::linkShort(QString link) {
    QUrl url = *new QUrl(link);
    url = url.adjusted(QUrl::RemoveScheme);
    url = url.adjusted(QUrl::RemovePath);
    url = url.adjusted(QUrl::RemovePort);
    return url.toString(QUrl::None).remove("/");
}

void CoinPage::section2Links(QJsonObject jsonObj) {
    QListView* helper;
    helperObj = jsonObj;

    setUpComboBox(jsonObj, "homepage", ui->CoinHomepage);
    helper = qobject_cast<QListView *>(ui->CoinHomepage->view());
    helper->setRowHidden(0, true);
    connect(ui->CoinHomepage, &QComboBox::activated, this, &CoinPage::activateComboBox);

    setUpComboBox(jsonObj, "blockchain_site", ui->CoinWebsite);
    helper = qobject_cast<QListView *>(ui->CoinWebsite->view());
    helper->setRowHidden(0, true);
    connect(ui->CoinWebsite, &QComboBox::activated, this, &CoinPage::activateComboBox);

    setUpComboBox(jsonObj, "official_forum_url", ui->CoinForum);
    helper = qobject_cast<QListView *>(ui->CoinForum->view());
    helper->setRowHidden(0, true);
    connect(ui->CoinForum, &QComboBox::activated, this, &CoinPage::activateComboBox);

    setUpComboBox(jsonObj, "chat_url", ui->CoinChat);
    helper = qobject_cast<QListView *>(ui->CoinChat->view());
    helper->setRowHidden(0, true);
    connect(ui->CoinChat, &QComboBox::activated, this, &CoinPage::activateComboBox);

    setUpComboBox(jsonObj, "announcement_url", ui->CoinAnnouncement);
    helper = qobject_cast<QListView *>(ui->CoinAnnouncement->view());
    helper->setRowHidden(0, true);
    connect(ui->CoinAnnouncement, &QComboBox::activated, this, &CoinPage::activateComboBox);

    //social medias link
    if(jsonObj["links"].toObject()["twitter_screen_name"].toString() != "") {
        ui->CoinSocialMedia->addItem(QIcon(":/CoinPage/icons/CoinPage/twitter.png"), "Twitter");
    }
    if(jsonObj["links"].toObject()["facebook_username"].toString() != "") {
        ui->CoinSocialMedia->addItem(QIcon(":/CoinPage/icons/CoinPage/facebook.png"), "Facebook");
    }
    if(jsonObj["links"].toObject()["telegram_channel_identifier"].toString() != "") {
        ui->CoinSocialMedia->addItem(QIcon(":/CoinPage/icons/CoinPage/telegram.png"), "Telegram");
    }
    if(jsonObj["links"].toObject()["subreddit_url"].toString() != "") {
        ui->CoinSocialMedia->addItem(QIcon(":/CoinPage/icons/CoinPage/reddit.png"), "Reddit");
    }

    helper = qobject_cast<QListView *>(ui->CoinSocialMedia->view());
    helper->setRowHidden(0, true);
    connect(ui->CoinSocialMedia, &QComboBox::activated, this, &CoinPage::activateComboBox);

    //git link
    if(jsonObj["links"].toObject()["github"].toArray().size() == 0 and jsonObj["links"].toObject()["bitbucket"].toArray().size() == 0) {
        ui->CoinRepos->setVisible(false);
    }else {
        if(jsonObj["links"].toObject()["github"].toArray().size() != 0) {
            ui->CoinRepos->addItem(QIcon(":/CoinPage/icons/CoinPage/github.png"), "Github");
        }
        if(jsonObj["links"].toObject()["bitbucket"].toArray().size() != 0) {
            ui->CoinRepos->addItem(QIcon(":/CoinPage/icons/CoinPage/bitbucket.png"), "Bit Bucket");
        }
    }

    helper = qobject_cast<QListView *>(ui->CoinRepos->view());
    helper->setRowHidden(0, true);
    connect(ui->CoinRepos, &QComboBox::activated, this, &CoinPage::activateComboBox);
}

void CoinPage::activateComboBox(int index) {
    QComboBox *s = qobject_cast<QComboBox*> (sender());
    if(s == ui->CoinHomepage) {
        QString url = helperObj["links"].toObject()["homepage"].toArray()[index - 1].toString();
        QDesktopServices::openUrl(QUrl(url));
    }
    if(s == ui->CoinWebsite) {
        QString url = helperObj["links"].toObject()["blockchain_site"].toArray()[index - 1].toString();
        QDesktopServices::openUrl(QUrl(url));
    }
    if(s == ui->CoinForum) {
        QString url = helperObj["links"].toObject()["official_forum_url"].toArray()[index - 1].toString();
        QDesktopServices::openUrl(QUrl(url));
    }
    if(s == ui->CoinChat) {
        QString url = helperObj["links"].toObject()["chat_url"].toArray()[index - 1].toString();
        QDesktopServices::openUrl(QUrl(url));
    }
    if(s == ui->CoinAnnouncement) {
        QString url = helperObj["links"].toObject()["announcement_url"].toArray()[index - 1].toString();
        QDesktopServices::openUrl(QUrl(url));
    }
    if(s == ui->CoinSocialMedia) {
        if(s->itemText(index) == "Twitter") {
            QString url = "https://twitter.com/" + helperObj["links"].toObject()["twitter_screen_name"].toString();
            QDesktopServices::openUrl(QUrl(url));
        }
        if(s->itemText(index) == "Facebook") {
            QString url = "https://www.facebook.com/" + helperObj["links"].toObject()["facebook_username"].toString() + "/";
            QDesktopServices::openUrl(QUrl(url));
        }
        if(s->itemText(index) == "Telegram") {
            QString url = "https://t.me/" + helperObj["links"].toObject()["telegram_channel_identifier"].toString();
            QDesktopServices::openUrl(QUrl(url));
        }
        if(s->itemText(index) == "Reddit") {
            QString url = "https://www.reddit.com/r/" + helperObj["links"].toObject()["subreddit_url"].toString();
            QDesktopServices::openUrl(QUrl(url));
        }
    }

    if(s == ui->CoinRepos) {
        if(s->itemText(index) == "Github") {
            QString url = "https://github.com/" + helperObj["links"].toObject()["repos_url"].toObject()["github"].toArray()[0].toString();
            QDesktopServices::openUrl(QUrl(url));
        }
        if(s->itemText(index) == "Bit Bucket") {
            QString url = "https://bitbucket.org/" + helperObj["links"].toObject()["repos_url"].toObject()["bitbucket"].toArray()[0].toString();
            QDesktopServices::openUrl(QUrl(url));
        }
    }
    s->setCurrentIndex(0);
}

void CoinPage::managerFinished(QNetworkReply* reply) {
    QPixmap pixmap;
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error in" << reply->url() << ":" << reply->errorString();
        return;
    }
    QByteArray img = reply->readAll();
    pixmap.loadFromData(img);
    pixmap = pixmap.scaled(QSize(32,32), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->CoinImage->setPixmap(pixmap);
}
