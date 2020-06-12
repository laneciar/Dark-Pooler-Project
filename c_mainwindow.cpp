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

    isMousePressed = false;
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
void MainWindow::setupLineChart()
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
    QVector<double> value, timeInEpoch;
    QVector<string> time, minute;

    //Reads in data from json(historical data 1 day delayed)
    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {

        value.push_back(chartData[i]["average"].asDouble());
        if((value[i] == 0) && (i != chartData.size() - 1))
        {
            value[i] = value[i-1];
        }

        time.push_back(chartData[i]["date"].asString());
        auto timeDate = QDate::fromString(time[i].c_str(), Qt::ISODate);

        minute.push_back(chartData[i]["minute"].asString());
        auto minuteDate = QTime::fromString(minute[i].c_str(), "hh:mm");

        timeInEpoch.push_back(QDateTime(timeDate, minuteDate, Qt::OffsetFromUTC).toSecsSinceEpoch());

    }

    //Adds data from the day
    ui->stockGraph->graph(0)->setData(timeInEpoch, value);

    //Sets temporary range
    ui->stockGraph->xAxis->setRange(timeInEpoch[0], timeInEpoch[chartData.size()-1]  + 500);
    ui->stockGraph->yAxis2->setRange(value[value.size()-1], 5, Qt::AlignCenter);

    ui->stockGraph->replot();
}



            //   LINE CHARTS
            //Real Time Charting(Incomplete)

void MainWindow::rt_oneDay_Line_Graph()
{
    setupLineChart();
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

//Needs UI Work
void MainWindow::h_oneDay_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1d");

    setupLineChart();

    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(13);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);

    QCPDataContainer<QCPGraphData> datas;
    QVector<string> minute;

    //Reads in data from json(historical data 1 day delayed)
    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {
        if(chartData[i]["average"].asDouble() == 0 && i != chartData.size()-1)
        {
            chartData[i]["average"] = chartData[i-1]["average"];
        }

        QCPGraphData data;
        data.key = i;
        data.value = chartData[i]["average"].asDouble();
        datas.add(data);

        minute.push_back(chartData[i]["minute"].asString());

        dateTicker->addTick(i, QString::fromStdString(minute[i]));



    }

    dateTicker->addTick(chartData.size(), "16:00");

    stockData(chartData[chartData.size()-1]["average"].asDouble(), chartData[chartData.size()-2]["average"].asDouble());

    //Assigns data to graph
    ui->stockGraph->graph(0)->data()->set(datas);

    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();

    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());

    ui->stockGraph->replot();
}

//Needs UI Work
void MainWindow::h_oneWeek_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "5dm");

    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(5);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);

    QCPDataContainer<QCPGraphData> datas;

    //Stores x and y values
    QVector<string>time;
    setupLineChart();

    //Reads in data from json(historical data 1 day delayed)
    for(int i = 0 ; i < chartData.size(); i++)
    {

        if(chartData[i]["average"].asDouble() == 0 && i != chartData.size()-1)
        {
            chartData[i]["average"] = chartData[i-1]["average"];
        }

        QCPGraphData data;
        data.key = i;
        data.value = chartData[i]["average"].asDouble();
        datas.add(data);

        time.push_back(chartData[i]["date"].asString());

        dateTicker->addTick(i, QString::fromStdString(time[i]));



    }

    stockData(chartData[chartData.size()-1]["average"].asDouble(), chartData[chartData.size()-2]["average"].asDouble());

    //Assigns data to graph
    ui->stockGraph->graph(0)->data()->set(datas);

    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();

    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());

    ui->stockGraph->replot();
}

//Needs UI Work
void MainWindow::h_oneMonth_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1mm");

    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(8);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);

    QCPDataContainer<QCPGraphData> datas;

    //Stores x and y values
    QVector<string>time;
    setupLineChart();

    //Reads in data from json(historical data 1 day delayed)
    for(int i = 0 ; i < chartData.size(); i++)
    {

        if(chartData[i]["average"].asDouble() == 0 && i != chartData.size()-1)
        {
            chartData[i]["average"] = chartData[i-1]["average"];
        }

        QCPGraphData data;
        data.key = i;
        data.value = chartData[i]["average"].asDouble();
        datas.add(data);

        time.push_back(chartData[i]["date"].asString());

        dateTicker->addTick(i, QString::fromStdString(time[i]));



    }

    stockData(chartData[chartData.size()-1]["average"].asDouble(), chartData[chartData.size()-2]["average"].asDouble());

    //Assigns data to graph
    ui->stockGraph->graph(0)->data()->set(datas);

    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();

    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());

    ui->stockGraph->replot();
    ui->stockGraph->replot();
}

