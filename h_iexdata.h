#ifndef IEXDATA_H
#define IEXDATA_H


#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <curl/curl.h>
#include <locale> //locale, isdigit
#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

using namespace std;

#define IEX_ENDPOINT "https://sandbox.iexapis.com/v1/"
#define API_TOKEN "Tpk_eb10dc4b66434ecdad0c4edf1594da69"

namespace IEX {
    void sendGetRequest(Json::Value &data, const string url);
    bool isValidSymbol(const string &);
    vector<string> getSymbolList();
    void parseSymbolData(const Json::Value &, vector<string> &);
    namespace stocks {
        Json::Value batch(const string &);
        Json::Value book(const string &);
        Json::Value chart(const string &);
        Json::Value chartYtd(const string &);
        Json::Value chartRange(const string &, const string &);
        Json::Value chartDate(const string &, const string &);
        Json::Value chartDynamic(const string &);
        Json::Value company(const string &);
        Json::Value crypto();
        Json::Value delayedQuote(const string &);
        Json::Value dividends(const string &, const string &);
        Json::Value earnings(const string &);
        Json::Value earningsToday();
        Json::Value effectiveSpread(const string &);
        Json::Value financials(const string &);
        Json::Value upcomingIPOS();
        Json::Value todayIPOS();
        Json::Value stats(const string &);
        Json::Value largestTrades(const string &);
        Json::Value list(const string &);
        Json::Value logo(const string &);
        Json::Value news(const string &, int last = 0);
        Json::Value OHLC(const string &);
        Json::Value peers(const string &);
        Json::Value previous(const string &);
        Json::Value price(const string &);
        Json::Value quote(const string &, bool displayPercent = false);
        Json::Value relevant(const string &);
        Json::Value sectorPerformance();
        Json::Value splits(const string &, const string &);
        Json::Value relevant(const string &);
        Json::Value timeSeries(const string &);
        Json::Value VolumeByVenue(const string &);
    }

    namespace refData {
        Json::Value symbols();
        Json::Value corporateActions(string date = "");
        Json::Value dividends(string date = "");
        Json::Value nextDayExDate(string date = "");
        Json::Value symbolDirectory(string date = "");
    }



    namespace stats {
        Json::Value intraday();
        Json::Value recent();
        Json::Value records();
        Json::Value historical(string date = "");
        Json::Value historicalDaily(string date = "");
    }

    namespace markets {
        Json::Value market();
    }
}

#endif // IEXDATA_H

