#include <stdio.h>
#include <vector>
#include <iostream>
#include <chrono>
using namespace std;
#define INF 0x7fffffff
#define maxN 50
#define USE_C 1
#define NOT_USE_C 0
#define USE_CPP 1
int matrix[maxN][maxN];
void Dijkstra_cpp(vector<vector<int>> &vec, vector<int> &result, int v0)
{
    vector<int> visited(vec.size(), 0);
    int last_visited = 0;
    result[0] = 0;
    for (int i = 0; i < vec.size() - 1; i++)
    {
        for (int j = 0; j < vec.size() - 1; j++)
        {
            if (visited[i] == 0)
            {
                if (vec[v0][j] != 0)
                {
                    int dist = vec[v0][j] + last_visited;
                    if (dist < result[j])
                        result[j] = dist;
                }
            }
        }
        int minIndex = 0;
        while (visited[minIndex] == 1)
            minIndex++;
        for (int j = minIndex; j < vec.size(); j++)
        {
            if (visited[j] == 0 && result[j] < result[minIndex])
            {
                minIndex = j;
            }
        }
        last_visited = result[minIndex];
        visited[minIndex] = 1;
        v0 = minIndex;
    }
}
int _tmain(int argc, _TCHAR *argv[])
{
    freopen("Dijkstra2Data.txt", "r", stdin);
    int n;
    cin >> n;
    vector<vector<int>> vec(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cin >> vec[i][j];
        }
    }
    vector<int> result(n, INF);
    Dijkstra_cpp(vec, result, 0);
    for (int i = 0; i < n; i++)
    {
        if (result[i] == INF)
            cout << "INF" << endl;
        else
            cout << result[i] << endl;
    }
    return 0;
}