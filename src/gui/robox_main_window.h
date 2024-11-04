#ifndef ROBOXMAINWINDOW_H
#define ROBOXMAINWINDOW_H

#include <QMainWindow>

class RoboxMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    RoboxMainWindow(QWidget *parent = nullptr);
    ~RoboxMainWindow();
};
#endif // ROBOXMAINWINDOW_H
