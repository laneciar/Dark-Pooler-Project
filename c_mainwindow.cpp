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
    connect(this, SIGNAL(chartChange()), this, SLOT(refresh()));


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

    //assignStatistics();

    h_ytd_Line_Graph();

    isLineChart = true;
    isRealTime = false;
    currentChartRange = "YTD";

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

//Setup Basic Line Cart Design
void MainWindow::setupLineChart(QString timeFormat, float tickCount)
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
    dateTicker->setTickCount(tickCount);
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);

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


//Determine if user is using real time or historical
void MainWindow::on_realTimeBox_stateChanged(int arg1)
{
    //Not Real Time
    if(arg1 == 0)
    {
       dataTimer.stop();
       isRealTime = false;
    }

    //Real Time
    else
    {
      isRealTime = true;
    }

    clearChart();
    emit chartChange();
}

//Choses chart style from combo box
void MainWindow::on_chartStlyeBox_currentIndexChanged(int index)
{
    if(index == 0)
    {
        clearChart();
        isLineChart = true;
    }

    else if(index == 1)
    {
        clearChart();
        isLineChart = false;

    }
    emit chartChange();
}

//Choses time frame that user selects
void MainWindow::on_timeFrame_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "YTD")
    {
        currentChartRange = "YTD";
    }

    else if(arg1 == "1D")
    {
        currentChartRange = "1D";
    }

    else if(arg1 == "1W")
    {
        currentChartRange = "1W";
    }

    else if(arg1 == "1M")
    {
        currentChartRange = "1M";

    }

    else if(arg1 == "6M")
    {
        currentChartRange = "6M";
    }

    else if(arg1 == "1Y")
    {
        currentChartRange = "1Y";
    }
    clearChart();
    emit chartChange();

}

//Determine current chart to be loaded
void MainWindow::refresh()
{

    //Not Real Time
    if(isRealTime == false)
    {

        //Line Chart
        if(isLineChart == true)
        {
            if(currentChartRange == "YTD")
            {
                h_ytd_Line_Graph();
            }

            else if(currentChartRange == "1D")
            {
                h_oneDay_Line_Graph();
            }

            else if(currentChartRange == "1W")
            {
                h_oneWeek_Line_Graph();
            }

            else if(currentChartRange == "1M")
            {
                h_oneMonth_Line_Graph();
            }

            else if(currentChartRange == "6M")
            {
                h_sixMonth_Line_Graph();
            }

            else if(currentChartRange == "1Y")
            {
                h_oneYear_Line_Graph();
            }
        }

        //Candlestick chart
        else if(isLineChart == false)
        {
            if(currentChartRange == "YTD")
            {
                h_ytd_Candle_Graph();
            }

            else if(currentChartRange == "1D")
            {
                h_oneDay_Candle_Graph();
            }

            else if(currentChartRange == "1W")
            {
                h_oneWeek_Candle_Graph();
            }

            else if(currentChartRange == "1M")
            {
                h_oneMonth_Candle_Graph();
            }

            else if(currentChartRange == "6M")
            {
                h_sixMonth_Candle_Graph();
            }

            else if(currentChartRange == "1Y")
            {
                h_oneYear_Candle_Graph();
            }

        }

    }

    //Real Time
    else if(isRealTime == true)
    {
        //Line Chart
        if(isLineChart == true)
        {
            if(currentChartRange == "YTD")
            {
                cout << "error cant use real time data" << endl;
            }

            else if(currentChartRange == "1D")
            {
                rt_oneDay_Line_Graph();
            }

            else if(currentChartRange == "1W")
            {
                rt_oneWeek_Line_Graph();
            }

            else if(currentChartRange == "1M")
            {
                rt_oneMonth_Line_Graph();
            }

            else if(currentChartRange == "6M")
            {
                rt_sixMonth_Line_Graph();
            }

            else if(currentChartRange == "1Y")
            {
                rt_oneYear_Line_Graph();
            }
        }

        //Candlestick Chart
        else if(isLineChart == false)
        {
            if(currentChartRange == "YTD")
            {
                cout << "error cant use real time data" << endl;
            }

            else if(currentChartRange == "1D")
            {
                rt_oneDay_Candle_Graph();
            }

            else if(currentChartRange == "1W")
            {
                rt_oneWeek_Candle_Graph();
            }

            else if(currentChartRange == "1M")
            {
                rt_oneMonth_Candle_Graph();
            }

            else if(currentChartRange == "6M")
            {
                rt_sixMonth_Candle_Graph();
            }

            else if(currentChartRange == "1Y")
            {
                rt_oneYear_Candle_Graph();
            }
        }
    }
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
        refresh();

    }
}

