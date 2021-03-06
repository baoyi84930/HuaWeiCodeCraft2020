#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
/*
    将ui 映射改为string映射，一开始存id 映射string
*/
#define TEST
int threadnum = 8;
vector<int> all_loops;
typedef unsigned int ui;
vector<array<string, 5>> Ans;
class Check
{
public:
    vector<vector<int>> Gout, Gin;
    unordered_map<ui, int> id2num;
    vector<string> ids;
    vector<ui> data;
    vector<bool> vis, able3, able4;
    unordered_map<int, vector<int>> p3;
    unordered_map<int, vector<array<int, 2>>> p4;
    int nodeCnt;
    int loops;
    void operator()(int threadId)
    {
        vis = vector<bool>(nodeCnt, false);
        int path[7];
        able3 = vector<bool>(nodeCnt, false);
        able4 = vector<bool>(nodeCnt, false);
        for (int i = threadId; i < nodeCnt; i += threadnum)
        {
            // cout << threadId << endl;
            if (!Gout[i].empty())
            {
                prepare(i);
                if (p3.size() == 0)
                {
                    continue;
                }
                d1(i, path);
                p3.clear();
                p4.clear();
                able3 = vis;
                able4 = vis;
                // frmpath1.clear();
                // frmpath2.clear();
                // frmpath3.clear();
            }
        }
        all_loops[threadId] = loops;
    }

    void readInput(string &File)
    {
        FILE *file = fopen(File.c_str(), "r");
        ui u, v, c;
        while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF)
        {
            data.push_back(u);
            data.push_back(v);
        }
        auto tmp = data;
        sort(tmp.begin(), tmp.end());
        tmp.erase(unique(tmp.begin(), tmp.end()), tmp.end());
        // cout << clock() - t << endl;
        nodeCnt = tmp.size();
        nodeCnt = 0;
        for (ui &x : tmp)
        {
            ids.push_back(to_string(x));
            id2num[x] = nodeCnt++;
        }

        int sz = data.size();
        Gout = vector<vector<int>>(nodeCnt);
        Gin = vector<vector<int>>(nodeCnt);

        for (int i = 0; i < sz; i += 2)
        {
            int u = id2num[data[i]], v = id2num[data[i + 1]];
            Gout[u].push_back(v);
            Gin[v].push_back(u);
        }

        for (int i = 0; i < nodeCnt; i++)
        {
            sort(Gout[i].begin(), Gout[i].end());
            sort(Gin[i].begin(), Gin[i].end());
        }
        loops = 0;
    }

    void backhome(int head, int cur, int depth, int able, int *path)
    {
        path[depth - 1] = cur;
        string tmp = "";
        for (int i = 0; i < depth; i++)
        {
            tmp.append(ids[path[i]] + ",");
        }
        switch (able)
        {
        case 3:
            for (int &u : p3[cur])
            {
                if (!vis[u])
                {
                    Ans[head][depth - 2].append(tmp);
                    Ans[head][depth - 2].append(ids[u] + "\n");
                    loops++;
                }
            }
            break;
        case 4:
            for (array<int, 2> &u : p4[cur])
            {
                if (!vis[u[0]] && !vis[u[1]])
                {
                    Ans[head][depth - 1].append(tmp);
                    Ans[head][depth - 1].append(ids[u[0]] + ",");
                    Ans[head][depth - 1].append(ids[u[1]] + "\n");
                    loops++;
                }
            }
            break;
        }
    }
    void d1(int head, int *path)
    {
        vis[head] = true;
        path[0] = head;
        for (int &v : Gout[head])
        {
            if (v < head)
            {
                continue;
            }
            if (able3[v])
            {
                backhome(head, v, 2, 3, path);
            }
            if (p4.size() == 0)
            {
                continue;
            }
            if (able4[v])
            {
                backhome(head, v, 2, 4, path);
            }
            dfs(head, v, 2, path);
        }
        vis[head] = false;
    }

    void dfs(int head, int cur, int depth, int *path)
    {
        if (depth == 5)
        {
            return;
        }
        vis[cur] = true;
        path[depth - 1] = cur;
        for (int &v : Gout[cur])
        {
            // 剪枝
            if (v <= head || vis[v])
            {
                continue;
            }
            if (able4[v] && depth < 5)
            {
                backhome(head, v, depth + 1, 4, path);
            }
            dfs(head, v, depth + 1, path);
        }
        vis[cur] = false;
    }
    // unordered_map<int, vector<vector<int>>> p3,
    void prepare(int i)
    {
        array<int, 2> temp2;
        // temp = vector<int>(1);
        // temp2 = vector<int>(2);
        for (int &v : Gin[i])
        {
            // temp[0] = v;
            temp2[1] = v;
            if (v > i)
            {
                for (int &u : Gin[v])
                {
                    if (u > i)
                    {
                        // temp.push_back(u);
                        p3[u].push_back(v);
                        able3[u] = true;
                        temp2[0] = u;
                        for (int &w : Gin[u])
                        {
                            if (w > i && w != v)
                            {
                                // tep.push_back(w);
                                p4[w].push_back(temp2);
                                able4[w] = true;
                                sort(p4[w].begin(), p4[w].end());
                            }
                        }
                    }
                }
            }
        }
    }

    /*
        fwrite 逐行写
    */
    void save()
    {
        // FILE *file = fopen("output.txt", "w");
        // int f = open("output.txt", O_RDWR | O_CREAT, 0666);

        FILE *file = fopen("/projects/student/result.txt", "w");
        for (int &u : all_loops)
        {
            loops += u;
        }
        fprintf(file, "%d\n", loops);
        for (int i = 0; i < 5; i++)
        {
            for (int k = 0; k < nodeCnt; k++)
            {
                fwrite(Ans[k][i].c_str(), 1, Ans[k][i].length(), file);
            }
        }
        fflush(file);
        fclose(file);
    }
};

int main()
{
    extern vector<array<string, 5>> Ans;
    extern vector<int> all_loops;
    // string testFile = "test_data.txt";
    string testFile = "/data/test_data.txt";
    // auto t = clock();
    Check check;
    vector<thread> threadname;
    check.readInput(testFile);
    Ans = vector<array<string, 5>>(check.nodeCnt);
    all_loops = vector<int>(threadnum, 0);
    for (int i = 0; i < threadnum; i++)
    {
        thread mythread(check, i);
        threadname.push_back(move(mythread));
    }
    for (int i = 0; i < threadnum; i++)
    {
        threadname[i].join();
    }
    check.save();
    // cout << clock() - t << endl;
    // system("pause");
    return 0;
}