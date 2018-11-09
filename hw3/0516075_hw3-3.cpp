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

bool comp (Process i,Process j) { return (i.at<j.at); }

int main (int argc, char* argv[]) {
    string line;
    ifstream fin (argv[1]);
    ofstream fout("ans3.txt");
    int n,tq,sum=0;
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
        fin >> tq;
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

        if(first[tmp.p]==0){
            r[tmp.p].ft=now;
            first[tmp.p]=1;
        }
        for(int i=1;i<=tq;i++){
            if((tmp.bt-1)==0){
                r[tmp.p].et=now+1;
                P[tmp.p].bt=0;
                tmp.bt--;
                now++;
                break;
            }
            else{
                now++;
                tmp.bt--;
            }
        }
        for(int j=0;j<n;j++){
            if((P[j].at-now<=0)&&P[j].inqueue==0&&P[j].bt>0){
                P[j].inqueue=1;
                q.push(P[j]);
            }
        }
        if(tmp.bt>0){
            q.push(tmp);
        }
    }
    // cout << "Process\t\tFirst In Time\tEnding Time"<<endl;
    // for(int i=0;i<n;i++){
    //     cout<<P[i].p<<"\t\t"<<P[i].ft<<"\t\t"<<P[i].et<<endl;
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