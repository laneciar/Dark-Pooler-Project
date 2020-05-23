#include "h_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint)
    , ui(new Ui::MainWindow)
{
    setup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Main Program UI Setup
void MainWindow::setup()
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

    symbolSearched = "TWTR";
    symbolSearchedStd = "TWTR";

    assignStatistics();
    oneMin_Line_Graph();

    //Creates Background
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    ui->stockGraph->setBackground(plotGradient);

    //Creates Axis Rectangle and color
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(39, 39, 39));
    ui->stockGraph->axisRect()->setBackground(axisRectGradient);
}

void MainWindow::setupLineChart(QString timeFormat, Json::Value chartData)
{
    ui->stockGraph->addGraph(ui->stockGraph->xAxis, ui->stockGraph->yAxis2);

    //Creates line properties
    QPen linePen;
    linePen.setStyle(Qt::PenStyle::SolidLine);
    linePen.setColor(QColor(255,255,255));
    linePen.setWidth(1);
    ui->stockGraph->graph(0)->setPen(linePen);
    ui->stockGraph->graph(0)->setBrush(QBrush(QColor(211,211,211, 70)));
    ui->stockGraph->graph(0)->setLineStyle(QCPGraph::lsLine);

    //Creates Y Axis on right side
    QSharedPointer<QCPAxisTicker> valueTicker(new QCPAxisTicker);
    ui->stockGraph->yAxis2->setTicker(valueTicker);
    ui->stockGraph->yAxis->setVisible(false);
    ui->stockGraph->yAxis2->setVisible(true);
    ui->stockGraph->yAxis2->setSubTicks(false);
    ui->stockGraph->yAxis2->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->yAxis2->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->yAxis2->setTickLabelColor(Qt::white);
    ui->stockGraph->yAxis2->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->yAxis2->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->yAxis2->grid()->setSubGridVisible(true);
    ui->stockGraph->yAxis2->grid()->setZeroLinePen(Qt::NoPen);
    ui->stockGraph->yAxis2->grid()->setVisible(true);

    //Creates X Axis on bottom
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat(timeFormat);
    dateTicker->setDateTimeSpec(Qt::UTC);
    dateTicker->setTickCount(8);
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);

    //Vectors to store data
    QVector<double> value(390), epoch(390);
    QVector<string> time(390);


    //Reads in data from json(historical data 1 day delayed)
    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {

        value[i] = (chartData[i]["average"].asDouble());

        if((value[i] == 0) && (i != chartData.size() - 1))
        {
            value[i] = value[i-1];
        }

        time[i] = chartData[i]["minute"].asString();
        QString temp = QString::fromStdString(time[i]);
        //epoch[i] = QDateTime::fromString(temp, "hh::mm").toSecsSinceEpoch();
        epoch[i] = 28800 + i * 60;

    }

    //Adds data from the day
    ui->stockGraph->graph(0)->addData(epoch, value);

    //Sets temporary range
    ui->stockGraph->xAxis->setRange(epoch[0], epoch[chartData.size()-1]  + 3600);
    ui->stockGraph->yAxis2->setRange(25, 40);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->stockGraph->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->stockGraph->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->stockGraph->yAxis2, SIGNAL(rangeChanged(QCPRange)), ui->stockGraph->yAxis, SLOT(setRange(QCPRange)));
}

//For mouse movement position on click
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QGroupBox *child = static_cast<QGroupBox*>(childAt(event->pos()));
        if (child!=ui->groupBox)        //mTitlebar is the QLabel on which we want to implement window drag
        {
            return;
        }
        isMousePressed = true;
        startPos = event->pos();
}

//Moves window when user drags
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(isMousePressed)
        {
            QPoint deltaPos = event->pos() - startPos;
            this->move(this->pos()+deltaPos);
        }
}

//Release window
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    QGroupBox *child = static_cast<QGroupBox*>(childAt(event->pos()));
        if (child!=ui->groupBox)        //mTitlebar is the QLabel on which we want to implement window drag
        {
            return;
        }
        isMousePressed = false;
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

//Current Clock Time
void MainWindow::currentTime()
{
    QTime time = QTime::currentTime();
    QString time_text = time.toString("hh : mm : ss ap");
    ui->timeClock->setText(time_text);
}



//User inputs symbol of stock
void MainWindow::on_symbolSearch_returnPressed()
{

    symbolSearched = ui->symbolSearch->text();
    symbolSearched = symbolSearched.toUpper();
    symbolSearchedStd = symbolSearched.toStdString();

    if(!IEX::isValidSymbol(symbolSearchedStd))
    {
        QMessageBox::warning(this, "Dark Pooler", "Not valid company, please try again");

    }

    else
    {
        assignStatistics();

        if(isLineChart == true)
            ytd_Line_Graph();

        else if(isCandlestickChart == true)
            ytd_Candle_Graph();
    }
}

