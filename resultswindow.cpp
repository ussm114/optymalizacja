#include "resultswindow.h"
#include "ui_resultswindow.h"

resultswindow::resultswindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::resultswindow)
{
    ui->setupUi(this);
}

resultswindow::~resultswindow()
{
    delete ui;
}

void resultswindow::calculate(std::string function, int n)
{
    this->function = function;
    this->n = n;
}
