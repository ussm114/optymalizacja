#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simplex.h"

using namespace std;

std::vector<QLineEdit *> simplexArr;
std::vector<QDoubleSpinBox *> restrVals;

bool isValid(std::vector<QLineEdit *> simplexArr)
{
    for(QLineEdit *varSimplex : simplexArr)
        if (varSimplex->text() == "")
            return false;
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
        ui->gridLayout->addWidget(new QLabel(QString("x")), 0, 1);
        ui->gridLayout->addWidget(new QLabel(QString("p1")), 1, 0);
        ui->gridLayout->addWidget(new QLabel(QString("p1")), 2, 0);
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            label = new QLabel(QString::fromStdString("x"+std::to_string(i+1)));
            label->setMaximumHeight(20);
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
    //this->setFixedSize(500,500);
    manageSimplex(2, false, ui);
    manageRestrictions(2, ui);
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
        vector<vector<double> > simplex;
        int N = ui->spinBox->value();

        if (!isValid(simplexArr))
            ui->statusBar->showMessage("Invalid simplex provided");
        else
            for (int i = 0; i < N+1; i++)
            {
                for (int j = 0; j < N; j++)
                    point.push_back(simplexArr[N*i+j]->text().toDouble());
                simplex.push_back(point);
                point.clear();
            }

        result = new resultswindow(this);
        result->show();
        result->calculate(ui->comboBox->currentText().toStdString() ,ui->spinBox->value());

        //printcon( Simplex(ui->comboBox->currentText().toStdString(), ui->spinBox->value()));
    }
}

void MainWindow::on_spinBox_valueChanged(int value)
{
    manageSimplex(value, ui->randBox->isChecked(), ui);
    manageRestrictions(value, ui);
}

void MainWindow::on_randBox_stateChanged()
{
    manageSimplex(ui->spinBox->value(), ui->randBox->isChecked(), ui);
}

MainWindow::~MainWindow()
{
    delete ui;
}
