#include<cstdio>
#include<iostream>
#include<queue>
#include<algorithm>

using namespace std;

int t, n, m, k;
bool check[51][51];
bool map[51][51];

int dx[4] = { 1, -1, 0, 0 };
int dy[4] = { 0, 0, 1, -1 };

void dfs(int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		int nx = x + dx[i];
		int ny = y + dy[i];
		if (nx >= 0 && ny >= 0 && nx < n && ny < m && !check[nx][ny] && map[nx][ny])
		{
			check[nx][ny] = true;
			dfs(nx, ny);
		}
	}
}

int main()
{
	scanf("%d", &t);

	while (t--)
	{
		int cnt = 0;
		scanf("%d %d %d", &n, &m, &k);

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				check[i][j] = false;
				map[i][j] = false;
			}
		}

		while (k--)
		{
			int a, b;
			scanf("%d %d", &a, &b);
			map[a][b] = true;
		}

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < m; j++)
			{
				if (!check[i][j] && map[i][j])
				{
					dfs(i, j);
					cnt++;
				}
			}
		}

		printf("%d\n", cnt);
	}


	return 0;
}