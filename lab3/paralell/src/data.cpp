#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include <ctime>
// martix 10000*10000
// 元素 10000

using namespace std;
int mar[10000][10000];

int main()
{
    unsigned seed; // Random generator seed
    seed = time(0);
    srand(seed);
    int i, j;
    for (i = 0; i < 10000; i++)
        for (j = 0; j < 10000; j++)
            mar[i][j] = 0;
    fstream fp1("./martix.txt");
    fstream fp2("./vector.txt");
    fp1 << 10000 << " " << 10000 << " " << 10000 << endl;

    int cnt = 0;
    while (cnt < 5000)
    {
        int a = rand() % 10000;
        int b = rand() % 10000;
        int c = rand() % 1000;
        if (mar[a][b] == 0)
        {
            cnt++;
            mar[a][b] = 1;
            fp1 << a << " " << b << " " << c << endl;
        }
    }
    cnt = 0;
    while (cnt < 10000)
    {
        int a = rand() % 1000;
        fp2 << a << endl;
        cnt++;
    }
}