#ifndef RESULTSWINDOW_H
#define RESULTSWINDOW_H

#include <QDialog>

namespace Ui {
class resultswindow;
}

class resultswindow : public QDialog
{
    Q_OBJECT

public:
    explicit resultswindow(QWidget *parent = 0);
    ~resultswindow();
    int calculate(std::string function, int n, std::vector <std::vector <double>> simplex, double tol, int iter, std::vector<double> restrVals, double a, double b, double g, double h);

private:
    Ui::resultswindow *ui;
    std::string function;
    int n;
    std::vector <std::vector <double>> simplex;
    double tol;
    int iter;
};

#endif // RESULTSWINDOW_H