//Choses chart style from combo box
void MainWindow::on_chartStlyeBox_currentIndexChanged(int index)
{
    if(index == 0)
    {
        clearChart();
        ytd_Line_Graph();
        isLineChart = true;
        isCandlestickChart = false;
    }

    else if(index == 1)
    {
        clearChart();
        ytd_Candle_Graph();
        isCandlestickChart = true;
        isLineChart = false;
    }
}

//Clears chart when new chart is selected
void MainWindow::clearChart()
{
    if(isLineChart == true)
    {
        ui->stockGraph->graph(0)->data().data()->clear();
        ui->stockGraph->replot();
        isLineChart = false;
    }

    else if(isCandlestickChart == true)
    {
        candlesticks->data().data()->clear();
        ui->stockGraph->replot();
        isCandlestickChart = true;
    }
}

//Assigns all data from company statistics
void MainWindow::assignStatistics()
{

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
    return;
}

//Assigns stock data to show at the top of the screen
void MainWindow::stockData(double num1, double num2)
{
    //Gives stock symbol and ending price
    ui->stockName->setText(symbolSearched);
    ui->stockPrice->setText('$' + QString::number(num1));

    double valueChange = num1 - num2;
    QString valueChangeStr = QString::number(valueChange);

    //Daily change of price is below 0
    if (valueChange < 0)
    {
        ui->stockPriceChange->setText("($"+valueChangeStr+')');
        ui->stockPriceChange->setStyleSheet("color:rgb(255, 0, 0)");
    }

    //Daily change of price is above 0.
    else if(valueChange > 0)
    {
        ui->stockPriceChange->setText("($"+valueChangeStr+')');
        ui->stockPriceChange->setStyleSheet("color:rgb(0, 170, 0)");
    }
}

void MainWindow::readData()
{
    Json::Value chartData = IEX::stocks::Quote(symbolSearchedStd);
    counter = counter + 1;
    ui->stockGraph->graph(0)->addData(counter, chartData["latestPrice"].asDouble());
    ui->stockGraph->xAxis->setRange(counter, 12, Qt::AlignRight);
    ui->stockGraph->yAxis2->setRange(chartData["latestPrice"].asDouble(), 5, Qt::AlignTop);
    ui->stockGraph->replot();
}

//Shows year-to-date line graph
void MainWindow::ytd_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartYtd(symbolSearchedStd);

    //Stores x and y values
    QVector<double> value(365), timeInEpoch(365);
    QVector<string>time(365);

    int  n = chartData.size();

    //Finds max and min for range
    float maxAvg = chartData[0]["close"].asDouble();
    float minAvg = chartData[0]["close"].asDouble();

    ui->stockGraph->addGraph(ui->stockGraph->xAxis, ui->stockGraph->yAxis2);

    QPen linePen;
    linePen.setStyle(Qt::PenStyle::SolidLine);
    linePen.setColor(QColor(255,255,255));
    linePen.setWidth(1);
    ui->stockGraph->graph(0)->setPen(linePen);
    ui->stockGraph->graph(0)->setBrush(QBrush(QColor(211,211,211, 70)));
    ui->stockGraph->graph(0)->setLineStyle(QCPGraph::lsLine);

    //Creates Y Axis on right side
    QSharedPointer<QCPAxisTicker> valueTicker(new QCPAxisTicker);
    valueTicker->setTickCount(10);
    ui->stockGraph->yAxis2->setTicker(valueTicker);
    ui->stockGraph->yAxis->setVisible(false);
    ui->stockGraph->yAxis2->setVisible(true);
    ui->stockGraph->yAxis2->setSubTicks(false);
    ui->stockGraph->yAxis2->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->yAxis2->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->yAxis2->setTickLabelColor(Qt::white);
    ui->stockGraph->yAxis2->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->yAxis2->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->yAxis2->grid()->setSubGridVisible(true);
    ui->stockGraph->yAxis2->grid()->setZeroLinePen(Qt::NoPen);
    ui->stockGraph->yAxis2->grid()->setVisible(true);


    //Creates X Axis on bottom
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("MMM d, \nyyyy");
    dateTicker->setDateTimeSpec(Qt::UTC);
    dateTicker->setTickCount(12);
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);

    //Rescale Axis
    ui->stockGraph->rescaleAxes();

    //Reads in data from json(historical data 1 day delayed)
    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {
        if(chartData[i].isMember("close"))
        {
            value[i] = (chartData[i]["close"].asDouble());
            time[i] = chartData[i]["date"].asString();
            timeInEpoch[i] = QDateTime::fromString(time[i].c_str(), Qt::ISODate).toSecsSinceEpoch();

            if((value[i] == 0) && (i != chartData.size() - 1))
            {
                value[i] = value[i-1];
            }

            if(value[i] > maxAvg)
            {
                maxAvg = value[i];
            }

            else if(value[i] < minAvg)
            {
                minAvg = value[i];
            }
        }
    }

    stockData(value[n-1], value[n-2]);

    //Assigns data to graph
    ui->stockGraph->graph(0)->setData(timeInEpoch, value);

    //Set x axis range
    ui->stockGraph->xAxis->setRange(timeInEpoch[0], timeInEpoch[n-1]);

    //Set y axis range
    QCPRange yAxis(minAvg + 2, maxAvg + 2);
    yAxis.normalize();
    yAxis.center();
    ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
    ui->stockGraph->replot();


}

