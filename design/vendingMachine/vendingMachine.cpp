#include <iostream>
#include <unordered_map>
#include <queue>
using namespace std;

// noCoinInsertedState -> coinInsertedState -> dispenseState -> noCoinInstertedState

class NoCoinInsertedState;
class CoinInsertedState;
class DispenseState;
class State;
class Product;
class Inventory;
class VendingMachine;

class Product{
    string name;
    int id;
    int price;

    public:

    Product(){}

    Product(string pName, int pId, int pPrice){
        name = pName;
        id = pId;
        price = pPrice;
    }

    int getPrice(){
        return price;
    }

    int getId(){
        return id;
    }

};

class Inventory{
    unordered_map<int, Product*> aisleToProduct;
    unordered_map<int, int> productIdToCountMap;
    queue<int> availableAisles;

    public:

    Inventory(int aisleCount){

        for(int i=1; i<=aisleCount; i++){
            availableAisles.push(i);
        }
    }

    void addProduct(Product* product){
        int productId = product->getId();
        if(productIdToCountMap.find(productId)==productIdToCountMap.end()){//new product to be added
            if(availableAisles.empty()){
                cout<<"No space to add product"<<endl;
                return;
            }
            int aisle = availableAisles.front(); availableAisles.pop();
            aisleToProduct[aisle] = product;
        }
        //product already exists
        productIdToCountMap[productId]++;
    }

    Product* getProduct(int aisleNumber){
        return aisleToProduct[aisleNumber];
    }

    bool checkIfProductAvailable(int aisleNumber){
        if(aisleToProduct.find(aisleNumber)!=aisleToProduct.end()){
            int productId = aisleToProduct[aisleNumber]->getId();
            if(productIdToCountMap[productId]<=0){
                return false;
            }
            return true;
        }
        return false;
    }

    Product* getProductAt(int aisleNumber){
        return aisleToProduct[aisleNumber];
    }

    void deductProductCount(int aisleNumber){
        auto product = aisleToProduct[aisleNumber];
        if(!product){
            cout<<"This aisle does't have any product!"<<endl;
            return;
        }
        int productId = product->getId();
        int updatedProductCount = productIdToCountMap[productId]-1;
        if(updatedProductCount<=0){
            productIdToCountMap.erase(productId);
            aisleToProduct.erase(aisleNumber);
            availableAisles.push(aisleNumber);
        }else{
            productIdToCountMap[productId] = updatedProductCount;
        }
    }

};

class State{
    public:
    virtual void insertCoin(int amount) = 0;
    virtual void pressButton(int aisleNumber) = 0;
    virtual void dispense(int aisleNumber) = 0;
};

class NoCoinInsertedState : public State {
    VendingMachine* vendingMachine;

    public:

    NoCoinInsertedState(VendingMachine* pVendingMachine){
        vendingMachine = pVendingMachine;
    }

    void insertCoin(int amount);

    void pressButton(int aisleNumber){
        cout<<"Please insert a coin first!"<<endl;
        return;
    }

    void dispense(int aisleNumber){
        cout<<"No coint is inserted!"<<endl;
        return;
    }
};

class CoinInsertedState : public State{
    VendingMachine* vendingMachine;

    public:

    CoinInsertedState(VendingMachine* pVendingMachine){
        vendingMachine = pVendingMachine;
    }

    void insertCoin(int amount);

    void pressButton(int aisleNumber);

    void dispense(int aisleNumber){
        cout<<"No product is selected"<<endl;
        return;
    }
};

class DispenseState : public State{
    VendingMachine* vendingMachine;

    public:

    DispenseState(VendingMachine* pVendingMachine){
        vendingMachine = pVendingMachine;
    }

    void insertCoin(int amount){
        cout<<"Not accepting coin!, product is being dispensed!"<<endl;
        return;
    }

    void pressButton(int aisleNumber){
        cout<<"Product is being dispensed!"<<endl;
        return;
    }

    void dispense(int aisleNumber);
};

class VendingMachine{
    NoCoinInsertedState* noCoinInsertedState;
    CoinInsertedState* coinInsertedState;
    DispenseState* dispenseState;
    Inventory* inventory;
    State* curVendingMachineState;
    int amount;
    static const int AISLE_COUNT = 2;

