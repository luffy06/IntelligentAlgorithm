class Ant {
private:
  vector<int> tabu;
  set<int> allowed;
  int now;
  int length;

public:
  Ant(int start, int nNode) {
    tabu.clear();
    allowed.clear();
    now = start;
    length = 0;

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
  }

  void GenerateNextNode(double **delta, int **distance, double ALPHA, double BETA) {
    double maxP = 0.0;
    int nextNode = -1;
    bool hasNext = false;
    for (int j : allowed) {
      double gamma = 1.0 / distance[now][j];
      double p = pow(delta[now][j], ALPHA) * pow(gamma, BETA);
      if (p > maxP) {
        maxP = p;
        hasNext = true;
      }
    }
    assert(hasNext);

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
    
    now = nextNode;
  }

  void AddStartNode(int **distance) {
    length = length + distance[now][tabu[0]];
    tabu.push_back(tabu[0]);
  }

  void SetNow(int n) {
    now = n;
  }

  int GetNow() {
    return now;
  }

  int GetLength() {
    return length;
  }

  vector<int> GetTabu() {
    return tabu;
  }

  void ShowTabu() {
    cout << tabu.size() << endl;
    for (int i : tabu)
      cout << i << "\t";
    cout << endl;
  }
  
};

class AntSystem {
protected:
  vector<Ant> antList;
  int nNode;
  double **pheromone;
  int **distance;
  vector<int> bestTour;
  int minDis;
  // coefficient
  double ALPHA;
  double BETA;
  double RHO;
  double Q;
  double C;
  double MAXITER;

public:
  AntSystem(int n, int **dis, int m, string config) {
    ReadConfig(config);
    antList.clear();
    nNode = n;
    pheromone = new double*[nNode];
    for (int i = 0; i < nNode; ++ i)
      pheromone[i] = new double[nNode];
    for (int i = 0; i < nNode; ++ i)
      for (int j = 0; j < nNode; ++ j)
        pheromone[i][j] = C;
    distance = dis;

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
    bestTour.clear();
    minDis = INF;
  }
  
  virtual void Run() = 0;

  virtual void UpdatePheromone() = 0;

  void ReadConfig(string config) {
    fstream in(config, fstream::in);
    string name;
    in >> name >> ALPHA;
    in >> name >> BETA;
    in >> name >> RHO;
    in >> name >> Q;
    in >> name >> C;
    in >> name >> MAXITER;
    in.close();
  }

  void ShowSolution() {
    cout << "Minimal Path:" << minDis << endl;
    for (int i : bestTour)
      cout << i << "\t";
    cout << endl;
  }
};

class AntCycle : public AntSystem {
private:
  fstream out;
public:
  AntCycle(int n, int **dis, int m) : AntSystem(n, dis, m, "antcycle.in") {
    out.open("result.out", fstream::app);
  }
  
  void Run() {
    out << "Ant-Cycle" << endl;
    for (int it = 1; it <= MAXITER; ++ it) {
      if (it % 100 == 0)
        cout << "Iteration " << it << endl;
      int bestAnt = -1;
      for (int i = 0; i < antList.size(); ++ i) {
        for (int j = 0; j < nNode - 1; ++ j) {
          antList[i].GenerateNextNode(pheromone, distance, ALPHA, BETA);
        }
        antList[i].AddStartNode(distance);
        assert(antList[i].GetTabu().size() == nNode + 1);
        if (antList[i].GetLength() < minDis) {
          minDis = antList[i].GetLength();
          bestAnt = i;
        }
      }
      out << it << " " << minDis << endl;
      if (bestAnt != -1) {
        bestTour.clear();
        for (int i : antList[bestAnt].GetTabu())
          bestTour.push_back(i);
      }

      UpdatePheromone();
      for (int i = 0; i < antList.size(); ++ i) {
        int st = rand() % nNode;
        antList[i] = Ant(st, nNode);
      }
    }
    out.close();
  }

  void UpdatePheromone() {    
    for (int i = 0; i < nNode; ++ i) {
      for (int j = 0; j < nNode; ++ j) {
        pheromone[i][j] = pheromone[i][j] * (1 - RHO);
      }
    }

    for (int i = 0; i < antList.size(); ++ i) {
      vector<int> tabu = antList[i].GetTabu();
      int length = antList[i].GetLength();
      for (int j = 0; j < tabu.size() - 1; ++ j) {
        int from = tabu[j];
        int to = tabu[j + 1];
        pheromone[from][to] = pheromone[from][to] + Q / length;
      }
    }
  }
};

