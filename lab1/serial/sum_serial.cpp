#include <stdio.h>
#include <iostream>
#include <gmpxx.h>
#include <ctime>
using namespace std;

int main()
{
    clock_t start = clock();
    mpf_set_default_prec(30000); // 默认精度，即小数点后精确多少位
    mpf_t fact;
    mpf_init_set_ui(fact, 1); 
    mpf_t N;
    mpf_init_set_d(N, 100000.0); 
    mpf_t K;
    mpf_init_set_d(K, 1); 
    mpf_t answer;
    mpf_init_set_d(answer,1); 
    while (mpf_cmp(N, K))
    {   
        mpf_div(fact,fact,K);
        mpf_add(answer,answer,fact);
        mpf_add_ui(K,K,1);
    }
    gmp_printf("%.*Ff\n", 200, answer);
    clock_t end   = clock();
     cout << "using" << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;
   return 0;
}
