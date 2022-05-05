#include <iostream>
#include <chrono>

using namespace std;
using namespace chrono;

#define MAXN 6000
#define INFINITY 1000000

void dijkstra();
int e[MAXN + 1][MAXN + 1] = {0};
int vis[MAXN + 1] = {0};
int dis[MAXN + 1];

int n, m;
int u = 0;

int main()
{
    auto begin = std::chrono::high_resolution_clock::now();

    for (int i = 1; i <= n; i++)
    {
        dis[i] = INFINITY;
    }
    cin >> n;
    // 初始化邻接表 填充数据
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &e[i][j]);
        }
    }
    for (int i = 0; i < n; i++)
    {
        dis[i] = e[0][i];
    }
    vis[0] = 1;

    dijkstra();

    for (int i = 0; i < n; i++)
    {
        cout << dis[i] << " ";
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = end - begin;
    cout << endl
         << fp_ms.count() << "ms" << endl;
    return 0;
}
void dijkstra()
{
    for (int i = 1; i <= n - 1; i++)
    {
        int min1 = INFINITY;
        // 寻找权值最小的点u
        for (int j = 0; j < n; j++)
        {
            if (vis[j] == 0 && dis[j] < min1)
            {
                min1 = dis[j];
                u = j;
            }
        }

        vis[u] = 1;

        for (int v = 0; v < n; v++)
        {
            // 对于每个u可达的v来说
            if (e[u][v] < INFINITY)
            {
                // 如果当前的dis[v]不满足三角形不等式，那么进行松弛操作
                if (dis[v] > dis[u] + e[u][v])
                {
                    dis[v] = dis[u] + e[u][v];
                }
            }
        }
    }
}