    public:

    VendingMachine(){
        noCoinInsertedState = new NoCoinInsertedState(this);
        coinInsertedState = new CoinInsertedState(this);
        dispenseState = new DispenseState(this);
        curVendingMachineState = noCoinInsertedState;
        amount = 0;
        inventory = new Inventory(AISLE_COUNT);
    }

    bool hasSufficientAmount(int pAmount){
        return amount>=pAmount;
    }

    NoCoinInsertedState* getNoCoinInsertedState(){
        return noCoinInsertedState;
    }

    CoinInsertedState* getCoinInsertedState(){
        return coinInsertedState;
    }

    DispenseState* getDispenseState(){
        return dispenseState;
    }

    Inventory* getInventory(){
        return inventory;
    }

    int getAmount(){
        return amount;
    }

    void setAmount(int pAmount){
        amount = pAmount;
    }

    State* getCurrentVendingMachineState(){
        return curVendingMachineState;
    }

    State* setCurrentVendingMachineState(State* pCurVendingMachineState){
        curVendingMachineState = pCurVendingMachineState;
    }

    void insertCoin(int amount){
        curVendingMachineState->insertCoin(amount);
    }

    void pressButton(int aisleNumber){
        curVendingMachineState->pressButton(aisleNumber);
        curVendingMachineState->dispense(aisleNumber);
    }

    void addProduct(Product* product){
        inventory->addProduct(product);
        return;
    }

    void setCurrentState(State* pSate){
        curVendingMachineState = pSate;
    }

    bool isProductAvailable(int aisleNumber){
        return inventory->checkIfProductAvailable(aisleNumber);
    }

    State* getNoCointInsertedState(){
        return noCoinInsertedState;
    }
};

// defined here because of dependency issues

void NoCoinInsertedState :: insertCoin(int amount){
    vendingMachine->setAmount(amount);
    vendingMachine->setCurrentState(vendingMachine->getCoinInsertedState());
}

void CoinInsertedState :: insertCoin(int amount){
    vendingMachine->setAmount(vendingMachine->getAmount() + amount);
}

void CoinInsertedState :: pressButton(int aisleNumber){
    Inventory* inventory = vendingMachine->getInventory();
    Product* product = inventory->getProduct(aisleNumber);

    //check if amount is enough to buy this product
    if(!vendingMachine->hasSufficientAmount(product->getPrice())){
        cout<<"Returning Coins!, Amount not sufficient to buy this product!"<<endl;
        return;
    }

    //check if product is available
    if(!vendingMachine->isProductAvailable(aisleNumber)){
        cout<<"Product is not available"<<endl;
        return;
    }

    vendingMachine->setCurrentState(vendingMachine->getDispenseState());

}

void DispenseState :: dispense(int aisleNumber){
    Inventory* inventory = vendingMachine->getInventory();
    Product* product = inventory->getProduct(aisleNumber);

    inventory->deductProductCount(aisleNumber);
    int change = vendingMachine->getAmount() - product->getPrice();
    vendingMachine->setAmount(0);
    vendingMachine->setCurrentState(vendingMachine->getNoCointInsertedState());

    cout<<"Product is dispensed and change of "<<change<<" Rs is returned!"<<endl;
}

int main(){
    VendingMachine* vendingMachine = new VendingMachine();
    Product* hersheys = new Product ("hersheys" , 1, 5.0);
    //inserting 3 hersheys
    for (int i = 1; i <= 3 ; i ++){
        vendingMachine->addProduct(hersheys);
    }
    Product* biskfarm = new Product("biskfarm" , 2 , 2.0);
    //inserting 3 biskfarm
    for (int i = 1 ; i <=3 ; i ++){
        vendingMachine->addProduct(biskfarm);
    }

    vendingMachine->insertCoin (5.0); vendingMachine->insertCoin (3.0); vendingMachine->pressButton(1);
    vendingMachine->insertCoin (5.0); vendingMachine->pressButton(1);
    vendingMachine->insertCoin(7.0);  vendingMachine->pressButton(2);
}