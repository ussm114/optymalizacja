#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simplex.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <iterator>
#include <cmath>

using namespace std;

template<class Con>
void printcon(const Con& c){
  std::cout.precision(12);
  copy( c.begin(),
    c.end(),
    ostream_iterator<typename Con::value_type>(cout, "  ") );
  cout<<endl;
}

double rosenbrock(vector<double> x){
  return 100*pow(x[1]-pow(x[0],2),2)+pow(1-x[0],2);
}

double polynomial_fun(vector<double> x){
  return pow( pow(x[0],2) + pow(x[1],2), 2 ) - pow(x[0]-3*x[1],2);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //initial guessing value for Rosenbrock function
    vector<double> init;
    init.push_back(1.23);
    init.push_back(10.96);

    //initial trial simplex
    double a0[]={-1.5, -1}, a1[]={-2, -2}, a2[]={ 2.5, 1.5};
    vector<vector<double> > simplex;
    simplex.push_back( vector<double>(a0,a0+2) );
    simplex.push_back( vector<double>(a1,a1+2) );
    simplex.push_back( vector<double>(a2,a2+2) );

    //optimizating...
    //printcon is a function printing container values

    if (ui->radioButton->isChecked())
        switch(ui->comboBox->currentIndex())
        {
        case 0:
            cout<<"Rosenbrock function achieves minimum at:"<<endl;
            printcon( Simplex(rosenbrock, 2)); //, simplex, 1E5) ); cout<<endl;
            break;
        case 1:
            cout<<"Polynomial function achieves minimum at:"<<endl;
            printcon( Simplex(polynomial_fun, 2, simplex, 1e-7) );  cout<<endl;
            break;
        default:
            cout<< "defaultowo " << ui->comboBox->currentIndex();
            break;
        }
}
