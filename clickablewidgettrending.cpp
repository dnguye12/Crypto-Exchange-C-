#include "clickablewidgettrending.h"

void ClickableWidgetTrending::mousePressEvent(QMouseEvent* event)
{
    emit clicked(event);
}
