#include "head.h"
#include "antsystem.h"

int n;
int **dis;


double CalcPseudoEuclideanDistance(int xi, int yi, int xj, int yj) {
  double sum = pow(xi - xj, 2) + pow(yi - yj, 2);
  double d = sqrt(sum / 10);
  int intd = (int)(d);
  if (intd < d)
    return intd + 1;
  return intd;
}

void Input() {
  n = 0;
  vector<int> xs, ys;
  int id;
  int x, y;

  while (cin >> id >> x >> y) {
    xs.push_back(x);
    ys.push_back(y);
    n = n + 1;
  }
  
  assert(n > 0);
  dis = new int*[n];
  for (int i = 0; i < n; ++ i)
    dis[i] = new int[n];

  for (int i = 0; i < n; ++ i) {
    for (int j = 0; j < n; ++ j) {
      dis[i][j] = CalcPseudoEuclideanDistance(xs[i], ys[i], xs[j], ys[j]);
    }
  }
}

void ShowDis() {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++)
      cout << dis[i][j] << "\t";
    cout << endl;
  }
}

int main() {
  srand((unsigned int)time(NULL));
  Input();
  
  AntCycle AC = AntCycle(n, dis, 50);
  cout << "Run Ant-Cycle Model" << endl;
  AC.Run();
  AC.ShowSolution();
  
  AntDensity AD = AntDensity(n, dis, 50);
  cout << "Run Ant-Density Model" << endl;
  AD.Run();
  AD.ShowSolution();

  AntQuantity AQ = AntQuantity(n, dis, 50);
  cout << "Run Ant-Quantity Model" << endl;
  AQ.Run();
  AQ.ShowSolution();
  return 0;
}