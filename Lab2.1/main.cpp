#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
using namespace std;
//Presupunem ca vrem sa inmultimim doar doua matrici patratice
const int nmax=1e6+2; //nr maxim threaduri
thread threads[nmax]; //lista de threaduri
queue<pair<int,int>> tasks; //lista taskurilor ce urmeaza sa fie facute, un taks reprezinta o pereche <indiceLinie,indiceColoana>
ifstream fin("matrix.in");
ofstream fout("matrix.out");
vector<vector<int>> resProd; //matricea resultat
vector<vector<int>> A; //prima matrice de inmultit
vector<vector<int>> B; //a doua matrice de inmultit
mutex mutexx;
void initRes(int n,int m){ //initializarea listei rezultat
    //rezultatul o sa aiba cate linii are A
    //cate coloane are B
    for(int i=0;i<n;i++){
        vector<int> aux(m,0);
        resProd.push_back(aux);
    }
}
void initiTasks(int n,int m){ //creerea listei de taskuri
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            cout<<i<<" "<<j<<"\n";
            tasks.push(make_pair(i,j));
        }
    }
}
void readMatrix(vector<vector<int>>& A){
    int n,m;
    fin>>n>>m;
    for(int i=0;i<n;i++){
        vector<int> aux;
        A.push_back(aux);
        for(int j=0;j<m;j++){
            int x;
            fin>>x;
            A[i].push_back(x);
        }
    }
}
void printMatrix(vector<vector<int>>& A){
    for(int i=0;i<A.size();i++){
        for(int j=0;j<A[i].size();j++){
            cout<<A[i][j]<<" ";
        }
        cout<<"\n";
    }
    cout<<"\n";
}
void prod(){
    while(true){
        pair<int,int> currentTask;
        thread::id this_id =this_thread::get_id();
        mutexx.lock(); //ia lock pe lista de taskuri
        cout<<"Thread id:"<<this_id<<" "<<tasks.size()<<"\n";
        if(tasks.empty()){ //daca nu mai e nici un task, threadul a terminat
            mutexx.unlock();
            return;
        }
        currentTask=tasks.front(); //altfel ia primul task din lista
        tasks.pop();
        mutexx.unlock();
        int line=currentTask.first;
        int column=currentTask.second;
        for(int i=0;i<A[line].size();i++){
            resProd[line][column]+=A[line][i]*B[i][column];
        }
    }
}
void createThreadsProd(int n){ //creezi n threaduri
    for(int i=0;i<n;i++){
        threads[i]=thread(prod);
    }
}
void joinThreadsProd(int n){ //dai join la cele n threaduri
    for(int i=0;i<n;i++){
        threads[i].join();
    }
}
bool checkIsValid(){
    if(A[0].size()!=B.size())
        return false;
    return true;
}
int main()
{
    ofstream log("log.txt", std::ios_base::app | std::ios_base::out);
    clock_t tStart = clock();
    readMatrix(A);
    readMatrix(B);
    printMatrix(A);
    printMatrix(B);
    if(checkIsValid()==false){
        cout<<"Invalid matrix dimension\n";
        return 0;
    }
    initRes(A.size(),B[0].size());
    initiTasks(A.size(),B[0].size());
    int nrThreads=10;
    createThreadsProd(nrThreads);
    joinThreadsProd(nrThreads);
    printMatrix(resProd);
    log<<"NUMBER OF NR THREADS:"<<nrThreads;
    log<<" TIME TAKEN:"<< (double)(clock() - tStart)/CLOCKS_PER_SEC<<"\n";
    return 0;
}
