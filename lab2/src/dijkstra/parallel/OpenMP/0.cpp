#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;

#define MAXN 6000

int graph[MAXN][MAXN];

struct point
{
    int dis;
    int index;
};

#pragma omp declare reduction(min                                                                        \
                              : struct point                                                             \
                              : omp_out.dis = omp_in.dis > omp_out.dis ? omp_out.dis : omp_in.dis,       \
                                omp_out.index = omp_in.dis > omp_out.dis ? omp_out.index : omp_in.index) \
    initializer(omp_priv = {INT_MAX, INT_MAX})

void dijkstra(int n, int source)
{
    int distance[n], predecessor[n], visited[n];

    for (int i = 0; i < n; i++)
    {
        distance[i] = graph[source][i];
        predecessor[i] = source;
        visited[i] = 0;
    }

    distance[source] = 0;
    visited[source] = 1;
    int count = 1;
    while (count < n - 1)
    {
        struct point mini = {INT_MAX, INT_MAX}, ans = {INT_MAX, INT_MAX};
        int i, j;
#pragma omp parallel for private(j) firstprivate(mini) reduction(min \
                                                                 : ans)
        for (i = 0; i < n; i++)
        {
            j = omp_get_thread_num();
            if (distance[i] < mini.dis && !visited[i])
            {
                mini.dis = distance[i];
                mini.index = i;
                ans.dis = mini.dis;
                ans.index = mini.index;
            }
        }
        int nextnode = ans.index;
        visited[nextnode] = 1;
        // This will update values and see if going through the newly explored node there is shorter path to any other unexplored node.
        mini.dis = ans.dis;
        mini.index = ans.index;

#pragma omp parallel for
        for (int i = 0; i < n; i++)
        {
            if (!visited[i])
            {
                if (graph[nextnode][i] != INT_MAX && mini.dis + graph[nextnode][i] < distance[i])
                {
                    distance[i] = mini.dis + graph[nextnode][i];
                    predecessor[i] = nextnode;
                }
            }
        }
        count++;
    }
    // This will print the results
    for (int i = 0; i < n; i++)
        if (i != source)
            printf("%d\n", distance[i]);
        
}

int main(int argc, char *argv[])
{
    int n, u;
    int thread = strtol(argv[1], NULL, 10);

    // input
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &graph[i][j]);
        }
    }
    omp_set_num_threads(thread); // Set the number of threads

    auto begin = std::chrono::high_resolution_clock::now();
    dijkstra(n, u);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = end - begin;
    cout << endl
         << fp_ms.count() << "ms" << endl;
    return 0;
}
