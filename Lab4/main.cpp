#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <queue>
using namespace std;
//Presupunem ca vrem sa inmultimim doar doua matrici patratice
const int nmax=1e6+2; //nr maxim threaduri
const int Nmax=1e6+2; //nr maxim de lini din rezultat
thread threads1[nmax]; //lista de threaduri pentru prima inmultire
thread threads2[nmax]; //lista de threaduri pentru a doua inmultire
queue<pair<int,int>> tasks1; //lista taskurilor ce urmeaza sa fie facute pentru primult produs, un taks reprezinta o pereche <indiceLinie,indiceColoana>
queue<pair<int,int>> tasks2; //lista taskurilor ce urmeaza sa fie facute pentru al doilea produs, un taks reprezinta o pereche <indiceLinie, indiceColoana>
ifstream fin("matrix.in");
ofstream fout("matrix.out");
vector<vector<int>> resProd1; //matricea resultat
vector<vector<int>> resProd2; //matricea resultat
vector<vector<int>> A; //prima matrice de inmultit
vector<vector<int>> B; //a doua matrice de inmultit
vector<vector<int>> C; //a treia matrice de inmultit
int countt[Nmax]; //vector caracteristic ce o sa il folosesc in procesarea celei de-a doua inmultiri
mutex mutexx1;
mutex mutexx2;
mutex mutexxM[Nmax]; //numarul de mutexuri
void initRes(int n,int m,int k){ //initializarea listei rezultat
    //rezultatul o sa aiba cate linii are A
    //cate coloane are B
    for(int i=0;i<n;i++){
        vector<int> aux(m,0);
        resProd1.push_back(aux);
    }
    for(int i=0;i<n;i++){
        vector<int> aux(k,0);
        resProd2.push_back(aux);
    }
}
void initiTasks(int n,int m,int k){ //creerea listelor de taskuri
    for(int i=0;i<n;i++){ //pentru prima inmultire
        for(int j=0;j<m;j++){
            cout<<i<<" "<<j<<"\n";
            tasks1.push(make_pair(i,j));
        }
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<k;j++){ //pentru a doua inmultire
            cout<<i<<" "<<k<<"\n";
            tasks2.push(make_pair(i,j));
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
void prod1(){
    while(true){
        pair<int,int> currentTask;
        thread::id this_id =this_thread::get_id();
        mutexx1.lock(); //ia lock pe lista de taskuri
        cout<<"THREAD* ID:"<<this_id<<" "<<tasks1.size()<<"\n";
        if(tasks1.empty()){ //daca nu mai e nici un task, threadul a terminat
            mutexx1.unlock();
            return;
        }
        currentTask=tasks1.front(); //altfel ia primul task din lista
        tasks1.pop();
        mutexx1.unlock();
        int line=currentTask.first;
        int column=currentTask.second;
        mutexxM[line].lock(); //pui lock pe linia pe care se afla elementul curent
        countt[line]++;
        for(int i=0;i<A[line].size();i++){
            resProd1[line][column]+=A[line][i]*B[i][column];
        }
        mutexxM[line].unlock();
    }
}
void prod2(){
    while(true){
        pair<int,int> currentTask;
        thread::id this_id =this_thread::get_id();
        mutexx2.lock(); //ia lock pe lista de taskuri
        cout<<"THREAD+ ID:"<<this_id<<" "<<tasks2.size()<<"\n";
        if(tasks2.empty()){ //daca nu mai e nici un task, threadul a terminat
            mutexx2.unlock();
            return;
        }
        currentTask=tasks2.front(); //altfel ia primul task din lista
        tasks2.pop(); //il scot oricum
        mutexx2.unlock();
        int line=currentTask.first;
        int column=currentTask.second;
        mutexxM[line].lock();
        if(countt[line]!=resProd1[0].size()){
            mutexxM[line].unlock();
            tasks2.push(make_pair(line,column));
            continue;
        }
        else{
           for(int i=0;i<resProd1[line].size();i++){
                resProd2[line][column]+=resProd1[line][i]*C[i][column];
            }
            mutexxM[line].unlock();
        }
    }
}
void createThreadsProd(int n,int m){ //creezi n threaduri
    for(int i=0;i<n;i++){
        threads1[i]=thread(prod1);
    }
    for(int i=0;i<m;i++){
        threads2[i]=thread(prod2);
    }
}
void joinThreadsProd(int n,int m){ //dai join la cele n threaduri
    for(int i=0;i<n;i++){
        threads1[i].join();
    }
    for(int i=0;i<m;i++){
        threads2[i].join();
    }
}
bool checkIsValid(){
    if(A[0].size()!=B.size() && B[0].size()!=C.size())
        return false;
    return true;
}
int main()
{
    ofstream log("log.txt", std::ios_base::app | std::ios_base::out);
    clock_t tStart = clock();
    readMatrix(A);
    readMatrix(B);
    readMatrix(C);
    printMatrix(A);
    printMatrix(B);
    printMatrix(C);
    if(checkIsValid()==false){
        cout<<"Invalid matrix dimension\n";
        return 0;
    }
    initRes(A.size(),B[0].size(),C[0].size());
    initiTasks(A.size(),B[0].size(),C[0].size());
    int nrThreadsProd1=10;
    int nrThreadsProd2=10;
    createThreadsProd(nrThreadsProd1,nrThreadsProd2);
    joinThreadsProd(nrThreadsProd1,nrThreadsProd2);
    printMatrix(resProd1);
    printMatrix(resProd2);
    log<<"NUMBER OF NR THREADS FOR THE FIRST PRODUCT:"<<nrThreadsProd1;
    log<<" NUMBER OF NR THREADS FOR THE SECOND PRODUCT:"<<nrThreadsProd2;
    log<<" TIME TAKEN:"<< (double)(clock() - tStart)/CLOCKS_PER_SEC<<"\n";
    return 0;
}