//Needs UI Work
void MainWindow::h_sixMonth_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "6m");

    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(6);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);

    QCPDataContainer<QCPGraphData> datas;

    //Stores x and y values
    QVector<string>time;
    setupLineChart();

    //Reads in data from json(historical data 1 day delayed)
    for(int i = 0 ; i < chartData.size(); i++)
    {

        if(chartData[i]["close"].asDouble() == 0 && i != chartData.size()-1)
        {
            chartData[i]["close"] = chartData[i-1]["close"];
        }

        QCPGraphData data;
        data.key = i;
        data.value = chartData[i]["close"].asDouble();
        datas.add(data);

        time.push_back(chartData[i]["date"].asString());

        dateTicker->addTick(i, QString::fromStdString(time[i]));



    }

    stockData(chartData[chartData.size()-1]["close"].asDouble(), chartData[chartData.size()-2]["close"].asDouble());

    //Assigns data to graph
    ui->stockGraph->graph(0)->data()->set(datas);

    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();

    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());

    ui->stockGraph->replot();
}

//Needs UI Work
void MainWindow::h_oneYear_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1y");
    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(12);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);

    QCPDataContainer<QCPGraphData> datas;

    //Stores x and y values
    QVector<string>time;
    setupLineChart();

    //Reads in data from json(historical data 1 day delayed)
    for(int i = 0 ; i < chartData.size(); i++)
    {

        if(chartData[i]["close"].asDouble() == 0 && i != chartData.size()-1)
        {
            chartData[i]["close"] = chartData[i-1]["close"];
        }

        QCPGraphData data;
        data.key = i;
        data.value = chartData[i]["close"].asDouble();
        datas.add(data);

        time.push_back(chartData[i]["date"].asString());

        dateTicker->addTick(i, QString::fromStdString(time[i]));



    }

    stockData(chartData[chartData.size()-1]["close"].asDouble(), chartData[chartData.size()-2]["close"].asDouble());

    //Assigns data to graph
    ui->stockGraph->graph(0)->data()->set(datas);

    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();

    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());
    ui->stockGraph->replot();
}

//Needs UI Work
void MainWindow::h_ytd_Line_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartYtd(symbolSearchedStd);

    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(6);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);

    QCPDataContainer<QCPGraphData> datas;

    //Stores x and y values
    QVector<string>time;
    setupLineChart();

    //Reads in data from json(historical data 1 day delayed)
    for(int i = 0 ; i < chartData.size(); i++)
    {

        if(chartData[i]["close"].asDouble() == 0 && i != chartData.size()-1)
        {
            chartData[i]["close"] = chartData[i-1]["close"];
        }

        QCPGraphData data;
        data.key = i;
        data.value = chartData[i]["close"].asDouble();
        datas.add(data);

        time.push_back(chartData[i]["date"].asString());

        dateTicker->addTick(i, QString::fromStdString(time[i]));



    }

    stockData(chartData[chartData.size()-1]["close"].asDouble(), chartData[chartData.size()-2]["close"].asDouble());

    //Assigns data to graph
    ui->stockGraph->graph(0)->data()->set(datas);

    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();

    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());
    ui->stockGraph->replot();


}



            //CANDLESTICK CHARTS
            //Real Time Charting(Incomplete)
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
//Needs work
void MainWindow::h_oneDay_Candle_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1d");

    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(13);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    QCPDataContainer<QCPFinancialData> datas;

    //Stores x and y values
    QVector<double> timeDatas;
    QVector<string>time(chartData.size());

    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {
        timeDatas.append(i);
        QCPFinancialData data;
        data.key = i;
        data.open = chartData[i]["open"].asDouble();
        data.close = chartData[i]["close"].asDouble();
        data.low = chartData[i]["low"].asDouble();
        data.high = chartData[i]["high"].asDouble();
        datas.add(data);
        time[i] = chartData[i]["minute"].asString();


        dateTicker->addTick(i, QString::fromStdString(time[i]));
    }
    cout << "test" << endl;

    //stockData(value[n-1], value[n-2]);

    // create candlestick chart:
    candlesticks = new QCPFinancial(ui->stockGraph->xAxis, ui->stockGraph->yAxis2);
    candlesticks->setName("Candlestick");
    candlesticks->data()->set(datas);
    //candlesticks->setWidth(binSize*0.9);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(0,128,0));
    candlesticks->setBrushNegative(QColor(255,0,0));
    candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
    candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));

    //Creates X Axis on bottom
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());

    //Creates Y Axis on right side
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
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());

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
    ui->stockGraph->replot();
}