void MainWindow::oneMin_Line_Graph()
{   
    Json::Value chartData = IEX::stocks::intraday(symbolSearchedStd);
    cout << chartData << endl;

    //Sets up and plots intial chart
    setupLineChart("hh:mm ap", chartData);

    //Starts real time data
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(readData()));
    dataTimer.start(1000);

}

//Shows year-to-date candlestick graph
void MainWindow::ytd_Candle_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartYtd(symbolSearchedStd);

    //Stores x and y values
    QVector<double> highPrice(365), lowPrice(365), openPrice(365), value(365), timeInEpoch(365);
    QVector<string>time(365);

    int  n = chartData.size();
    double binSize = 3600*24;

    float maxAvg = chartData[0]["high"].asDouble();
    float minAvg = chartData[0]["low"].asDouble();

    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {
        if(chartData[i].isMember("high"))
        {
            highPrice[i] = (chartData[i]["high"].asDouble());

            if((highPrice[i] == 0) && (i != chartData.size() - 1))
            {
                highPrice[i] = highPrice[i-1];
            }

            if(highPrice[i] > maxAvg)
            {
                maxAvg = highPrice[i];
            }
        }

        if(chartData[i].isMember("low"))
        {
            lowPrice[i] = (chartData[i]["low"].asDouble());

            if((lowPrice[i] == 0) && (i != chartData.size() - 1))
            {
                lowPrice[i] = lowPrice[i-1];
            }

            if(lowPrice[i] < minAvg)
            {
                minAvg = lowPrice[i];
            }
        }

        if(chartData[i].isMember("open"))
        {
            openPrice[i] = (chartData[i]["open"].asDouble());

            if((openPrice[i] == 0) && (i != chartData.size() - 1))
            {
                openPrice[i] = openPrice[i-1];
            }
        }

        if(chartData[i].isMember("close"))
        {
            value[i] = (chartData[i]["close"].asDouble());

            if((value[i] == 0) && (i != chartData.size() - 1))
            {
                value[i] = value[i-1];
            }
        }

        time[i] = chartData[i]["date"].asString();
        QString temp = QString::fromStdString(time[i]);
        timeInEpoch[i] = QDateTime::fromString(time[i].c_str(), Qt::ISODate).toSecsSinceEpoch();
    }

    stockData(value[n-1], value[n-2]);

    // create candlestick chart:
    candlesticks = new QCPFinancial(ui->stockGraph->xAxis, ui->stockGraph->yAxis2);
    candlesticks->setName("Candlestick");
    candlesticks->setChartStyle(QCPFinancial::csCandlestick);
    candlesticks->setData(timeInEpoch, openPrice, highPrice, lowPrice, value, false);
    candlesticks->setWidth(binSize*0.9);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(0,128,0));
    candlesticks->setBrushNegative(QColor(255,0,0));
    candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
    candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));

    //Creates Y Axis on right side
    QSharedPointer<QCPAxisTicker> valueTicker(new QCPAxisTicker);
    valueTicker->setTickCount(10);
    ui->stockGraph->yAxis2->setTicker(valueTicker);
    ui->stockGraph->yAxis->setVisible(false);
    ui->stockGraph->yAxis2->setVisible(true);
    ui->stockGraph->yAxis2->setSubTicks(false);
    ui->stockGraph->yAxis2->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->yAxis2->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->yAxis2->setTickLabelColor(Qt::white);
    ui->stockGraph->yAxis2->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->yAxis2->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->yAxis2->grid()->setSubGridVisible(true);
    ui->stockGraph->yAxis2->grid()->setZeroLinePen(Qt::NoPen);
    ui->stockGraph->yAxis2->grid()->setVisible(true);


    //Creates X Axis on bottom
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("MMM d, \nyyyy");
    dateTicker->setDateTimeSpec(Qt::UTC);
    dateTicker->setTickCount(12);
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);

    //Rescale Axis
    ui->stockGraph->rescaleAxes();

    //Creates Background
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    ui->stockGraph->setBackground(plotGradient);

    //Creates Axis Rectangle and color
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(39, 39, 39));
    ui->stockGraph->axisRect()->setBackground(axisRectGradient);

    //Set x axis range
    ui->stockGraph->xAxis->setRange(timeInEpoch[0] - 86400, timeInEpoch[n-1] + 86400);

    //Set y axis range
    QCPRange yAxis(minAvg + 2, maxAvg + 2);
    yAxis.normalize();
    yAxis.center();
    ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
    ui->stockGraph->replot();

}
