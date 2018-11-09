#include <iostream>
#include <fstream>
#include <string>
#include <climits>
#include <vector>
#include <queue>
#include <algorithm>
#include <math.h>
#include <iomanip>  
using namespace std;

typedef struct{
    int p;
    int at;             //arrive time
    int bt;             //burst time
    int rec_bt;         //static burst time
    bool inqueue;
}Process;

typedef struct{
    int ft;
    int et;
}Result;

bool comp (Process i,Process j) { return (i.bt<j.bt); }

bool comp_p (Process i,Process j) { return (i.p<j.p); }

int main (int argc, char* argv[]) {
    string line;
    ifstream fin (argv[1]);
    ofstream fout("ans1.txt");
    int n,sum=0;
    vector <Process> P;
    queue <Process> q;
    vector <Result> r;
    if (fin.is_open()){
        fin >>  n;
        P.resize(n);
        r.resize(n);
        for (int i = 0; i < n; i++){
            P[i].p=i;
            P[i].inqueue=0;
            fin >> P[i].at;
        }
        for (int i = 0; i < n; i++){
            fin >>P[i].bt;
            P[i].rec_bt=P[i].bt;
            sum+=P[i].bt;
        }
        fin.close();
    }
    else cout << "Unable to open file";
    sort(P.begin(),P.end(),comp);
    // for(int i=0;i<n;i++){
    //     cout<<P[i].p<<" "<<P[i].at<<" "<<P[i].bt<<endl;
    // }
    int first[n]={0};
    for(int now=0;now<sum;){
        for(int i=0;i<n;i++){
            if((P[i].at-now<=0)&&P[i].inqueue==0&&P[i].bt>0){
                P[i].inqueue=1;
                q.push(P[i]);
            }
        }
        if(q.empty()){
            now++;
            continue;
        }
        Process tmp=q.front();
        q.pop();
        // cout<<"now = "<<now<<endl;
        // cout<<tmp.p<<" "<<tmp.at<<" "<<tmp.bt<<" "<<endl;
        if(first[tmp.p]==0){
            r[tmp.p].ft=now;
            first[tmp.p]=1;
        }
        now+=tmp.bt;
        r[tmp.p].et=now;
        for(int i=0;i<n;i++){
            if((P[i].at-now<=0)&&P[i].inqueue==0&&P[i].bt>0){
                P[i].inqueue=1;
                // cout<<"P[i] = "<<P[i].p<<endl;
                q.push(P[i]);
            }
        }
    }
    sort(P.begin(),P.end(),comp_p);
    //cout << "Process\t\tFirst In Time\tEnding Time"<<endl;
    // for(int i=0;i<n;i++){
    //     cout<<P[i].p<<"\t\t"<<r[i].ft<<"\t\t"<<r[i].et<<endl;
    // }
    // cout << "Process\t\tWaiting Time\tTurnaround Time"<<endl;
    int wt[n],tt[n];
    int sumwt=0,sumtt=0;
    for(int i=0;i<n;i++){
        wt[i]=r[i].et-P[i].at-P[i].rec_bt;
        tt[i]=r[i].et-P[i].at;
    }
    for(int i=0;i<n;i++){
        // cout<<P[i].p<<"\t\t"<<wt[i]<<"\t\t"<<tt[i]<<endl;
        fout<<wt[i]<<" "<<tt[i]<<endl;
        sumwt+=wt[i];
        sumtt+=tt[i];
    }
    float avwt=(float)sumwt/n;
    float avtt=(float)sumtt/n;
    int decide_avwt=log10(avwt);
    fout<<setprecision(decide_avwt+6)<<avwt<<endl;
    int decide_avtt=log10(avtt);
    fout<<setprecision(decide_avtt+6)<<avtt<<endl;
}