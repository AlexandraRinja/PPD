#include"Product.h"
Product::Product():price(0){
    this->name="";
    this->quantity=0;
}
Product::Product(string name,const double price,int quantity):price(price){
    this->name=name;
    this->quantity=quantity;
}
string Product::getName(){
    return this->name;
}
void Product::setName(string name){
    this->name=name;
}
double Product::getPrice(){
    return this->price;
}
int Product::getQuantity(){
    return this->quantity;
}
void Product::setQuantity(int quantity){
    this->quantity=quantity;
}


