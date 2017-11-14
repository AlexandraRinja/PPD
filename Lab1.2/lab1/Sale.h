#ifndef SAlE_H
#define SAlE_H
#include"Product.h"
class Sale{
private:
    Product* product;
    int quantity;
public:
    Sale(){};
    Sale(Product* prod,int quantity);
    void setQuantity(int quantity);
    void setProduct(Product* prod);
    int getQuanity();
    Product* getProduct();
    friend ostream& operator<<(ostream& os, const Sale& sale){
        os <<"SALE "<<sale.product<< " quantity:" << sale.quantity;
        return os;
    };
};
#endif // SAlE_H


