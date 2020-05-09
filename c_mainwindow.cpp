#include "h_mainwindow.h"
#include "h_login.h"
#include "h_webcurl.h"
#include "h_webscrapper.h"
#include "h_iexdata.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint)
    , ui(new Ui::MainWindow)
{

    //Clock Setup
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(currentTime()));
    timer->start(1000);

    //Date Setup
    QString format = "MMMM dd, yyyy";
    QString date = QDate::currentDate().toString(format);
    ui->currentDate->setText(date);

    ui->stockGraph->addGraph();
    ui->stockGraph->graph(0)->setPen(QPen(Qt::black));

    QString aapl = "AAPL";
    ytdGraphLine(aapl);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//For mouse movement position on click
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mouseClick_X = event->x();
    mouseClick_Y = event->y();
}

//Moves window when user drags
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalX()-mouseClick_X, event->globalY()-mouseClick_Y);
}

//Exits window
void MainWindow::on_exitButton_clicked()
{
    this->close();
}

//Minimizes Screen
void MainWindow::on_minimizeButton_clicked()
{
    this->showMinimized();
}

//Maximizes Screen
void MainWindow::on_maximizeButton_clicked()
{
    if(isMaximized == false)
    {
        this->showMaximized();
        isMaximized = true;
    }
    else if(isMaximized ==true)
    {
       this->showNormal();
       isMaximized = false;
    }
}

//User inputs symbol of stock
void MainWindow::on_symbolSearch_returnPressed()
{
    symbolSearched = ui->symbolSearch->text();
    ytdGraphLine(symbolSearched);
    assignStatistics(symbolSearched);


}

//User selects line chart
void MainWindow::on_lineChartButton_clicked()
{
}

//Current Clock Time
void MainWindow::currentTime()
{
    QTime time = QTime::currentTime();
    QString time_text = time.toString("hh : mm : ss ap");
    ui->timeClock->setText(time_text);
}

//Assigns all data from company statistics
void MainWindow::assignStatistics(QString symbol)
{
    const auto symbolSearchedStd = symbol.toStdString();
    pageAddress = "https://www.wsj.com/market-data/quotes/" + symbolSearchedStd + "/financials";

    CurlObj webPage(pageAddress);
    WebScrapper web = WebScrapper(symbolSearchedStd, webPage.getData());



    ui->peRatio->setNum(web.peRatio);
    ui->pcfRatio->setNum(web.pcfRatio);
    ui->psRatio->setNum(web.psRatio);
    ui->pbRatio->setNum(web.pbRatio);
    ui->qtrEPS->setNum(web.qtrEpsEstimate);
    ui->nxtReportL->setNum(web.annEpsEst);
    ui->QtrYearAgoL->setNum(web.qtrLastYear);
    ui->LastReport->setNum(web.annLastYear);
    ui->totalCash->setText(web.totalCash);
    ui->totalDebt->setText(web.totalDebt);
    ui->totalLiabilities->setText(web.totalLiabilites);
    ui->bookValue->setNum(web.bookValueTotal);
    ui->debtEquity->setNum(web.totalDebtEquity);
    ui->debtCapital->setNum(web.totalDebtCapital);
    ui->debtAssets->setNum(web.totalDebtAssets);
    ui->interestCoverageL->setNum(web.interestCoverage);
    ui->lastEPSDate->setText(web.lastReport);
    ui->nextEPSDate->setText(web.nextReport);
    ui->fiscalEnd->setText(web.fiscalEnd);
    return;
}

//Shows year-to-date graph of specific company
void MainWindow::ytdGraphLine(QString symbol)

{
    string symbol_std = symbol.toStdString();


    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartYtd(symbol_std);

    //Size of json
    int n = chartData.size();

    //Stores x and y values
    QVector<double> time(n), closePrice(n);

    //Intialize first vector to first values
    closePrice[0] = chartData[0]["close"].asDouble();

    //Finds max and min for range
    float maxAvg = closePrice[0];
    float minAvg = closePrice[0];


    //Reads in data from json(historical data 1 day delayed)
    for(int i = 1; i < n; i++)
    {
        time[i] = i + 1;
        closePrice[i] = (chartData[i]["close"].asDouble());

        if((closePrice[i] == 0) && (time[i] != chartData.size() - 1))
        {
            closePrice[i] = closePrice[i-1];
        }

        if(closePrice[i] > maxAvg)
        {
            maxAvg = closePrice[i];
        }

        else if(closePrice[i] < minAvg)
        {
            minAvg = closePrice[i];
        }

    }


    ui->stockName->setText(symbol);
    ui->stockPrice->setNum(closePrice[chartData.size() - 1]);

    double closePriceChange = closePrice[chartData.size() - 1] - closePrice[chartData.size() - 2];
    QString closePriceChangeStr = QString::number(closePriceChange);

    if (closePriceChange < 0)
    {
        ui->stockPriceChange->setText('('+closePriceChangeStr+')');
        ui->stockPriceChange->setStyleSheet("color:rgb(255, 0, 0)");


    }

    else if(closePriceChange > 0)
    {
        ui->stockPriceChange->setText('('+closePriceChangeStr+')');
        ui->stockPriceChange->setStyleSheet("color:rgb(0, 170, 0)");

    }

    //Initializes graph
    ui->stockGraph->addGraph();
    ui->stockGraph->graph(0)->setData(time, closePrice);
    ui->stockGraph->graph(0)->setPen(QPen(Qt::white));

    // set some pens, brushes and backgrounds:
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->yAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->yAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->stockGraph->yAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->yAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->yAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->stockGraph->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->stockGraph->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->stockGraph->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    ui->stockGraph->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(39, 39, 39));
    ui->stockGraph->axisRect()->setBackground(axisRectGradient);


    ui->stockGraph->xAxis->setRange(0, 365);
    ui->stockGraph->yAxis->setRange(minAvg - 5, maxAvg + 5);
    ui->stockGraph->replot();





}

