#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>
#include <QDialog>
#include <QMessageBox>
#include <QStackedWidget>
#include<QDateTime>
#include <QDate>
#include <QTimer>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <QMouseEvent>
#include "ui_u_mainwindow.h"
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
public slots:

signals:
    void dataRead();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void assignStatistics(QString symbol);
    void ytdGraphLine(QString symbol);

private slots:

    void on_maximizeButton_clicked();

    void on_minimizeButton_clicked();

    void on_exitButton_clicked();

    void on_symbolSearch_returnPressed();

    void currentTime();

    void on_lineChartButton_clicked();

private:
    Ui::MainWindow *ui;

    QSizePolicy QSizePolicy;
    string pageAddress;
    QString symbolSearched = "AAPL";
    string symbolSearchedStd;
    bool isMaximized = false;
    int mouseClick_X;
    int mouseClick_Y;

    //Timer for clock
    QTimer *timer;




};




#endif // MAINWINDOW_H