class AntDensity : public AntSystem {
private:
  fstream out;
public:
  AntDensity(int n, int **dis, int m) : AntSystem(n, dis, m, "antdensity.in") {
    out.open("result.out", fstream::app);
  }
  
  void Run() {
    out << "Ant-Density" << endl;
    for (int it = 1; it <= MAXITER; ++ it) {
      if (it % 100 == 0)
        cout << "Iteration " << it << endl;
      for (int j = 0; j < nNode - 1; ++ j) {
        for (int i = 0; i < antList.size(); ++ i) {
          antList[i].GenerateNextNode(pheromone, distance, ALPHA, BETA);
        }
        UpdatePheromone();
      }
      int bestAnt = -1;
      for (int i = 0; i < antList.size(); ++ i) {
        antList[i].AddStartNode(distance);
        assert(antList[i].GetTabu().size() == nNode + 1);
        if (antList[i].GetLength() < minDis) {
          minDis = antList[i].GetLength();
          bestAnt = i;
        }
      }
      out << it << " " << minDis << endl;
      if (bestAnt != -1) {
        bestTour.clear();
        for (int i : antList[bestAnt].GetTabu())
          bestTour.push_back(i);
      }

      for (int i = 0; i < antList.size(); ++ i) {
        int st = rand() % nNode;
        antList[i] = Ant(st, nNode);
      }
    }
    out.close();
  }

  void UpdatePheromone() {    
    for (int i = 0; i < nNode; ++ i) {
      for (int j = 0; j < nNode; ++ j) {
        pheromone[i][j] = pheromone[i][j] * (1 - RHO);
      }
    }

    for (int i = 0; i < antList.size(); ++ i) {
      vector<int> tabu = antList[i].GetTabu();
      for (int j = 0; j < tabu.size() - 1; ++ j) {
        int from = tabu[j];
        int to = tabu[j + 1];
        pheromone[from][to] = pheromone[from][to] + Q;
      }
    }
  }
};

class AntQuantity : public AntSystem {
private:
  fstream out;
public:
  AntQuantity(int n, int **dis, int m) : AntSystem(n, dis, m, "antquantity.in") {
    out.open("result.out", fstream::app);
  }
  
  void Run() {
    out << "Ant-Quantity" << endl;
    for (int it = 1; it <= MAXITER; ++ it) {
      if (it % 100 == 0)
        cout << "Iteration " << it << endl;
      for (int j = 0; j < nNode - 1; ++ j) {
        for (int i = 0; i < antList.size(); ++ i) {
          antList[i].GenerateNextNode(pheromone, distance, ALPHA, BETA);
        }
        UpdatePheromone();
      }
      int bestAnt = -1;
      for (int i = 0; i < antList.size(); ++ i) {
        antList[i].AddStartNode(distance);
        assert(antList[i].GetTabu().size() == nNode + 1);
        if (antList[i].GetLength() < minDis) {
          minDis = antList[i].GetLength();
          bestAnt = i;
        }
      }
      out << it << " " << minDis << endl;
      if (bestAnt != -1) {
        bestTour.clear();
        for (int i : antList[bestAnt].GetTabu())
          bestTour.push_back(i);
      }

      for (int i = 0; i < antList.size(); ++ i) {
        int st = rand() % nNode;
        antList[i] = Ant(st, nNode);
      }
    }
    out.close();
  }

  void UpdatePheromone() {    
    for (int i = 0; i < nNode; ++ i) {
      for (int j = 0; j < nNode; ++ j) {
        pheromone[i][j] = pheromone[i][j] * (1 - RHO);
      }
    }

    for (int i = 0; i < antList.size(); ++ i) {
      vector<int> tabu = antList[i].GetTabu();
      for (int j = 0; j < tabu.size() - 1; ++ j) {
        int from = tabu[j];
        int to = tabu[j + 1];
        pheromone[from][to] = pheromone[from][to] + Q / distance[from][to];
      }
    }
  }
};