//Needs UI Work
void MainWindow::h_oneWeek_Candle_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "5dm");

    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(4);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    QCPDataContainer<QCPFinancialData> datas;

    //Stores x and y values
    QVector<double> timeDatas;
    QVector<string>time(365);

    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {
        timeDatas.append(i);

        QCPFinancialData data;
        data.key = i;
        data.open = chartData[i]["open"].asDouble();
        data.close = chartData[i]["close"].asDouble();
        data.low = chartData[i]["low"].asDouble();
        data.high = chartData[i]["high"].asDouble();
        datas.add(data);
        time[i] = chartData[i]["date"].asString();


        dateTicker->addTick(i, QString::fromStdString(time[i]));
    }
    //stockData(value[n-1], value[n-2]);

    // create candlestick chart:
    candlesticks = new QCPFinancial(ui->stockGraph->xAxis, ui->stockGraph->yAxis2);
    candlesticks->setName("Candlestick");
    candlesticks->data()->set(datas);
    //candlesticks->setWidth(binSize*0.9);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(0,128,0));
    candlesticks->setBrushNegative(QColor(255,0,0));
    candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
    candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));

    //Creates X Axis on bottom
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());

    //Creates Y Axis on right side
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
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());

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
    ui->stockGraph->replot();

}

//Needs UI Work
void MainWindow::h_oneMonth_Candle_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1mm");

    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(9);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    QCPDataContainer<QCPFinancialData> datas;

    //Stores x and y values
    QVector<double> timeDatas;
    QVector<string>time(365);

    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {
        timeDatas.append(i);

        QCPFinancialData data;
        data.key = i;
        data.open = chartData[i]["open"].asDouble();
        data.close = chartData[i]["close"].asDouble();
        data.low = chartData[i]["low"].asDouble();
        data.high = chartData[i]["high"].asDouble();
        datas.add(data);
        time[i] = chartData[i]["date"].asString();


        dateTicker->addTick(i, QString::fromStdString(time[i]));
    }
    //stockData(value[n-1], value[n-2]);

    // create candlestick chart:
    candlesticks = new QCPFinancial(ui->stockGraph->xAxis, ui->stockGraph->yAxis2);
    candlesticks->setName("Candlestick");
    candlesticks->data()->set(datas);
    //candlesticks->setWidth(binSize*0.9);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(0,128,0));
    candlesticks->setBrushNegative(QColor(255,0,0));
    candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
    candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));

    //Creates X Axis on bottom
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());

    //Creates Y Axis on right side
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
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());

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
    ui->stockGraph->replot();
}

//Needs UI Work
void MainWindow::h_sixMonth_Candle_Graph()
{

        //Retrieves json format of data
        Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "6m");

        QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
        dateTicker->setTickCount(6);
        dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
        QCPDataContainer<QCPFinancialData> datas;

        //Stores x and y values
        QVector<double> timeDatas;
        QVector<string>time(365);

        for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
        {
            timeDatas.append(i);

            QCPFinancialData data;
            data.key = i;
            data.open = chartData[i]["open"].asDouble();
            data.close = chartData[i]["close"].asDouble();
            data.low = chartData[i]["low"].asDouble();
            data.high = chartData[i]["high"].asDouble();
            datas.add(data);
            time[i] = chartData[i]["date"].asString();


            dateTicker->addTick(i, QString::fromStdString(time[i]));
        }
        //stockData(value[n-1], value[n-2]);

        // create candlestick chart:
        candlesticks = new QCPFinancial(ui->stockGraph->xAxis, ui->stockGraph->yAxis2);
        candlesticks->setName("Candlestick");
        candlesticks->data()->set(datas);
        //candlesticks->setWidth(binSize*0.9);
        candlesticks->setTwoColored(true);
        candlesticks->setBrushPositive(QColor(0,128,0));
        candlesticks->setBrushNegative(QColor(255,0,0));
        candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
        candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));

        //Creates X Axis on bottom
        ui->stockGraph->xAxis->setTicker(dateTicker);
        ui->stockGraph->rescaleAxes();
        ui->stockGraph->xAxis->setSubTicks(false);
        ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
        ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
        ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
        ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
        ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
        ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
        ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);
        ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());

        //Creates Y Axis on right side
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
        ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());

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
        ui->stockGraph->replot();

}

