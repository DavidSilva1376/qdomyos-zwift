#include "homefitnessbuddy.h"
#include "zwiftworkout.h"
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QSettings>
#include <QTextDocument>
#include <QtXml>

homefitnessbuddy::homefitnessbuddy(bluetooth *bl, QObject *parent) : QObject(parent) {

    QSettings settings;
    bluetoothManager = bl;
    mgr = new QNetworkAccessManager(this);
    QNetworkCookieJar *cookieJar = new QNetworkCookieJar();
    mgr->setCookieJar(cookieJar);

    startEngine();
}

void homefitnessbuddy::startEngine() {
    QSettings settings;
    connect(mgr, &QNetworkAccessManager::finished, this, &homefitnessbuddy::login_onfinish);
    QUrl url(QStringLiteral("https://app.homefitnessbuddy.com/peloton/powerzone/"));
    QNetworkRequest request(url);

    // request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("qdomyos-zwift"));

    mgr->get(request);
}

void homefitnessbuddy::error(QNetworkReply::NetworkError code) {
    qDebug() << QStringLiteral("homefitnessbuddy ERROR") << code;
}

void homefitnessbuddy::login_onfinish(QNetworkReply *reply) {
    disconnect(mgr, &QNetworkAccessManager::finished, this, &homefitnessbuddy::login_onfinish);
    QByteArray payload = reply->readAll(); // JSON

    qDebug() << QStringLiteral("login_onfinish") << payload;

    QTextStream in(payload);
    QStringList fieldNames;
    QJsonObject row;
    QRegularExpression fieldRe(QStringLiteral("<th(?:\\s+[^>]+)?>([^<]*)</th>"),
                               QRegularExpression::CaseInsensitiveOption);
    QRegularExpression valueRe(QStringLiteral("(?:<td(?:\\s+[^>]+)?>([^<]*)</td>|</tr>)"),
                               QRegularExpression::CaseInsensitiveOption);
    qDebug() << fieldRe.isValid() << valueRe.isValid();
    while (!in.atEnd()) {
        QString line = in.readLine();
        // qDebug() << line;
        QRegularExpressionMatch match = fieldRe.match(line);
        if (match.hasMatch()) {
            fieldNames.append(match.captured(1));
        } else if (!fieldNames.isEmpty() && line.indexOf(QStringLiteral("<TR>")) >= 0) {
            QRegularExpressionMatchIterator i = valueRe.globalMatch(line);
            QRegularExpressionMatch vMatch;
            int indexField = -1;
            int nfields = fieldNames.size();
            QString fieldName;
            while (i.hasNext()) {
                vMatch = i.next();
                if (vMatch.lastCapturedIndex() == 1) {
                    indexField++;
                    if (nfields > indexField) {
                        if (!(fieldName = fieldNames.at(indexField)).isEmpty()) {
                            QTextDocument text;
                            text.setHtml(vMatch.captured(1));
                            if (row[fieldName].toString().isEmpty()) { // to avoid Coach duplication
                                row[fieldName] = text.toPlainText();
                            }
                        }
                    } else {
                        qDebug() << QStringLiteral("BUG? ") << nfields << QStringLiteral("<=") << indexField;
                    }
                } else if (indexField == nfields - 1) {
                    indexField = -1;
                    lessons.append(row);
                    row = QJsonObject();
                } else {
                    qDebug() << QStringLiteral("BUG2? ") << nfields << QStringLiteral("<=") << indexField;
                }
            }
            break;
        }
    }
    // if(token.length()) emit loginState(true);

    // REMOVE IT
    // searchWorkout(QDate(2021,5,19) ,"Christine D'Ercole");
}

void homefitnessbuddy::searchWorkout(QDate date, const QString &coach) {
    for (const QJsonValue &r : qAsConst(lessons)) {
        QDate d = QDate::fromString(r.toObject().value(QStringLiteral("Date")).toString(), QStringLiteral("MM/dd/yy"));
        d = d.addYears(100);
        bool c = !coach.compare(r.toObject().value(QStringLiteral("Coach")).toString());
        if (d == date && c) {
            connect(mgr, &QNetworkAccessManager::finished, this, &homefitnessbuddy::search_workout_onfinish);
            QUrl url(QStringLiteral("https://app.homefitnessbuddy.com/peloton/powerzone/zwift_export.php"));
            QUrlQuery query;
            query.addQueryItem(QStringLiteral("class_id"), r.toObject().value(QStringLiteral("Class ID")).toString());
            url.setQuery(query.query());
            QNetworkRequest request(url);

            // request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("qdomyos-zwift"));

            mgr->get(request);
            return;
        }
    }
}

void homefitnessbuddy::search_workout_onfinish(QNetworkReply *reply) {
    disconnect(mgr, &QNetworkAccessManager::finished, this, &homefitnessbuddy::search_workout_onfinish);
    QByteArray payload = reply->readAll(); // JSON

    qDebug() << QStringLiteral("search_workout_onfinish") << payload;

    QSettings settings;
    // NOTE: clazy-unused-non-trivial-variable
    // QString difficulty = settings.value(QStringLiteral("peloton_difficulty"), QStringLiteral("lower")).toString();

    trainrows.clear();
    trainrows = zwiftworkout::load(payload);

    if (!trainrows.isEmpty()) {
        emit workoutStarted(&trainrows);
    }
}
