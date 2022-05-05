
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <pthread.h>
using namespace std;

const int MAXRank = 50, ThreadNum = 4, MAXInteger = 0x7fffffff;
class Node
{
public:
    Node(string v, int f, int t)
    {
        value = v;
        father = f;
        tag = t;
    }
    string value;
    int father, tag, cost;
};

class LooseArg
{
public:
    LooseArg(int b, int l) : begin_id(b), loose_id(l) {}
    int begin_id;
    int loose_id;
};

vector<Node> pathinfo;
vector<int> openset;
int Link[MAXRank][MAXRank], N;

void *init_thread(void *id)
{
    int k = *(int *)id;
    for (int i = k; i < MAXRank; i += ThreadNum)
        for (int j = 0; j < MAXRank; j++)
            Link[i][j] = MAXInteger;
}

void init()
{
    pthread_t tid[ThreadNum];
    int id[ThreadNum];
    for (int i = 0; i < ThreadNum; i++)
    {
        id[i] = i;
        pthread_create(tid + i, NULL, init_thread, id + i);
    }
    for (int i = 0; i < ThreadNum; i++)
        pthread_join(tid[i], NULL);
}

void importgraph(char *path)
{
    fstream fin(path);
    if (fin.is_open())
    {
        string s, d;
        int c, a, b, k = 0;
        map<string, int> mapinfo;
        typedef map<string, int>::iterator POINT;
        while (fin)
        {
            fin >> s >> d >> c;
            POINT p = mapinfo.find(s), q = mapinfo.find(d);
            if (p != mapinfo.end() && q != mapinfo.end())
                a = p->second, b = q->second;
            else
            {
                if (p == mapinfo.end())
                {
                    a = k;
                    mapinfo.insert(pair<string, int>(s, k));
                    pathinfo.push_back(Node(s, k++, 0));
                }
                else
                    a = p->second;
                if (q == mapinfo.end())
                {
                    b = k;
                    mapinfo.insert(pair<string, int>(d, k));
                    pathinfo.push_back(Node(d, k++, 0));
                }
                else
                    b = q->second;
            }
            Link[a][b] = c;
        }
        N = k;
    }
    else
        cout << "open file error" << endl;
}

void *loose_thread(void *arg)
{
    LooseArg *p = (LooseArg *)arg;
    for (int i = p->begin_id; i < openset.size(); i += ThreadNum)
    {
        int x = openset[i];
        if (Link[p->loose_id][x] == MAXInteger)
            continue;
        if (pathinfo[p->loose_id].cost + Link[p->loose_id][x] < pathinfo[x].cost)
        {
            pathinfo[x].father = p->loose_id;
            pathinfo[x].cost = pathinfo[p->loose_id].cost + Link[p->loose_id][x];
        }
    }
    delete p;
}

void dijkstra(string start)
{
    int min = MAXInteger, k = 0, x = 0, begin = 0;
    for (int i = 0; i < N; i++)
        if (pathinfo[i].value == start)
        {
            begin = i;
            break;
        }
    pathinfo[begin].tag = 1;
    pathinfo[begin].cost = 0;
    for (int i = 0; i < N; i++)
    {
        if (i != begin)
        {
            openset.push_back(i);
            pathinfo[i].cost = Link[begin][i];
            pathinfo[i].father = begin;
        }
    }
    while (openset.size())
    {
        min = MAXInteger;
        for (int i = 0; i < openset.size(); i++)
        {
            k = openset[i];
            if (pathinfo[k].cost < min)
                x = i, min = pathinfo[k].cost;
        }
        k = openset[x];
        pathinfo[k].tag = 1;
        openset.erase(openset.begin() + x);

        pthread_t tid[ThreadNum];
        int id[ThreadNum];
        for (int i = 0; i < ThreadNum; i++)
        {
            id[i] = i;
            pthread_create(tid + i, NULL, loose_thread, new LooseArg(id[i], k));
        }
        for (int i = 0; i < ThreadNum; i++)
            pthread_join(tid[i], NULL);
    }
}

void print()
{
    for (int i = 0; i < N; i++)
        cout << pathinfo[i].value << '\t';
    cout << endl;
    for (int i = 0; i < N; i++)
        cout << pathinfo[pathinfo[i].father].value << '\t';
    cout << endl;
    for (int i = 0; i < N; i++)
        cout << pathinfo[i].tag << '\t';
    cout << endl;
    for (int i = 0; i < N; i++)
    {
        if (pathinfo[i].cost == MAXInteger)
            cout << "*\t";
        else
            cout << pathinfo[i].cost << '\t';
    }
    cout << endl
         << endl;
}

int main(int argc, char *argv[])
{
    init();
    importgraph((char *)"./graph");
    dijkstra("s");
    print();
    return 0;
}
