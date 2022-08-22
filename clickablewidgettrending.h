#ifndef CLICKABLEWIDGETTRENDING_H
#define CLICKABLEWIDGETTRENDING_H

#include <QWidget>

class ClickableWidgetTrending : public QWidget
{
    Q_OBJECT
signals:
        void clicked(QMouseEvent* event);
public:
    using QWidget::QWidget;

    void mousePressEvent(QMouseEvent* event);
};

#endif // CLICKABLEWIDGETTRENDING_H
