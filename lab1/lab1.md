# LAB1

- PB19000362
- 钟书锐

## 问题描述

计算出自然常数$e$的前100位有效数字。

自行参考合适的数值方法（数值积分、无穷级数、蒙特卡洛等）实现。

选取OpenMP、MPI、CUDA中的一种实现。可以调用第三方高精度计算库，数值方法必须自行实现。

## 算法设计

### 问题分析
算法需要求出e的前100位
主流有三种方法求e:数值积分、无穷级数、蒙特卡洛

- 无穷级数：利用公式:$e=1 +\frac{1}{1!}+ \frac{1}{2!}+..+ \frac{1}{n!}+...$
- 数值积分：利用公式$ln(t)=\int_{1}^{t}\frac{1}{x}dx$，则$\int_{1}^{e}\frac{1}{x}$=1
- 蒙特卡洛：蒙特卡洛方法是一种用野蛮粗暴的蛮力对抗精致数学的一种计算思维
     - 利用公式$ln(t)=\int_{1}^{t}\frac{1}{x}dx$，=》$\int_{1}^{2}\frac{1}{x}dx=ln(2)$
     - 

### 算法描述


### 方案1；无穷级数
- 利用公式:$e=1 +\frac{1}{1!}+ \frac{1}{2!}+..+ \frac{1}{n!}+...$


- 串行版本:
```
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
```


### 方案2；数值积分
- 利用$ln(t)=\int_{1}^{t}\frac{1}{x}dx$

- 则$\int_{1}^{e}\frac{1}{x}$=1


- 对$\frac{1}{x}$从1开始积分

## 实验评测

### 实验配置

### 数据集配置

## 实验结果

## 结论

###  参考文献