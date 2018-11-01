#include <iostream>
#include <fstream>
#include <string>
#include <climits>
using namespace std;

int main (int argc, char* argv[]) {
  string line;
  ifstream fin (argv[1]);
  ofstream fout("ans1.txt");
  int n;
  int burst[20], tmp[21], arrival[20], p[20], wt[20], tat[20];
  float avwt = 0, avtat = 0;
  if (fin.is_open())
  {
    fin >>  n;
    for (int i = 0; i < n; i++){
        fin >> arrival[i];
        p[i] = i+1;
    }
    for (int i = 0; i < n; i++){
        fin >> burst[i];
        tmp[i] = burst[i];
    }
    fin.close();
  }
  else cout << "Unable to open file";

  int cnt = 0;
  int end = 0;
  tmp[20] = INT_MAX;
  for (int time = 0; cnt != n;)
  {
        int min = 20;
        for (int i = 0; i < n; i++){
            if (arrival[i] <= time && tmp[i] < tmp[min] && tmp[i] > 0)
                min = i;
        }
        if (min != 20){
            cnt++;
            end = time + tmp[min];
            time += tmp[min];
            tmp[min] = 0;
            wt[min] = end - arrival[min] - burst[min];
            tat[min] = end - arrival[min];
            avwt += wt[min];
            avtat += tat[min];
        }else {
            time++;
        }
  }

  cout << "Process\t\tWaiting Time\tTurnaround Time";

  for (int i = 0; i < n; i++){
     cout << "\n[" << p[i] << "]" << "\t\t" << wt[i] << "\t\t" << tat[i];
     fout<<wt[i]<<" "<<tat[i]<<endl;
  }
  avwt /= n;
  avtat /= n;
  cout << "\n\nAverage Waiting Time:" << avwt;
  cout << "\nAverage Turnaround Time:" << avtat << endl;
  
  fout<<avwt<<endl;
  fout<<avtat<<endl;
  fout.close();
  return 0;
}