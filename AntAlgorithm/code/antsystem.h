class Ant {
private:
  vector<int> tabu;
  set<int> allowed;
  int now;
  double length;
  bool canWalk;

public:
  Ant(int start, int nNode) {
    tabu.clear();
    allowed.clear();
    now = start;
    length = 0;
    canWalk = true;

    tabu.push_back(now);
    for (int i = 0; i < nNode; ++ i) {
      if (i != start)
        allowed.insert(i);
    }
  }

  ~Ant() {
    tabu.clear();
    allowed.clear();
    now = -1;
    length = 0;
    canWalk = false;
  }

  int GenerateNextNode(double delta[][], double distance[][]) {
    double maxP = 0.0;
    int nextNode = -1;
    bool hasNext = false;
    for (int j : allowed) {
      if (distance[now][j] < 0)
        continue;
      double gamma = 1.0 / distance[now][j];
      double p = pow(delta[now][j], ALPHA) * pow(gamma, BETA);
      if (p > maxP) {
        maxP = p;
        hasNext = true;
      }
    }
    if (hasNext) {
      vector<int> buffer;
      for (int j : allowed) {
        double gamma = 1.0 / distance[now][j];
        double p = pow(delta[now][j], ALPHA) * pow(gamma, BETA);
        if (fabs(maxP - p) <= EPS)
          buffer.push_back(j);
      }
      nextNode = buffer[rand() % buffer.size()];
      length = length + distance[now][nextNode];
      allowed.erase(allowed.find(nextNode));
      tabu.push_back(nextNode);
    }
    return nextNode;
  }

  void AddStartNode(double distance[][]) {
    if (now != -1 && distance[now][tabu[0]] > 0) {
      length = length + distance[now][tabu[0]];
      tabu.push_back(tabu[0]);
    }
  }

  void SetNow(int n) {
    now = n;
  }

  int GetNow() {
    return now;
  }

  double GetLength() {
    return length;
  }

  void SetCanWalk(bool v) {
    canWalk = v;
  }

  bool CanWalk() {
    return canWalk;
  }

  vector<int> GetTabu() {
    return tabu;
  }
  
};

class AntSystem
{
private:
  vector<Ant> antList;
  int nNode;
  double **pheromone;
  double **distance;
  vector<int> bestTour;
  double minDis;

public:
  AntSystem(int n, double dis[][], int m) {
    antList.clear();
    nNode = n;
    pheromone = new double[nNode][nNode];
    distance = new double[nNode][nNode];
    for (int i = 0; i < nNode; ++ i) {
      for (int j = 0; j < nNode; ++ j) {
        pheromone[i][j] = C;
        distance[i][j] = dis[i][j];
      }
    }

    bestTour.clear();
    minDis = INF;

    for (int i = 0; i < m; ++ i) {
      int st = rand() % nNode;
      antList.push_back(Ant(st, nNode));
    }
  }

  ~AntSystem() {
    antList.clear();
    delete pheromone;
    delete distance;
    bestTour.clear();
    minDis = INF;
  }
  
  void Run() {
    for (int it = 1; it <= MAXITER; ++ it) {
      int bestAnt = -1;
      for (int i = 0; i < antList.size(); ++ i) {
        for (int j = 0; j < nNode && antList[i].CanWalk(); ++ j) {
          int nextNode = antList[i].GenerateNextNode(pheromone, distance);
          if (nextNode == -1)
            antList[i].SetCanWalk(false);
          antList[i].SetNow(nextNode);
        }
        antList[i].AddStartNode(distance);
        if (antList[i].GetTabu().size() == nNode + 1 && antList[i].GetLength() < minDis) {
          minDis = antList[i].GetLength();
          bestAnt = i;
        }
      }
      assert(bestAnt != -1);
      bestTour.clear();
      for (int i : antList[bestAnt].GetTabu())
        bestTour.push_back(i);

      UpdatePheromone();
      for (int i = 0; i < antList.size(); ++ i) {
        int st = rand() % Node;
        antList[i] = Ant(st, nNode);
      }
    }
  }

  void UpdatePheromone() {    
    for (int i = 0; i < nNode; ++ i) {
      for (int j = 0; j < nNode; ++ j) {
        pheromone[i][j] = pheromone[i][j] * (1 - RHO);
      }
    }

    for (int i = 0; i < antList.size(); ++ i) {
      vector<int> tabu = antList[i].GetTabu();
      double length = antList[i].GetLength();
      for (int j = 0; j < tabu.size() - 1; ++ j) {
        int from = tabu[j];
        int to = tabu[j + 1];
        pheromone[from][to] = pheromone[from][to] + Q / length;
      }
    }
  }

  void ShowSolution() {
    cout << "Minimal Path:" << minDis << endl;
    for (int i : bestTour)
      cout << i << "\t";
    cout << endl;
  }
};