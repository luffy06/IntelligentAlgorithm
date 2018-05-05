import numpy as np
import matplotlib.pyplot as plt

def read_data():
  filename = 'result.out'
  f = open(filename, 'r')
  lines = f.readlines()
  f.close()
  datas = []
  for i in range(3):
    name = lines[i * 1000 + i].strip()
    x = []
    y = []
    for j in range(1000):
      line = lines[i * 1000 + j + i + 1]
      ds = line.strip().split(' ')
      x.append(ds[0])
      y.append(ds[1])
    datas.append((name, x, y))
  return datas

def draw_graph(i, name, x, y):
  plt.figure(i)
  plt.title(name)
  plt.plot(y, '-')
  plt.xlabel('Iteration Times')
  plt.ylabel('Min Tour Length')
  # plt.show()
  plt.savefig(name + '.png')

def main():
  data = read_data()
  for i, d in enumerate(data):
    draw_graph(i, d[0], d[1], d[2])

if __name__ == '__main__':
  main()