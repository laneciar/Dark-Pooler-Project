#ifndef STOCKCHART_H
#define STOCKCHART_H

#include <QWidget>
#include <h_qcustomplot.h>

class StockChart : public QWidget
{
    Q_OBJECT
public:
    explicit StockChart(QWidget *parent = nullptr);


};

#endif // STOCKCHART_H
