#include <iostream>
#include <omp.h>
#include <gmpxx.h>
#include <ctime>

using namespace std;
#define STEPS 10000
#define THREADS 8

int main()
{
    clock_t start = clock();
    mpf_set_default_prec(3000);
    mpf_t fact[STEPS + 10];
    mpf_t sum[THREADS + 10];
    mpf_t tempt, t;
    mpf_init_set_d(tempt, 1);

    for (int i = 1; i < STEPS; i++)
    {
        mpf_mul_ui(tempt, tempt, i);
        mpf_init_set(fact[i], tempt);
    }
    int k = 0;
    int task = 0;
    omp_set_num_threads(THREADS);

#pragma omp parallel private(k, t, task)
    {
        mpf_init_set_d(t, 1);
        task = omp_get_thread_num();
        mpf_init_set_d(sum[task], 0);
        for (k = task + 1; k < STEPS; k += THREADS)
        {
            mpf_ui_div(t, 1, fact[k]);
            mpf_add(sum[task], sum[task], t);
        }
    }
    mpf_t result;
    mpf_init_set_d(result, 1);
    for (int i = 0; i < THREADS; i++)
        mpf_add(result, result, sum[i]);
    gmp_printf("%.*Ff\n", 200, result);
    clock_t end = clock();
    cout << "using" << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;

    return 0;
}
