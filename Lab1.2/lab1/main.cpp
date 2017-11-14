#if defined (_MSC_VER)  // Visual studio
    #define thread_local __declspec( thread )
#elif defined (__GCC__) // GCC
    #define thread_local __thread
#endif
#include<vector>
#include<string>
#include<cstdlib>
#include<ctime>
#include <thread>
#include <chrono>
#include <mutex>
#include"Sale.h"
#include <random>
#include <time.h>
#include <chrono>
#include <ios>
#include <fstream>

using namespace std;

const int lMax=10; //lungimea maxima a unui cuvant
const int nMax=1e6; //numarul maxim de threaduri
const int maxQuantity=100; //cantitatea maxim posibila

vector<Product*> products;
vector<Sale*> bills;
thread threads[nMax];
vector<mutex* > mutexs;
mutex mutexx;
int sum=0;

double getRandomNr(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}
string getRandomName(){
    int lenght=rand()%lMax+1; //adun 1 ca sa evit lungimea 0
    string s="";
    for(int i=0;i<lenght;i++){
        int ch=rand()%26;
        s+=('a'+ch);
    }
    return s;
}
void createProd(int n){
    for(int i=0;i<n;i++){
        string name=getRandomName();
        const double price=getRandomNr(10,1000);
        int quantity=(int)getRandomNr(0,maxQuantity);
        products.push_back(new Product(name,price,quantity));
        mutex* m=new mutex();
        mutexs.push_back(m);
    }
}
void print(){
    cout<<"PRINT\n";
    for(auto prod:products){
        cout<<*(prod)<<"\n";
    }
    cout<<"\n\n";
}

int intRand(const int & min, const int & max) {
    hash<thread::id> hasher;
    static thread_local mt19937* generator = nullptr;
    if (!generator) generator = new mt19937(clock() + hasher(this_thread::get_id()));
    uniform_int_distribution<int> distribution(min, max);
    return distribution(*generator);
}

void transactions(){
    thread::id this_id =this_thread::get_id();

    mutexx.lock();
    cout << "CLIENT thread " << this_id << "\n";

    int nrProd=products.size();
    int indexProd=intRand(0,nrProd-1);//generez random indicele produsului produsului
    int currentQuantity=intRand(0,maxQuantity);
    Product* currentProd=products[indexProd];

    if(currentProd->getQuantity()>currentQuantity){
        int currentPrice=currentQuantity*currentProd->getPrice();
        bills.push_back(new Sale(currentProd,currentQuantity));
        cout <<"SUCCESS TRANZACTION product:"<<currentProd->getName()<<", current quantity:"<<currentQuantity<<", price:"<<currentPrice<<"\n";
        sum+=currentPrice;
        currentProd->setQuantity(currentProd->getQuantity()-currentQuantity);
    }
    else{
        cout<<"FAILED TRANSACTION: "<<currentProd->getName()<<", current quantity:"<<currentQuantity<<"\n";
    }
    mutexx.unlock();
    this_thread::sleep_for(chrono::seconds(1));
}
void transactions2(){
    thread::id this_id =this_thread::get_id();

    mutexx.lock();
    cout << "CLIENT thread " << this_id << "\n";
    int nrProd=products.size();
    int indexProd=intRand(0,nrProd-1);//generez random indicele produsului produsului
    int currentQuantity=intRand(0,maxQuantity);
    mutexx.unlock();

    mutexs[indexProd]->lock();
    Product* currentProd=products[indexProd];
    if(currentProd->getQuantity()>currentQuantity){
        int currentPrice=currentQuantity*currentProd->getPrice();
        currentProd->setQuantity(currentProd->getQuantity()-currentQuantity);
        mutexx.lock();
        bills.push_back(new Sale(currentProd,currentQuantity));
        cout <<"SUCCESS TRANZACTION product:"<<currentProd->getName()<<", current quantity:"<<currentQuantity<<", price:"<<currentPrice<<"\n";
        sum+=currentPrice;
        mutexx.unlock();
    }
    else{
        mutexx.lock();
        cout<<"FAILED TRANSACTION: "<<currentProd->getName()<<", current quantity:"<<currentQuantity<<"\n";
        mutexx.unlock();
    }
    mutexs[indexProd]->unlock();
}
void inventory(){
    thread::id this_id =this_thread::get_id();
    mutexx.lock();
    cout<<"SUMA "<<sum<<"\n";
    cout << "INVENTAR thread " << this_id << "\n";
    int currentSum=0;
    for(int i=0;i<bills.size();i++){
        currentSum+=bills[i]->getQuanity()*bills[i]->getProduct()->getPrice();
    }
    if(currentSum==sum){
        cout<<"SUCESS INVENTORY\n";
    }
    else{
        cout<<"FAILED INVENTORY\n";
    }
    mutexx.unlock();
    this_thread::sleep_for(chrono::seconds(1));
}
void createThreads(int n,int m,int type){
    for(int i=0;i<n;i++){
        if(type==1){
            threads[i]=thread(transactions2);
            continue;
        }
        threads[i]=thread(transactions);
    }
    for(int i=n,j=0;j<m;j++,i++){
        threads[i]=thread(inventory);
    }
}
void joinThreads(int n,int m){
    for(int i=0;i<n+m;i++){
        threads[i].join();
    }
}
int main(){
    ofstream log("results.out", std::ios_base::app | std::ios_base::out);
    clock_t tStart = clock();
    products.clear();
    int nrProd=10000;
    int nrThreadC=10;
    int nrThreadI=10;
    createProd(nrProd);
    //print();
    int type=1;
    createThreads(nrThreadC,nrThreadI,type);
    joinThreads(nrThreadC,nrThreadI);
    log<<"\nTYPE:"<<type<<" NR PROD:"<<nrProd<<" NR THREADS:"<<nrThreadC<<" NR THREADS INVENTORY:"<<nrThreadI;
    log<<" TIME TAKEN:"<< (double)(clock() - tStart)/CLOCKS_PER_SEC;
    //fiecare thread face mai multe operatii (cumpara de un numar random de ori un produs random)
    //avem o lista de billuri care o sa contina toate tranzactiile
}
