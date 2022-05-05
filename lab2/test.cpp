#include <iostream>
#include <random>
using namespace std;
#define MAXN 6000
#define INF 1000000
int n = 5000;
int tmp;
int edge[MAXN + 1][MAXN + 1];
int main()
{

    default_random_engine e;
    uniform_int_distribution<unsigned> u(1, 1000);
    for (int i = 0; i < n; i++)
        for (int j = 0; j <= i; j++)
        {
            if (i == j)
                edge[i][j] = 0;
            else
            {
                if ((tmp = u(e)) > 700)
                    edge[i][j] = edge[j][i] = INF;
                else
                    edge[i][j] = edge[j][i] = tmp;
            }
        }
    cout << n << endl;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            cout << edge[i][j] << " ";
            if (j == n - 1)
                cout << endl;
        }
    return 0;
}