//Needs UI Work
void MainWindow::h_oneYear_Candle_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartRange(symbolSearchedStd, "1y");

    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(12);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    QCPDataContainer<QCPFinancialData> datas;

    //Stores x and y values
    QVector<double> timeDatas;
    QVector<string>time(365);

    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {
        timeDatas.append(i);

        QCPFinancialData data;
        data.key = i;
        data.open = chartData[i]["open"].asDouble();
        data.close = chartData[i]["close"].asDouble();
        data.low = chartData[i]["low"].asDouble();
        data.high = chartData[i]["high"].asDouble();
        datas.add(data);
        time[i] = chartData[i]["date"].asString();


        dateTicker->addTick(i, QString::fromStdString(time[i]));
    }
    //stockData(value[n-1], value[n-2]);

    // create candlestick chart:
    candlesticks = new QCPFinancial(ui->stockGraph->xAxis, ui->stockGraph->yAxis2);
    candlesticks->setName("Candlestick");
    candlesticks->data()->set(datas);
    //candlesticks->setWidth(binSize*0.9);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(0,128,0));
    candlesticks->setBrushNegative(QColor(255,0,0));
    candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
    candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));

    //Creates X Axis on bottom
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());

    //Creates Y Axis on right side
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
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());

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
    ui->stockGraph->replot();
}

//Needs UI Work
void MainWindow::h_ytd_Candle_Graph()
{
    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartYtd(symbolSearchedStd);

    QSharedPointer<QCPAxisTickerText> dateTicker(new MyAxisTickerText);
    dateTicker->setTickCount(6);
    dateTicker->setTickStepStrategy(QCPAxisTicker::tssMeetTickCount);
    QCPDataContainer<QCPFinancialData> datas;

    //Stores x and y values
    QVector<double> timeDatas;
    QVector<string>time(365);

    for(Json::Value::ArrayIndex i = 0 ; i != chartData.size(); i++)
    {
        timeDatas.append(i);

        QCPFinancialData data;
        data.key = i;
        data.open = chartData[i]["open"].asDouble();
        data.close = chartData[i]["close"].asDouble();
        data.low = chartData[i]["low"].asDouble();
        data.high = chartData[i]["high"].asDouble();
        datas.add(data);
        time[i] = chartData[i]["date"].asString();


        dateTicker->addTick(i, QString::fromStdString(time[i]));
    }
    //stockData(value[n-1], value[n-2]);

    // create candlestick chart:
    candlesticks = new QCPFinancial(ui->stockGraph->xAxis, ui->stockGraph->yAxis2);
    candlesticks->setName("Candlestick");
    candlesticks->data()->set(datas);
    //candlesticks->setWidth(binSize*0.9);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(0,128,0));
    candlesticks->setBrushNegative(QColor(255,0,0));
    candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
    candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));

    //Creates X Axis on bottom
    ui->stockGraph->xAxis->setTicker(dateTicker);
    ui->stockGraph->rescaleAxes();
    ui->stockGraph->xAxis->setSubTicks(false);
    ui->stockGraph->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->stockGraph->xAxis->setTickLabelColor(Qt::white);
    ui->stockGraph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->stockGraph->xAxis->grid()->setSubGridVisible(true);
    ui->stockGraph->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->stockGraph->xAxis->scaleRange(1.03, ui->stockGraph->xAxis->range().center());

    //Creates Y Axis on right side
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
    ui->stockGraph->yAxis2->scaleRange(1.05, ui->stockGraph->yAxis2->range().center());

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
    ui->stockGraph->replot();

}
