//Clears chart when new chart is selected
void MainWindow::clearChart()
{
    if(isLineChart == true)
    {
        ui->stockGraph->graph(0)->data().data()->clear();
        ui->stockGraph->replot();
    }

    else if(isLineChart == false)
    {
        candlesticks->data().data()->clear();
        ui->stockGraph->replot();
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

//Reads in real time data
void MainWindow::readData()
{
    Json::Value chartData = IEX::stocks::intraday(symbolSearchedStd);
    QVector<double> value, epoch;
    QVector<string> time;

    //Reads in data from json(historical data 1 day delayed)
    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {

        value.push_back(chartData[i]["average"].asDouble());
        if((value[i] == 0) && (i != chartData.size() - 1))
        {
            value[i] = value[i-1];
        }

        //time[i] = chartData[i]["minute"].asString();
        //QString temp = QString::fromStdString(time[i]);
        //epoch[i] = QDateTime::fromString(temp, "hh::mm").toSecsSinceEpoch();
        epoch.push_back(28800 + i * 60);

    }

    //Adds data from the day
    ui->stockGraph->graph(0)->setData(epoch, value);

    //Sets temporary range
    ui->stockGraph->xAxis->setRange(epoch[0], epoch[chartData.size()-1]  + 3600);
    ui->stockGraph->yAxis2->setRange(value[value.size()-1], 5, Qt::AlignCenter);

    ui->stockGraph->replot();
}



            // LINE CHARTS
            //Real Time Charting

//Completed(Still need to fix some ui stuff)
void MainWindow::rt_oneDay_Line_Graph()
{
    setupLineChart("hh: mm ap", 9);
    //Starts real time data
    readData();
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(readData()));
    dataTimer.start(10000);

}

void MainWindow::rt_oneWeek_Line_Graph()
{

}

void MainWindow::rt_oneMonth_Line_Graph()
{

}

void MainWindow::rt_sixMonth_Line_Graph()
{

}

void MainWindow::rt_oneYear_Line_Graph()
{

}


            //Historical Charting

//Completed(Still need to fix x axis)
void MainWindow::h_oneDay_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1d");

    setupLineChart("hh: mm ap", 9);

    //Stores x and y values
    QVector<double> value, epoch;
    QVector<string>time(390);


    int  n = chartData.size();

    //Finds max and min for range
    float maxAvg = chartData[0]["average"].asDouble();
    float minAvg = chartData[0]["average"].asDouble();

    //Reads in data from json(historical data 1 day delayed)
    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {
        if(chartData[i].isMember("average"))
        {
            value.push_back(chartData[i]["average"].asDouble());
            time[i] = chartData[i]["date"].asString();
            epoch.push_back(28800 + i * 60);

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
    ui->stockGraph->graph(0)->setData(epoch, value);

    //Set x axis range
    ui->stockGraph->xAxis->setRange(epoch[0], epoch[chartData.size()-1]  + 3600);

    //Set y axis range
    ui->stockGraph->yAxis2->setRange(value[value.size()-1], 4, Qt::AlignCenter);
    ui->stockGraph->replot();
}

//Need to fix the x axis dates
void MainWindow::h_oneWeek_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "5dm");

    //Stores x and y values
    QVector<double> value, timeInEpoch;
    QVector<string>time(195);
    setupLineChart("ddd, MMM d", 8);

    int  n = chartData.size();

    //Finds max and min for range
    float maxAvg = chartData[0]["average"].asDouble();
    float minAvg = chartData[0]["average"].asDouble();

    //Reads in data from json(historical data 1 day delayed)
    for(int i = 0 ; i < n; i++)
    {
            value.push_back(chartData[i]["average"].asDouble());
            time[i] = chartData[i]["date"].asString();

            //timeInEpoch.push_back(QDateTime::fromString(time[i].c_str(), Qt::ISODate).toSecsSinceEpoch());


            if((value[i] == 0) && (i != n - 1))
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

    stockData(value[n-1], value[n-2]);

    //Assigns data to graph
    ui->stockGraph->graph(0)->setData(timeInEpoch, value);

    //Set x axis range
    ui->stockGraph->xAxis->setRange(timeInEpoch[0], timeInEpoch[n-1]);

    //Set y axis range
    ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
    ui->stockGraph->replot();
}

//Fix x axis dates and tickers
void MainWindow::h_oneMonth_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1mm");
    //Stores x and y values
    QVector<double> value, timeInEpoch;
    QVector<string>time(300);
    setupLineChart("MMM d", 5);

    int  n = chartData.size();

    //Finds max and min for range
    float maxAvg = chartData[0]["close"].asDouble();
    float minAvg = chartData[0]["close"].asDouble();

    //Reads in data from json(historical data 1 day delayed)
    for(Json::Value::ArrayIndex i = 0 ; i < n; i++)
    {
            value.push_back(chartData[i]["close"].asDouble());
            time[i] = chartData[i]["date"].asString();
            timeInEpoch.push_back(QDateTime::fromString(time[i].c_str(), Qt::ISODate).toSecsSinceEpoch());
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

    stockData(value[n-1], value[n-2]);

    //Assigns data to graph
    ui->stockGraph->graph(0)->setData(timeInEpoch, value);

    //Set x axis range
    ui->stockGraph->xAxis->setRange(timeInEpoch[0], timeInEpoch[n-1]);

    //Set y axis range
    ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
    ui->stockGraph->replot();
}

//Fix x axis dates and tickers
void MainWindow::h_sixMonth_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "6m");
    //Stores x and y values
    QVector<double> value, timeInEpoch;
    QVector<string>time(183);
    setupLineChart("MMM d", 8);

    int  n = chartData.size();

    //Finds max and min for range
    float maxAvg = chartData[0]["close"].asDouble();
    float minAvg = chartData[0]["close"].asDouble();

    //Reads in data from json(historical data 1 day delayed)
    for(Json::Value::ArrayIndex i = 0 ; i < n; i++)
    {
            value.push_back(chartData[i]["close"].asDouble());
            time[i] = chartData[i]["date"].asString();
            timeInEpoch.push_back(QDateTime::fromString(time[i].c_str(), Qt::ISODate).toSecsSinceEpoch());
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

    stockData(value[n-1], value[n-2]);

    //Assigns data to graph
    ui->stockGraph->graph(0)->setData(timeInEpoch, value);

    //Set x axis range
    ui->stockGraph->xAxis->setRange(timeInEpoch[0], timeInEpoch[n-1]);

    //Set y axis range
    ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
    ui->stockGraph->replot();
}

