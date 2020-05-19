#include "h_webscrapper.h"


WebScrapper::WebScrapper(string NAME, string HTML)
{
    name = NAME;
    html = HTML;
    fillData();
}

double WebScrapper::findNum(string html)
{
    int i = 0;
    string output;
    bool wasInt=false, isInt=false;
    while(html[i]!='\0'&&!wasInt)
    {
        //If parser finds NULL value in wsj html, it sets value to 0 and exits while loop.
        if(html[i] == '-' && html[i+1] == ' ')
        {
            output = "0";
            wasInt = true;
        }

        //Code recognizes first integer, returns true
        if(isdigit(html[i]) || (html[i] == '-' && isdigit(html[i+1])))
        {
            isInt=true;
        }
        //Since above is true it sets isInt to true thus accepting this if statement.
        if(isInt)
        {
            //Lets parser know that the value is now over, and outputs the value.
            if(html[i]!='.'&&!isdigit(html[i]) && html[i] != '-')
            {
                wasInt = true;
                isInt = false;
            }
            else output+=html[i];
        }
        i++;
    }
    return stod(output);
}

string WebScrapper::findString(string html)
{
    int i = 0;
    string output;
    bool wasInt=false, isInt=false;
    while(html[i]!='\0'&&!wasInt)
    {
        //If parser finds NULL value in wsj html, it sets value to 0 and exits while loop.
        if(html[i] == '-' && html[i+1] == ' ')
        {
            output = "0";
            wasInt = true;
        }

        //Code recognizes first integer, returns true
        if(isdigit(html[i]) || (html[i] == '-' && isdigit(html[i+1])))
        {
            isInt=true;
        }

        //Since above is true it sets isInt to true thus accepting this if statement.
        if(isInt)
        {
            //Parser recognizes the end of the requested value, terminates while loop and outputs the value. (B and M represent billions and millions on certain values)
            if(html[i]!='.'&&!isdigit(html[i]) && html[i] != '-' && html[i] != ' ' && html[i] != 'B' && html[i] != 'M')
            {
                wasInt = true;
                isInt = false;
            }
            else output+=html[i];
        }
        i++;
    }
    return output;
}

void WebScrapper::fillData()
{

    //If webscrapper cannot find symbol, defaults all values to 0.
    if(html.find("Quote Not Found page for WSJ Market Data") != string::npos)
    {
         peRatio = 0;
         pcfRatio = 0;
         pbRatio = 0;
         psRatio = 0;
         qtrEpsEstimate = 0;
         annEpsEst = 0;
         qtrLastYear = 0;
         annLastYear = 0;
         totalCash = "";
         totalDebt = "";
         totalLiabilites = "";
         bookValueTotal = 0;
         totalDebtEquity = 0;
         totalDebtCapital = 0;
         totalDebtAssets = 0;
         interestCoverage = 0;
         lastReport = "";
         nextReport = "";
         fiscalEnd = "";
    }

    else //Assigns values to requested varaibles.
    {
        // P/E Ratio
        size_t indexpeRatio = html.find("P/E Ratio ");
        peRatio = findNum(html.substr(indexpeRatio, html.length() - indexpeRatio));

        // P/CF Ratio
        size_t indexpcfRatio = html.find("Price to Cash Flow Ratio");
        pcfRatio = findNum(html.substr(indexpcfRatio, html.length() - indexpcfRatio));

        // P/S Ratio
        size_t indexpsRatio = html.find("Price to Sales Ratio");
        psRatio = findNum(html.substr(indexpsRatio, html.length() - indexpsRatio));

        // P/B Ratio
        size_t indexpbRatio = html.find("Price to Book Ratio");
        pbRatio = findNum(html.substr(indexpbRatio, html.length() - indexpbRatio));

        //Next Quarter EPS Estimate
        size_t indexqtrEpsEstimate = html.find("Qtr. EPS Est.");
        qtrEpsEstimate = findNum(html.substr(indexqtrEpsEstimate, html.length() - indexqtrEpsEstimate));

        //Annual Eps Estimate
        size_t indexAnnEps = html.find("Ann. EPS Est.");
        annEpsEst = findNum(html.substr(indexAnnEps, html.length() - indexAnnEps));

        //Quarter EPS Estimate Last Year
        size_t indexqtrLastYear = html.find("Qtr. Year Ago");
        qtrLastYear = findNum(html.substr(indexqtrLastYear, html.length() - indexqtrLastYear));

        //Annual Eps Last Year
        size_t indexAnnLastYear = html.find("Ann. Year Ago");
        annLastYear = findNum(html.substr(indexAnnLastYear, html.length() - indexAnnLastYear));

        //Total Cash
        size_t indextotalCash = html.find("Cash &amp; Short-Term Investment");
        string s_tCash = findString(html.substr(indextotalCash, html.length() - indextotalCash));
        totalCash = QString::fromStdString(s_tCash);

        //Total Debt
        size_t indextotalDebt = html.find("Total Debt<", 0, 11);
        string s_tDebt = findString(html.substr(indextotalDebt, html.length() - indextotalDebt));
        totalDebt = QString::fromStdString(s_tDebt);

        //Total Liabilities
        size_t indextotalLiabilites = html.find("Total Liabilities");
        string s_tLib = findString(html.substr(indextotalLiabilites, html.length() - indextotalLiabilites));
        totalLiabilites = QString::fromStdString(s_tLib);

        //Book Value
        size_t indexBookValue = html.find("Book Value Per Share");
        bookValueTotal = findNum(html.substr(indexBookValue, html.length() - indexBookValue));

        //Total Debt-Equity
        size_t indextotalDebtEquity = html.find("Total Debt to Total Equity");
        totalDebtEquity = findNum(html.substr(indextotalDebtEquity, html.length() - indextotalDebtEquity));

        //Total Debt-Capital
        size_t indextotalDebtCapital = html.find("Total Debt to Total Capital");
        totalDebtCapital = findNum(html.substr(indextotalDebtCapital, html.length() - indextotalDebtCapital));

        //Total Debt-Assets
        size_t indextotalDebtAssets = html.find("Total Debt to Total Assets");
        totalDebtAssets = findNum(html.substr(indextotalDebtAssets, html.length() - indextotalDebtAssets));

        //Interest Coverage
        size_t indexinterestCoverage = html.find("Interest Coverage");
        interestCoverage = findNum(html.substr(indexinterestCoverage, html.length() - indexinterestCoverage));

        //Last Report
        size_t indexLastReport = html.find("Last Report") + 43;
        string s_lastReport = html.substr(indexLastReport, 11);
        lastReport = QString::fromStdString(s_lastReport);

        //Next Report
        size_t indexNextReport = html.find("Next Report") + 43;
        string s_NextReport = html.substr(indexNextReport, 11);
        nextReport = QString::fromStdString(s_NextReport);

        //Fiscal Year End
        size_t indexFiscalEnd = html.find("Fiscal Yr Ends") + 46;
        string s_FiscalEnd = html.substr(indexFiscalEnd, 13);
        fiscalEnd = QString::fromStdString(s_FiscalEnd);
    }




}




