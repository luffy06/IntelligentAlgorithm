#include "head.h"
#include "antsystem.h"

int n, m;
double **dis;


void Input() {
  cin >> n >> m;
  dis = new double[n][n];

  vector<double> xs, ys;
  for (int i = 0; i < n; ++ i) {
    int id;
    double x, y;
    cin >> id >> x >> y;
    xs.push_back(x);
    ys.push_back(y);
  }

  for (int i = 0; i < n; ++ i) {
    for (int j = 0; j < n; ++ j) {
      double d = sqrt(pow(xs[i] - xs[j], 2) + pow(ys[i] - ys[j]));
      dis[i][j] = d;
    }
  }
}

int main() {
  Input();
  AntSystem AS = AntSystem(n, dis, m);
  AS.Run();
  AS.ShowSolution();
  return 0;
}