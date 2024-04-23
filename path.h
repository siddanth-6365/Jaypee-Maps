#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <fstream>

const int N = 1e5 + 10;
const int INF = INT_MAX;
int ans = 0, ans2 = 0, a = 0;
int weight[1000000] = { 0 };

using namespace std;
vector < vector < int > >vect1;

vector < int >g[N];
vector < pair < int, int > >g2[N];
vector < int >path2;
ifstream f1;

/**
 * Performs a depth-first search on a graph to find a path between two nodes and computes the weight of the path.
 *
 * @param vis2 An array to keep track of visited nodes.
 * @param s The starting node.
 * @param d The destination node.
 * @param path A vector to keep track of the current path.
 *
 * @returns None
 */

void
dfs (int vis2[], int s, int d, vector < int >path)
{

  vis2[s] = 1;


  if (s == d)
    {

      for (int i = 0; i < path.size (); i++)
	{

	  path2.push_back (path[i]);
	}
      ans2++;
      vector < int >vect2;

      for (int j = 0; j < path.size (); j++)
	{
	  vect2.push_back (path[j]);
	}

      for (int j = 0; j < path.size (); j++)
	{
	  int x, y, wt;
	  f1.open ("dist_matrix.txt");
	  while (f1 >> x >> y >> wt)
	    {

	      if (path[j] == x && path[j + 1] == y || path[j] == y
		  && path[j + 1] == x)
		{

		  weight[a] = weight[a] + wt;
		}
	    }
	  f1.close ();
	}

      vect2.push_back (weight[a]);
      vect1.push_back (vect2);
      vect2.clear ();

      a++;
    }

  else
    {

      for (int i = 0; i < g[s].size (); i++)
	{

	  int y = g[s][i];

	  if (vis2[y] == 0)
	    {

	      vis2[y] = 1;

	      path.push_back (y);

	      dfs (vis2, y, d, path);
	      path.pop_back ();
	    }
	}
    }

  vis2[s] = 0;
}
