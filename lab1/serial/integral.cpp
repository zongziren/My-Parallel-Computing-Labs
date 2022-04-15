
#include <iostream>
#include <gmpxx.h>
#include <math.h>

using namespace std;
int main()
{
	mpf_set_default_prec(1000); // 默认精度，即小数点后精确多少位
    mpf_t one;
    mpf_init_set_ui(one, 1); 
    mpf_t ten;
    mpf_init_set_ui(ten, 10); 
    mpf_t h;
    mpf_init_set_d(h, 0.0000001); 
    mpf_t result;
    mpf_init_set_ui(result, 0);
    mpf_t i;
    mpf_init_set_d(i, 1.0);
    mpf_t y;
    mpf_init_set_d(y, 0);
    mpf_add(i,i,h);
    while(mpf_cmp_d(i,2.0)<0)
    {
        mpf_div(y,one,i); 
        mpf_mul(y,y,h);
        mpf_add(result,result,y);
        mpf_add(i,i,h);
    }
    mpf_div(result,one,result); 

    
	return 0;
}
