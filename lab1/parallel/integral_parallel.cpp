#include <iostream>
#include <omp.h>
#include <gmpxx.h>
#include <ctime>
#include <cmath>

using namespace std;
#define STEPS 10000000
#define THREADS 16

double Trap(double a, double b, int n, double(*f)(double), int chunk){
    double integral, h;
    int k;
    h = (b-a)/n;
    integral = (f(a) + f(b))/2.0;

#pragma omp parallel for reduction(+:integral) schedule(dynamic, chunk)
    for (int k = 1; k <= n-1 ; ++k) {
        integral += f(a+k*h);
    }
    integral = integral*h;

    return integral;
}

double f(double x)
{
    return 1/x;
}


int main()
{
    clock_t start = clock();
    double r=Trap(1.0, 2.0, STEPS, f, THREADS);
    double e = pow(2,1/r);
    printf("%.40f\n",e);
    clock_t end = clock();
    cout << "using" << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;
     return 0;
}
