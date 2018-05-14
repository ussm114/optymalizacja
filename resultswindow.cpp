#include "resultswindow.h"
#include "ui_resultswindow.h"
#include "simplex.h"
#include <QtDataVisualization>
#include "qcustomplot.h"

using namespace QtDataVisualization;


double map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void plot(std::vector <std::vector <double>> simplex, QCustomPlot* customPlot)
{
    QCPCurve *newCurve = new QCPCurve(customPlot->xAxis, customPlot->yAxis);

    QVector <double> x;
    QVector <double> y;

    for (int i = 0; i < 3; i++)
    {
        x.push_back(simplex[i][0]);
        y.push_back(simplex[i][1]);
    }
    x.push_back(simplex[0][0]);
    y.push_back(simplex[0][1]);

    newCurve->setData(x, y);

    customPlot->rescaleAxes();
    customPlot->replot();
}

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

//a: reflection  -> xr
//b: expansion   -> xe
//g: contraction -> xc
//h: full contraction to x1

int resultswindow::calculate(std::string function, int n, std::vector <std::vector <double>> simplex, double eps, int iter, std::vector<double> restrVals, double a, double b, double g, double h)
{

    QStatusBar *statusBar = new QStatusBar;
    QCustomPlot *customPlot = new QCustomPlot;
    QSizePolicy *sizePolicy = new QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy->setVerticalStretch(10);

    customPlot->setSizePolicy(*sizePolicy);

    this->layout()->addWidget(statusBar);
    if (n==2)
        this->layout()->addWidget(customPlot);

    this->function = function;
    this->n = n;
    QString result = "(";

    // %-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-

    FunctionParser fparser;
    std::string vars;
    if (n == 1)
        vars = "x";
    else
    {
        for (int i = 0; i < n-1; i++)
            vars = vars + "x" + std::to_string(i+1) + ",";
        vars = vars + "x" + std::to_string(n);
    }

    fparser.AddConstant("pi", 3.1415926535897932);
    fparser.Parse(function, vars);

    if (n == 2)
    {
        QCPColorMap *colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
        std::vector<double> point;

        colorMap->data()->setSize(100, 100);
        colorMap->data()->setRange(QCPRange(restrVals[0], restrVals[1]), QCPRange(restrVals[2], restrVals[3]));
        for (int x1 = 0; x1 < 100; ++x1)
            for (int x2 = 0; x2 < 100; ++x2)
            {
                point.clear();
                point.push_back(map(x1, 0, 100, restrVals[0], restrVals[1]));
                point.push_back(map(x2, 0, 100, restrVals[2], restrVals[3]));
                colorMap->data()->setCell(x1, x2, fparser.Eval(point));
            }
        colorMap->setGradient(QCPColorGradient::gpPolar);
        colorMap->rescaleDataRange(true);
        plot(simplex, customPlot);
    }

    std::vector<double> xcenter_old(n,0);   // simplex center * (N+1) N liczb z wart 0
    std::vector<double> xcenter_new(n,0);   // simplex center * (N+1)
    std::vector<double> vf(n+1,0);          // f evaluated at simplex vertices

    int x1 = 0, xn = 0, xnp1 = 0;
    //x1:   f(x1) = min { f(x1), f(x2)...f(x_{n+1} }
    //xnp1: f(xnp1) = max { f(x1), f(x2)...f(x_{n+1} }
    //xn:   f(xn)<f(xnp1) && f(xn)> all other f(x_i)
    int cnt = 0; //iteration step number

    double diff = 0;

    // optimization begins
    for(cnt = 0; cnt < iter; ++cnt)
    {
        for(int i = 0; i < n+1; i++)
            vf[i] = fparser.Eval(simplex[i]);

        x1 = 0; xn = 0; xnp1 = 0; // find index of max, second max, min of vf.

        for(int i = 0; i < vf.size(); i++)
        {
            if(vf[i] < vf[x1])
                x1 = i;
            if(vf[i]>vf[xnp1])
                xnp1=i;
        }
        xn = x1;

        for(int i = 0; i < vf.size(); i++)
            if(vf[i] < vf[xnp1] && vf[i] > vf[xn] )
                xn = i;

        // x1, xn, xnp1 are found

        std::vector<double> xg(n, 0);   // xg: centroid of the N best vertexes
        for(int i=0; i<simplex.size(); i++)
            if(i!=xnp1)
                std::transform(xg.begin(), xg.end(), simplex[i].begin(), xg.begin(), std::plus<double>() );

        std::transform(xg.begin(), xg.end(), simplex[xnp1].begin(), xcenter_new.begin(), std::plus<double>());
        std::transform(xg.begin(), xg.end(), xg.begin(), std::bind2nd(std::divides<double>(), n));
        // xg found, xcenter_new updated

        // termination condition
        diff=0;          // calculate the difference of the simplex center
        for(int i = 0; i < n; i++)
            diff += fabs(xcenter_old[i]-xcenter_new[i]);

        if (diff/n < eps)
            break;            // terminate the optimizer
        else
            xcenter_old.swap(xcenter_new); // update simplex center

        // reflection:
        std::vector<double> xr(n,0);
        for( int i = 0; i < n; i++)
            xr[i] = xg[i] + a * (xg[i] - simplex[xnp1][i]);
        // reflection, xr found

        double fxr=fparser.Eval(xr);    // record function at xr

        if(vf[x1]<=fxr && fxr<=vf[xn])
            std::copy(xr.begin(), xr.end(), simplex[xnp1].begin());

        // expansion:
        else if(fxr<vf[x1])
        {
            std::vector<double> xe(n,0);
            for(int i = 0; i < n; i++)
                xe[i] = xr[i] + b * (xr[i] - xg[i]);
            if(fparser.Eval(xe) < fxr)
                std::copy(xe.begin(), xe.end(), simplex[xnp1].begin());
            else
                std::copy(xr.begin(), xr.end(), simplex[xnp1].begin());
        }   // expansion finished,  xe is not used outside the scope

        // contraction:
        else if(fxr > vf[xn])
        {
            std::vector<double> xc(n,0);
            for(int i = 0; i < n; i++)
                xc[i] = xg[i] + g * (simplex[xnp1][i] - xg[i]);
            if(fparser.Eval(xc) < vf[xnp1])
                std::copy(xc.begin(), xc.end(), simplex[xnp1].begin());
            else
            {
                for( int i=0; i<simplex.size(); i++ )
                    if( i!=x1 )
                        for(int j=0; j<n; j++)
                            simplex[i][j] = simplex[x1][j] + h * ( simplex[i][j]-simplex[x1][j] );
            }
        } // contraction finished, xc is not used outside the scope  

        if (n == 2)
            plot(simplex, customPlot);
    } // optimization is finished

    // %-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-

    for (int i = 0; i < simplex[x1].size(); i++)
    {
        result += QString::number(simplex[x1][i], 'g', -log10(eps)+1);
        result += ", ";
    }
    result.remove(result.size()-2, 2);
    result += ")";

    ui->resultlabel->setText(result);
    ui->iterlabel->setText(QString::number(cnt));
    ui->termlabel->setText(QString::number(diff/n));

    if(cnt == iter)     //max number of iteration achieves before tol is satisfied
    {
        statusBar->showMessage("Iteration limit achieves, result may not be optimal");
        return 1;
    }

    return 0;
}
