#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace QtDataVisualization;

std::vector<QLineEdit *> simplexArr;
std::vector<QDoubleSpinBox *> restrVals;

bool isValid(std::vector<QLineEdit *> simplexArr, int n)
{

    for(QLineEdit *varSimplex : simplexArr)
        if (varSimplex->text() == "")
            return false;

    int wsp = n;
    int punkty = n + 1;

    for(int j1 = 0; j1 < punkty; j1++)
        for(int j2 = 0; j2 < punkty; j2++)
            if(j1 != j2)
            {
                int licznikTakichSamych = 0;
                for(int i = 0; i < wsp; i++)
                {
                    if(simplexArr[i + wsp*j1]->text().toStdString() == simplexArr[i + wsp*j2]->text().toStdString())
                        licznikTakichSamych++;
                    if (licznikTakichSamych == wsp)
                        return false;
                }

            }

    return true;
}

void manageSimplex(int n, bool randSimplex, Ui::MainWindow *ui)
{
    QLayoutItem* item;
    QLineEdit* lineEdit;
    QLabel* label;
    double rnd, min, max;

    simplexArr.clear();

    while (( item = ui->gridLayout->takeAt(0)) != NULL )
        delete item->widget();

    if (n == 1)
    {
        label = new QLabel(QString::fromStdString("x"));
        label->setAlignment(Qt::AlignHCenter);
        ui->gridLayout->addWidget(label, 0, 1);
        ui->gridLayout->addWidget(new QLabel(QString("p1")), 1, 0);
        ui->gridLayout->addWidget(new QLabel(QString("p1")), 2, 0);
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            label = new QLabel(QString::fromStdString("x"+std::to_string(i+1)));
            label->setMaximumHeight(20);
            label->setAlignment(Qt::AlignHCenter);
            ui->gridLayout->addWidget(label, 0, i+1);
        }
        for (int i = 0; i < n+1; i++)
        {
            label = new QLabel(QString::fromStdString("p"+std::to_string(i+1)));
            label->setMinimumWidth(20);
            ui->gridLayout->addWidget(label, i+1, 0);
        }
    }
    for (int i = 0; i < n*(n+1); i++)
    {
        lineEdit = new QLineEdit();
        ui->gridLayout->addWidget(lineEdit, i/n+1, i%n+1);
        simplexArr.push_back(lineEdit);
    }
    if (randSimplex)
        for (int i = 0; i < n*(n+1); i++)
        {
            min = restrVals[(2*i)%(2*n)]->value();
            max = restrVals[(2*i+1)%(2*n)]->value();
            rnd = min + (double)rand() / RAND_MAX * (max - min);
            simplexArr[i]->setText(QString::number(rnd,'f',2));
        }
    else if (n==2)
    {
        simplexArr[0]->setText("-1");
        simplexArr[1]->setText("-3");
        simplexArr[2]->setText("4");
        simplexArr[3]->setText("-2");
        simplexArr[4]->setText("1");
        simplexArr[5]->setText("2");
    }
}

void manageRestrictions(int n, Ui::MainWindow *ui)
{
    QLayoutItem* item;
    QLayout* sublayout;
    QHBoxLayout* newsublayout;
    QDoubleSpinBox* spinBox;

    // clear all restrictions
    while (( item = ui->restrictions->takeAt(0)) != NULL )
    {
        sublayout = item->layout();
        while (( item = sublayout->takeAt(0)) != NULL )
            delete item->widget();
        delete sublayout;
    }

    restrVals.clear();

    for (int i = 0; i < n; i++)
    {
        newsublayout = new QHBoxLayout();

        spinBox = new QDoubleSpinBox();
        spinBox->setMinimum(-9999999999);
        spinBox->setValue(-5);
        restrVals.push_back(spinBox);
        newsublayout->addWidget(spinBox);

        newsublayout->addWidget(new QLabel(QString::fromStdString("<=   x"+to_string(i+1)+"   <=")), 0, Qt::AlignCenter);

        spinBox = new QDoubleSpinBox();
        spinBox->setMinimum(-9999999999);
        spinBox->setValue(5);
        restrVals.push_back(spinBox);
        newsublayout->addWidget(spinBox);

        ui->restrictions->addLayout(newsublayout);
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manageSimplex(2, false, ui);
    manageRestrictions(2, ui);
    this->setFixedSize(500, 550);
}

void MainWindow::on_calculateButton_clicked()
{
    FunctionParser fparser;
    vector<double> point;

    fparser.AddConstant("pi", 3.1415926535897932);
    fparser.Parse(ui->comboBox->currentText().toUtf8().constData(), "x,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10");
    ui->statusBar->showMessage(fparser.ErrorMsg());

    if (ui->statusBar->currentMessage()==NULL)
    {
        double a = ui->aSpinBox->value();
        double b = ui->bSpinBox->value();
        double g = ui->gSpinBox->value();
        double h = ui->hSpinBox->value();
        vector<vector<double> > simplex;
        int n = ui->spinBox->value();

        QString tmp = ui->eps->text();


        tmp.remove(0, 2);
        double eps = pow(10, tmp.toInt());

        tmp = ui->iter->text();

        tmp.remove(0, 2);
        int iter = pow(10, tmp.toInt());

        if (!isValid(simplexArr, n)) {
            ui->statusBar->showMessage("Invalid simplex provided");
            return;
        }
        else
            for (int i = 0; i < n+1; i++)
            {
                for (int j = 0; j < n; j++)
                    point.push_back(simplexArr[n*i+j]->text().toDouble());
                simplex.push_back(point);
                point.clear();
            }

        std::vector<double> restrvals;
        for (int i = 0; i < restrVals.size(); i++)
            restrvals.push_back(restrVals[i]->value());

        result = new resultswindow(this);
        if (n != 2)
            result->setFixedSize(500, 150);
        result->show();

        result->calculate(ui->comboBox->currentText().toStdString(), n, simplex, eps, iter, restrvals, a, b, g, h);
    }
}

void MainWindow::on_spinBox_valueChanged(int value)
{
    manageSimplex(value, 0, ui);
    manageRestrictions(value, ui);
    this->setFixedSize(500, 500 + value * 25);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_randButton_clicked()
{
    manageSimplex(ui->spinBox->value(), 1, ui);
}
