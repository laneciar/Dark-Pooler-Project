#include "h_mainwindow.h"

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

    ui->stockName->setText("");
    ui->stockPrice->setText("");
    ui->stockPriceChange->setText("");



}

MainWindow::~MainWindow()
{
    delete ui;
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
        ytd_Line_Graph();
    }


}

//Current Clock Time
void MainWindow::currentTime()
{
    QTime time = QTime::currentTime();
    QString time_text = time.toString("hh : mm : ss ap");
    ui->timeClock->setText(time_text);
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
    ui->fiscalEnd->setText(web.fiscalEnd);
    return;
}

//Shows year-to-date graph of specific company
void MainWindow::ytd_Line_Graph()
{

    QDateTime startYear = QDateTime(QDate(2020,1,1));
    startYear.setTimeSpec(Qt::UTC);
    double startYearTime = startYear.toTime_t();



    //Retrieves json format of data
    Json::Value chartData = IEX::stocks::chartYtd(symbolSearchedStd);

    //Stores x and y values
    QVector<double> time(365), closePrice(365);

    int  n = chartData.size();

    //Intialize first vector to first values
    closePrice[0] = chartData[0]["close"].asDouble();
    time[0] = startYearTime;

    //Finds max and min for range
    float maxAvg = closePrice[0];
    float minAvg = closePrice[0];

    //Reads in data from json(historical data 1 day delayed)
    for(Json::Value::ArrayIndex i = 1 ; i != chartData.size(); i++)
    {
        if(chartData[i].isMember("close"))
        {
            if((closePrice[i] == 0) && (time[i] != chartData.size() - 1))
            {
                closePrice[i] = closePrice[i-1];
            }

            closePrice[i] = (chartData[i]["close"].asDouble());
            time[i] = startYearTime + 86400*i;
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

    //Gives stock symbol and ending price
    ui->stockName->setText(symbolSearched);
    ui->stockPrice->setText('$' + QString::number(closePrice[n - 1]));

    double closePriceChange = closePrice[n - 1] - closePrice[n - 2];
    QString closePriceChangeStr = QString::number(closePriceChange);

    //Daily change of price is below 0
    if (closePriceChange < 0)
    {
        ui->stockPriceChange->setText("($"+closePriceChangeStr+')');
        ui->stockPriceChange->setStyleSheet("color:rgb(255, 0, 0)");
    }

    //Daily change of price is above 0.
    else if(closePriceChange > 0)
    {
        ui->stockPriceChange->setText("($"+closePriceChangeStr+')');
        ui->stockPriceChange->setStyleSheet("color:rgb(0, 170, 0)");
    }



    //Initializes graph
    ui->stockGraph->addGraph(ui->stockGraph->xAxis, ui->stockGraph->yAxis2);
    ui->stockGraph->graph(0)->setData(time, closePrice);

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
    dateTicker->setDateTimeFormat("MMM d, yy");
    dateTicker->setDateTimeSpec(Qt::UTC);
    dateTicker->setTickCount(6);
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
    ui->stockGraph->xAxis->setRange(startYearTime, time[n-1]);

    //Set y axis range
    QCPRange yAxis(minAvg + 2, maxAvg + 2);
    yAxis.normalize();
    yAxis.center();
    ui->stockGraph->yAxis2->setRange(minAvg - 10, maxAvg + 10);
    ui->stockGraph->replot();




}

