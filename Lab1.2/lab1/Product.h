#ifndef PRODUCT_H
#define PRODUCT_H
#include<iostream>
using namespace std;
class Product{
private:
    string name;
    const double price;
    int quantity;

public:
    Product();
    Product(string name,const double price,int quantity);
    string getName();
    void setName(string name);
    double getPrice();
    int getQuantity();
    void setQuantity(int quantity);
    friend ostream& operator<<(ostream& os, const Product& prod){
        os <<"PRODUCT:"<<prod.name<< " price:" << prod.price << " quantity:" << prod.quantity;
        return os;
    };
};
#endif // PRODUCT_H

