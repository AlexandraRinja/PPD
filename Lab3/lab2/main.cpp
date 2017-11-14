#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;
ifstream fin("matrix.in");
ofstream fout("matrix.out");

const int nMax=1e6;
thread threads[nMax];
thread threadsProd[nMax];
int auxProd[nMax];
vector<vector<int>> resSum;
vector<vector<int>> resProd;
vector<vector<int>> A;
vector<vector<int>> B;
mutex mutexx;
vector<mutex* > mutexs;
bool finish=false;
int countReset=0;

void initRes(int n,int m){
    for(int i=0;i<n;i++){
        vector<int> aux(m,0);
        resSum.push_back(aux);
        resProd.push_back(aux);
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
void sum(int i,int j,int cate,int n,int m){
    if(cate==-1){
        //e ultimul thread ia tot ce a ramas
        for(;i<n;){
            resSum[i][j]=A[i][j]+B[i][j];
            j+=1;
            if(j>=m){
                i+=j/m;
                j=j%m;
            }
        }
    }
    for(;cate>0;cate--){
        resSum[i][j]=A[i][j]+B[i][j];
        j+=1;
        if(j>=m){
            i+=j/m;
            j=j%m;
        }
    }
}
void createThreads(int nr,int n,int m){
    int cate=(n*m)/nr; //cate element pe thread
    int startI=0;
    int startJ=0;
    for(int i=0;i<nr-1;i++){
        threads[i]=thread(sum,startI,startJ,cate,n,m);
        startJ=startJ+cate;
        if(startJ>=m){
            startI+=startJ/3;
            startJ=startJ%3;
        }
    }
    threads[nr-1]=thread(sum,startI,startJ,-1,n,m);
}
void joinThreads(int n){
    for(int i=0;i<n;i++){
        threads[i].join();
    }
}

void shiftRow(int rowIndex,int countt,int dimension){
    int n=A[rowIndex].size();
    while(countt!=0){
        for(int k=0;k<dimension;k++,rowIndex++){
            int aux[dimension];
            for(int i=0;i<dimension;i++){
                aux[i]=A[rowIndex][i];
            }
            int i;
            for(i=0;i<n-dimension;i++){
                A[rowIndex][i]=A[rowIndex][i+dimension];
            }
            for(int j=0;i<n;i++,j++){
                A[rowIndex][i]=aux[j];
            }
        }
        countt--;
    }
}
void shiftColumn(int columnIndex,int countt,int dimension){
    int n=A.size();
    while(countt!=0){
        for(int k=0;k<dimension;k++,columnIndex++){
            int aux[dimension];
            for(int i=0;i<dimension;i++){
                aux[i]=B[i][columnIndex];
            }
            int i;
            for(i=0;i<n-dimension;i++){
                B[i][columnIndex]=B[i+dimension][columnIndex];
            }
            for(int j=0;j<dimension;j++,i++){
                B[i][columnIndex]=aux[j];
            }
        }
        countt--;
    }
}
bool resetF(int n,int dimension){
    int aux=n/dimension-1;
    mutexx.lock();
    countReset+=1;
    if(countReset>aux){
        mutexx.unlock();
        return false;
    }
    for(int i=0;i<n;i+=dimension){
        shiftRow(i,1,dimension);
    }
    for(int j=0;j<n;j+=dimension){
        shiftColumn(j,1,dimension);
    }
    //printMatrix(A);
    //printMatrix(B);
    mutexx.unlock();
    return true;
}
void sett(int n,int dimension){
    for(int k=0,i=0;i<n;i+=dimension,k++){
        shiftRow(i,k,dimension);
        shiftColumn(i,k,dimension);
    }
}
void createMutex(int nr){
    for(int i=0;i<nr;i++){
        mutex* m=new mutex();
        mutexs.push_back(m);
    }
}
mutex mutexCheck;
void prod(int index,int startI,int startJ,int dimension){
    thread::id this_id =this_thread::get_id();

    mutexx.lock();
    cout<<"Thread id:"<<this_id<<" "<<index<<" S:"<<startI<<" "<<startJ<<"\n";
    mutexx.unlock();

    while(true){

        mutexx.lock();
        //cout<<"THREAD ID:"<<this_id<<"\n";
        if(finish){ //am terminat
            mutexx.unlock();
            break;
        }
        mutexx.unlock();

        mutexs[index]->lock();
        //mutexx.lock();
        //cout<<"Index:"<<index<<" mod:"<<auxProd[index]<<"\n";
        //mutexx.unlock();
        if(auxProd[index]==1){
            mutexs[index]->unlock();
        }
        else{ //nu am procesat partea asta pana acum
            for(int i=0;i<dimension;i++){
                for(int j=0;j<dimension;j++){
                    int currI=startI+i;
                    int currJ=startJ+j;
                    for(int k=0;k<dimension;k++){
                        resProd[currI][currJ]+=A[currI][k]*B[k][currJ];
                    }
                }
            }
            auxProd[index]=1;
            mutexs[index]->unlock();
        }
        this_thread::sleep_for (chrono::seconds(1));
    }
}
void check(int nr,int dimension){
    thread::id this_id =this_thread::get_id();
    while(true){
        mutexx.lock();
        //cout<<"CHECK "<<this_id<<"\n";
        if(finish){
            mutexx.unlock();
            break;
        }
        mutexx.unlock();
        for(int i=0;i<nr;i++){
            mutexs[i]->lock();
        }
        bool reset=true; //presupun ca toate is 1
        for(int i=0;i<nr;i++){
            if(auxProd[i]==0){
                reset=false;
                break;
            }
        }
        if(reset){//toate au terminat
            bool finish1=resetF(nr,dimension);
            if(!finish1){//nu mai pot face alt reset
                mutexx.lock();
                finish=true;
                mutexx.unlock();
            }
            for(int i=0;i<nr;i++){
                auxProd[i]=0;
            }
        }
        for(int i=0;i<nr;i++){
            mutexs[i]->unlock();
        }
        this_thread::sleep_for (chrono::seconds(5));
    }
}
void computeNrThread(int n,int& nr,int& dimension){
    if(nr>n*n){
        nr=n*n; //nu am ce sa fac cu mai mult de n*n threaduri
        return;
    }
    int matrixArea=n*n;
    int nrCurr=0;
    for(int i=1;i<=n;i++){
        //i e dimensiunea pt o matrice
        int currArea=i*i;
        int nrThread=matrixArea/currArea;
        int rest=matrixArea%currArea;
        if(rest!=0){
            continue;
        }
        if(nrThread>nr){
            continue;
        }
        nr=nrThread;
        dimension=i;
        break;
    }
}
void createThreadsProd(int n,int& nr,int& dimension){ //n pt ca matricea e patratica
    //nr numarul efectiv de threaduri
    //d dimensiunea fiecare block din matrice
    //n dimensiunea matricei
    computeNrThread(n,nr,dimension);
    //cout<<"Dimensiune matrice:"<<n<<" Nr threaduri:"<<nr<<" dimensiune bucata:"<<dimension<<"\n";
    int startI=0;
    int startJ=0;
    for(int i=0;i<nr;i++){
        threadsProd[i]=thread(prod,i,startI,startJ,dimension);
        auxProd[i]=0;
        startJ+=dimension;
        if(startJ>=n){
            startI+=dimension;
            startJ=0;
        }
    }
    //un thread ce verifica cand sa te opresti
    threadsProd[nr]=thread(check,nr,dimension);
}
void joinThreadsProd(int nr){
    for(int i=0;i<=nr;i++){
        threadsProd[i].join();
    }
}
int main()
{
    ofstream log("log.txt", std::ios_base::app | std::ios_base::out);
    clock_t tStart = clock();
    readMatrix(A);
    readMatrix(B);

    printMatrix(A);
    printMatrix(B);

    int n=A.size();
    initRes(n,n);
    int nr=5;
    int aux=nr;
    int dimension=1;
    createThreadsProd(A.size(),nr,dimension);
    createMutex(nr);
    joinThreadsProd(nr);
    printMatrix(resProd);
    log<<"INITIAL NUMBER OF NR THREADS:"<<aux<<" ACTUAL NUMBER OF THREADS:"<<nr;
    log<<" TIME TAKEN:"<< (double)(clock() - tStart)/CLOCKS_PER_SEC<<"\n";
}

