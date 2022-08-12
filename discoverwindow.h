#ifndef DISCOVERWINDOW_H
#define DISCOVERWINDOW_H

#include <QMainWindow>

namespace Ui {
class DiscoverWindow;
}

class DiscoverWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DiscoverWindow(QWidget *parent = nullptr);
    ~DiscoverWindow();

private:
    Ui::DiscoverWindow *ui;
};

#endif // DISCOVERWINDOW_H
