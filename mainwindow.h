#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "common/common.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // 显示主窗口
    void showMainWindow();

private:
    Ui::MainWindow *ui;
    Common m_common;
};

#endif // MAINWINDOW_H