//Fix x axis dates and tickers
void MainWindow::h_oneYear_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1y");
    //Stores x and y values
    QVector<double> value, timeInEpoch;
    QVector<string>time(365);
    setupLineChart("MMM d", 8);

    int  n = chartData.size();

    //Finds max and min for range
    float maxAvg = chartData[0]["close"].asDouble();
    float minAvg = chartData[0]["close"].asDouble();

    //Reads in data from json(historical data 1 day delayed)
    for(Json::Value::ArrayIndex i = 0 ; i < n; i++)
    {
            value.push_back(chartData[i]["close"].asDouble());
            time[i] = chartData[i]["date"].asString();
            timeInEpoch.push_back(QDateTime::fromString(time[i].c_str(), Qt::ISODate).toSecsSinceEpoch());
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

    stockData(value[n-1], value[n-2]);

    //Assigns data to graph
    ui->stockGraph->graph(0)->setData(timeInEpoch, value);

    //Set x axis range
    ui->stockGraph->xAxis->setRange(timeInEpoch[0], timeInEpoch[n-1]);

    //Set y axis range
    ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
    ui->stockGraph->replot();
}

//Completed
void MainWindow::h_ytd_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartYtd(symbolSearchedStd);

    //Stores x and y values
    QVector<double> value(365), timeInEpoch(365);
    QVector<string>time(365);
    setupLineChart("MMM d, \nyyyy", 8);

    int  n = chartData.size();

    //Finds max and min for range
    float maxAvg = chartData[0]["close"].asDouble();
    float minAvg = chartData[0]["close"].asDouble();

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



            //CANDLESTICK CHARTS
            //Real Time Charting
void MainWindow::rt_oneDay_Candle_Graph()
{

}

void MainWindow::rt_oneWeek_Candle_Graph()
{

}

void MainWindow::rt_oneMonth_Candle_Graph()
{

}

void MainWindow::rt_sixMonth_Candle_Graph()
{

}

void MainWindow::rt_oneYear_Candle_Graph()
{

}




            //Historical Charting

void MainWindow::h_oneDay_Candle_Graph()
{

}

//Needs a lot of work still
void MainWindow::h_oneWeek_Candle_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "5dm");

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
    dateTicker->setTickCount(6);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);


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
    ui->stockGraph->xAxis->setRange(timeInEpoch[0] - 200000, timeInEpoch[n-1] + 400000);

    //Set y axis range
    ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
    ui->stockGraph->replot();
}

//Needs a lot of work still
void MainWindow::h_oneMonth_Candle_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1mm");

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
    dateTicker->setTickCount(6);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);


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
    ui->stockGraph->xAxis->setRange(timeInEpoch[0] - 200000, timeInEpoch[n-1] + 400000);

    //Set y axis range
    ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
    ui->stockGraph->replot();
}

//Working but not lookng to par
void MainWindow::h_sixMonth_Candle_Graph()
{

        //Retrieves json format of data
        Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "6m");

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
        dateTicker->setTickCount(6);
        dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
        ui->stockGraph->xAxis->setTicker(dateTicker);
        ui->stockGraph->xAxis->setSubTicks(false);
        ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
        ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
        ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
        ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
        ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
        ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
        ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);


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
        ui->stockGraph->xAxis->setRange(timeInEpoch[0] - 200000, timeInEpoch[n-1] + 400000);

        //Set y axis range
        ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
        ui->stockGraph->replot();

}

//Working but not lookng to par
void MainWindow::h_oneYear_Candle_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1y");

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
    valueTicker->setTickCount(8);
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
    dateTicker->setTickCount(10);
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
    ui->stockGraph->xAxis->setRange(timeInEpoch[0] - 160000, timeInEpoch[n-1] + 300000);

    //Set y axis range
    QCPRange yAxis(minAvg + 2, maxAvg + 2);
    yAxis.normalize();
    yAxis.center();
    ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
    ui->stockGraph->replot();
}

//Completed
void MainWindow::h_ytd_Candle_Graph()
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
























