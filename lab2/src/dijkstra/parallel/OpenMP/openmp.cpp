#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <assert.h>
#include <iostream>
#include <chrono>

#define MAXN 2000
#define INFINITY 1000000

using namespace std;
using namespace chrono;

int graph[MAXN][MAXN];

int minDistance(int dis[], int vis[], int n)
//从目前未访问的点中找出dist最小的 返回其下标
{
    int min = INFINITY, min_index;
    for (int v = 0; v < n; v++)
        if (vis[v] == 0 && dis[v] <= min)
        {
            min = dis[v];
            min_index = v;
        }
    return min_index;
}

int *dijkstra(int graph[MAXN][MAXN], int src, int n)
// dijkstra 算法
{
    int dis[n];
    int vis[n];
    for (int i = 0; i < n; i++)
        dis[i] = INFINITY, vis[i] = 0;
    dis[src] = 0;
    for (int i = 0; i < n - 1; i++)
    {
        int u = minDistance(dis, vis, n);
        vis[u] = 1;
        for (int v = 0; v < n; v++)
        {
            // 对于每个u可达的v来说
            if (!vis[v] && graph[u][v] < INFINITY)
            {
                // 如果当前的dis[v]不满足三角形不等式，那么进行松弛操作
                if (dis[v] > dis[u] + graph[u][v])
                {
                    dis[v] = dis[u] + graph[u][v];
                }
            }
        }
    }
    int *result;
    result = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        result[i] = dis[i];
    return result;
}

int main(int argc, char *argv[])
{
    int thread_count = strtol(argv[1], NULL, 10);
    int n; //结点数

    // input the graph
    cin >> n;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            cin >> graph[i][j];
    int **answerMatrix, *arrayTemp;

    //统计time
    auto begin = std::chrono::high_resolution_clock::now();
    answerMatrix = (int **)malloc(n * sizeof(int *));
#pragma omp parallel num_threads(thread_count)
#pragma omp for
    for (int i = 0; i < n; i++)
    {
        arrayTemp = dijkstra(graph, i, n);
        answerMatrix[i] = arrayTemp;
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d \t", answerMatrix[i][j]);
        }
        printf("\n");
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = end - begin;
    cout << endl
         << fp_ms.count() << "ms" << endl;
}