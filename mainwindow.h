#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <QtDataVisualization>
#include "resultswindow.h"
#include "fparser.hh"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_spinBox_valueChanged(int arg1);
    void on_calculateButton_clicked();
    void on_randButton_clicked();

private:
    Ui::MainWindow *ui;
    resultswindow* result;
};

#endif // MAINWINDOW_H
