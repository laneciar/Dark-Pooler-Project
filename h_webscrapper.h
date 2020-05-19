#ifndef WEBSCRAPPER_H
#define WEBSCRAPPER_H

#include <QMainWindow>
#include <QObject>
#include <QColor>
#include <QWidget>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <string>
#include "h_mainwindow.h"
#include <ctype.h>

using namespace std;

class WebScrapper {
private:
    string html;
    string sign;
    string name;


public:
    double peRatio;
    double pcfRatio;
    double pbRatio;
    double psRatio;
    double qtrEpsEstimate;
    double annEpsEst;
    double qtrLastYear;
    double annLastYear;
    QString totalCash;
    QString totalDebt;
    QString totalLiabilites;
    double bookValueTotal;
    double totalDebtEquity;
    double totalDebtCapital;
    double totalDebtAssets;
    double interestCoverage;
    QString lastReport;
    QString nextReport;
    QString currentDate;
    QString fiscalEnd;



    //NAME The ticker of the company.
    //HTML The html to pull info from.
    WebScrapper (string NAME, string HTML);

    //Fills data from parser
    void fillData();

    //Finds value if its needed to be an int/double
    double findNum(string HTML);

    //Finds value if its needed to be a string
    string findString(string HTML);
};

#endif // WEBSCRAPPER_H
