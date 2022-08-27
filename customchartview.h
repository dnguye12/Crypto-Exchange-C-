#ifndef CUSTOMCHARTVIEW_H
#define CUSTOMCHARTVIEW_H

#include <QChartView>

class CustomChartView : public QChartView
{
    Q_OBJECT
public:
    using QChartView::QChartView;

    void mouseMoveEvent(QMouseEvent *event);
};

#endif // CUSTOMCHARTVIEW_H
