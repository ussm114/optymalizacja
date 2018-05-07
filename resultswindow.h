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
    void calculate(std::string function, int n);

private:
    Ui::resultswindow *ui;
    std::string function;
    int n;
};

#endif // RESULTSWINDOW_H
