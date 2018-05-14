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

    for(int j1 = 0; j1 < punkty; j1++) {
        for(int j2 = 0; j2 < punkty; j2++) {
            if(j1 != j2) {
                int licznikTakichSamych = 0;
                for(int i = 0; i < wsp; i++) {
                    if(simplexArr[i + wsp*j1]->text().toStdString() == simplexArr[i + wsp*j2]->text().toStdString()) {
                        licznikTakichSamych++;
                    }
                    if (licznikTakichSamych == wsp) {
                        return false;
                    }
                }

            }
        }
    }

//    sekcja próbna, co prawda działająca
    //już niepotrzebna, te zagnieżdżone pętle działają dobrze
//    int n = simplexArr.size();
//    if (n == 6) {
//        string newArray[6];
//        for(int i = 0; i < 6; i++) {
//            newArray[i] = simplexArr[i]->text().toStdString();
//        }
//        if (newArray[0] == newArray[2] && newArray[1] == newArray[3]) { //1 i 2 są identyczne
//            return false;
//        }
//        else if (newArray[0] == newArray[4] && newArray[1] == newArray[5]) { //1 i 3 są identyczne
//            return false;
//        }
//        else if (newArray[1] == newArray[4] && newArray[3] == newArray[5]) { //2 i 3 są identyczne
//            return false;
//        }
//    }

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
        int n = ui->spinBox->value();
        //cout << n << endl;
        //cout << "a wiec jest n" << endl;

        QString tmp = ui->eps->text();


        tmp.remove(0, 2);
        double eps = pow(10, tmp.toInt());

        tmp = ui->iter->text();


        //cout << tmp.toStdString() << endl;

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

        /*Q3DSurface* surface = new Q3DSurface();
        surface->setFlags(surface->flags() ^ Qt::FramelessWindowHint);

        QSurfaceDataArray *data = new QSurfaceDataArray;
        QSurfaceDataRow *dataRow1 = new QSurfaceDataRow;
        QSurfaceDataRow *dataRow2 = new QSurfaceDataRow;

        *dataRow1 << QVector3D(0.0f, 0.1f, 0.5f) << QVector3D(1.0f, 0.5f, 0.5f);
        *dataRow2 << QVector3D(0.0f, 1.8f, 1.0f) << QVector3D(1.0f, 1.2f, 1.0f);
        *data << dataRow1 << dataRow2;

        QSurface3DSeries *series = new QSurface3DSeries;
        series->dataProxy()->resetArray(data);
        surface->addSeries(series);
        QCustom3DItem *item = new QCustom3DItem();
        item->setPosition(QVector3D(1,1,1));
        //surface->addCustomItem(item);
        surface->show();*/

        result = new resultswindow(this);
        result->show();

        std::vector<double> restrvals;
        for (int i = 0; i < restrVals.size(); i++)
            restrvals.push_back(restrVals[i]->value());
//        cout << simplexArr[0]->text().toStdString() << endl; //
//        cout << simplexArr[1]->text().toStdString() << endl; //
//        cout << simplexArr[2]->text().toStdString() << endl; //
//        cout << simplexArr[3]->text().toStdString() << endl; //
//        cout << simplexArr[4]->text().toStdString() << endl; //
//        cout << simplexArr[5]->text().toStdString() << endl; //
        result->calculate(ui->comboBox->currentText().toStdString(), n, simplex, eps, iter, restrvals);
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
