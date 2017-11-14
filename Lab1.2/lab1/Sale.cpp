#include"Sale.h"
Sale::Sale(Product* prod,int quantity){
    this->product=prod;
    this->quantity=quantity;
}
Product* Sale::getProduct(){
    return this->product;
}
void Sale::setProduct(Product* prod){
    this->product=prod;
}
int Sale::getQuanity(){
    return this->quantity;
}
void Sale::setQuantity(int quantity){
    this->quantity=quantity;
}
