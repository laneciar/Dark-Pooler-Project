#ifndef MainWindow_H
#define MainWindow_H

#include "ui_u_mainwindow.h"
#include "h_stockchart.h"
#include "curl/curl.h"
#include "h_webcurl.h"
#include "h_webscrapper.h"
#include "h_iexdata.h"

#include <QMessageBox>
#include <string.h>
#include <QMainWindow>
#include <QDialog>
#include <QMessageBox>
#include <QStackedWidget>
#include <QDateTime>
#include <QDate>
#include <QTimer>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <QMouseEvent>
#include <QtCharts/QtCharts>
#include <QJsonDocument>
#include <cmath>

using namespace  std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void setup();
    void setupLineChart(QString timeFormat, float tickCount);
    void assignStatistics();
    void clearChart();
    void stockData(double num1, double num2);

    //Line Graphs

    void rt_oneDay_Line_Graph();
    void rt_oneWeek_Line_Graph();
    void rt_oneMonth_Line_Graph();
    void rt_sixMonth_Line_Graph();
    void rt_oneYear_Line_Graph();

    void h_oneDay_Line_Graph();
    void h_oneWeek_Line_Graph();
    void h_oneMonth_Line_Graph();
    void h_sixMonth_Line_Graph();
    void h_oneYear_Line_Graph();
    void h_ytd_Line_Graph();


    //Candlestick Graphs
    void rt_oneDay_Candle_Graph();
    void rt_oneWeek_Candle_Graph();
    void rt_oneMonth_Candle_Graph();
    void rt_sixMonth_Candle_Graph();
    void rt_oneYear_Candle_Graph();


    void h_oneDay_Candle_Graph();
    void h_oneWeek_Candle_Graph();
    void h_oneMonth_Candle_Graph();
    void h_sixMonth_Candle_Graph();
    void h_oneYear_Candle_Graph();
    void h_ytd_Candle_Graph();






public slots:
    void readData();
    void refresh();

signals:
    void chartChange();

private slots:

    void on_maximizeButton_clicked();

    void on_minimizeButton_clicked();

    void on_exitButton_clicked();

    void currentTime();

    void on_symbolSearch_returnPressed();

    void on_chartStlyeBox_currentIndexChanged(int index);

    void on_realTimeBox_stateChanged(int arg1);

    void on_timeFrame_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QSizePolicy QSizePolicy;
    QString symbolSearched;
    QPoint startPos;
    QTimer *timer, dataTimer;
    QCPFinancial *candlesticks;
    QString currentChartRange;
    string pageAddress;
    string symbolSearchedStd;
    int counter = 0;
    bool isMaximized = false;
    bool isMousePressed;
    bool isLineChart, isCandlestickChart, isRealTime;



};




#endif // MAINWINDOW_H
