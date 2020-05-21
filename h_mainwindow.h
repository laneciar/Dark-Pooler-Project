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
    void assignStatistics();
    void clearChart();
    void stockData(double num1, double num2);

    //Line Graphs
    void ytd_Line_Graph();
    void oneMin_Line_Graph();
    void fiveMin_Line_Graph();
    void fifteenMin_Line_Graph();
    void oneHour_Line_Graph();
    void fiveHour_Line_Graph();
    void oneDay_Line_Graph();
    void oneMonth_Line_Graph();

    //Candlestick Graphs
    void ytd_Candle_Graph();
    void oneMin_Candle_Graph();
    void fiveMin_Candle_Graph();
    void fifteenMin_Candle_Graph();
    void oneHour_Candle_Graph();
    void fiveHour_Candle_Graph();
    void oneDay_Candle_Graph();
    void oneMonth_Candle_Graph();





public slots:
    void readData();

signals:
    void processingDone();

private slots:

    void on_maximizeButton_clicked();

    void on_minimizeButton_clicked();

    void on_exitButton_clicked();

    void currentTime();

    void on_symbolSearch_returnPressed();

    void on_chartStlyeBox_currentIndexChanged(int index);



private:
    Ui::MainWindow *ui;
    QSizePolicy QSizePolicy;
    QString symbolSearched;
    QPoint startPos;
    QTimer *timer, dataTimer;
    QCPFinancial *candlesticks;
    string pageAddress;
    string symbolSearchedStd;
    bool isMaximized = false;
    bool isMousePressed;
    int counter = 0;
    bool isLineChart = true, isCandlestickChart = false;



};




#endif // MAINWINDOW_H
