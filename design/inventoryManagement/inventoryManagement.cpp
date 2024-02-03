#include <iostream>
#include <unordered_map>
// #include <set>
#include <unordered_set>
using namespace std;

class Address{
    int pinCode;
    string city;
    string state;

    public:

    Address(int pinCode, string city, string state){
        this->pinCode = pinCode;
        this->city = city;
        this->state = state;
    }
    //getters & setters methods


    int getPinCode() {
        return pinCode;
    }

    void setPinCode(int pinCode) {
        this->pinCode = pinCode;
    }

    string getCity() {
        return city;
    }

    void setCity(string city) {
        this->city = city;
    }

    string getState() {
        return state;
    }

    void setState(string state) {
        this->state = state;
    }

};


class Product{
    public:
    string productName;
    int productId;
};

//having list of same product
//simulate when you order 10 amul milk pouches, but in cart only count incraeses
class ProductCategory{
    public:
    int productCategoryId;
    string categoryName;
    vector<Product*> products;
    double price;

    ProductCategory(int id, string name, double pPrice){
        productCategoryId = id;
        categoryName = name;
        price = pPrice;
    }

    void addProduct(Product* product){
        products.push_back(product);
    }

    void removeProduct(int count){
        while(!products.empty() and count--){
            products.pop_back();
        }
    }

    int getProductCategoryId(){
        return productCategoryId;
    }
};

class Inventory{

    public:

    vector<ProductCategory*> productCategoryList;

    void addCategory(int categoryId, string name, double price){
        ProductCategory* productCategory = new ProductCategory(categoryId, name, price);
        productCategoryList.push_back(productCategory);
    }

    void addProduct(Product* product, int productCategoryId){
        for(ProductCategory* pc : productCategoryList){
            if(pc->getProductCategoryId() == productCategoryId){
                pc->addProduct(product);
            }
        }
    }

    ProductCategory* getProductCategoryFromID(int productCategoryId){

        for(ProductCategory* productCategory : productCategoryList){

            if(productCategory->getProductCategoryId() == productCategoryId){
                return productCategory;
            }
        }

        return NULL;
    }


    void removeProduct(unordered_map<int,int> productCategoryAndCountMap){
        for(auto entry : productCategoryAndCountMap){
            int productCategoryId = entry.first;
            int numberOfProductsToRemove = entry.second;

            ProductCategory* productCategory = getProductCategoryFromID(productCategoryId);
            productCategory->removeProduct(numberOfProductsToRemove);
        }
    }
};

class PaymentMode{
    public:

    virtual bool makePayment() = 0;
};

class CardPaymentMode : public PaymentMode{
    public:
    bool makePayment() {

        return true;
    }
};

class Cart {

    unordered_map<int, int> productCategoryIdVsCountMap;

    public:

    //add item to cart
    void addItemInCart(int productCategoryId, int count){

        if(productCategoryIdVsCountMap.find(productCategoryId)!=productCategoryIdVsCountMap.end()){
            int noOfItemsInCart = productCategoryIdVsCountMap[productCategoryId];
            productCategoryIdVsCountMap[productCategoryId] = noOfItemsInCart + count;
        } else{
            productCategoryIdVsCountMap[productCategoryId] = count;
        }
    }


    //remove item to cart
    void removeItemFromCart(int productCategoryId, int count) {

        if (productCategoryIdVsCountMap.find(productCategoryId)!=productCategoryIdVsCountMap.end())
        {
            int noOfItemsInCart = productCategoryIdVsCountMap[productCategoryId];
            if (count - noOfItemsInCart == 0) {
                productCategoryIdVsCountMap.erase(productCategoryId);
            } else {
                productCategoryIdVsCountMap[productCategoryId] = noOfItemsInCart - count;
            }
        }
    }


    //empty my cart
    void emptyCart(){
        productCategoryIdVsCountMap.clear();
    }

    //View Cart
    unordered_map<int, int> getCartItems(){
        return productCategoryIdVsCountMap;
    }
};

class User {
    public:

    string userName;
    Address* address;
    Cart* userCartDetails;
    vector<int> orderIds;

    int userId;

    //get UserCart
    Cart* getUserCart(){
        return userCartDetails;
    }
};

class UserController {

    unordered_set<User*> userList;

    public:

    UserController(vector<User*> userList){
        for(int i=0; i<userList.size(); i++){
            this->userList.insert(userList[i]);
        }
    }

    //add user
    void addUser(User* user){
        userList.insert(user);
    }

    //remove user
    void removeUser(User* user){
        userList.erase(user);
    }

    //get particular user
    User* getUser(int userId){
        for(User* user : userList){
            if(user->userId == userId){
                return user;
            }
        }
        return NULL;
    }
};

class Warehouse {
    public:

    Inventory* inventory;
    Address* address;

    //update inventory
    void removeItemFromInventory(unordered_map<int, int> productCategoryAndCountMap){

        //it will update the items in the inventory based upon product category.
        inventory->removeProduct(productCategoryAndCountMap);
    }

    void addItemToInventory(unordered_map<int, int> productCategoryAndCountMap){

        //it will update the items in the inventory based upon product category.
    }
};

class WarehouseController {

    unordered_set<Warehouse*> warehouseList;
    WarehouseSelectionStrategy* warehouseSelectionStrategy;

    public:

    WarehouseController(vector<Warehouse*> warehouseList, WarehouseSelectionStrategy* warehouseSelectionStrategy){
        for(auto wl : warehouseList){
            this->warehouseList.insert(wl);
        }
        this->warehouseSelectionStrategy = warehouseSelectionStrategy;
    }

    //add new warehouse
    void addNewWarehouse(Warehouse* warehouse){
        warehouseList.insert(warehouse);
    }

    //remove warehouse
    void removeWarehouse(Warehouse* warehouse){
        warehouseList.erase(warehouse);
    }

    Warehouse* selectWarehouse(WarehouseSelectionStrategy* selectionStrategy){
        this->warehouseSelectionStrategy = selectionStrategy;
        return warehouseSelectionStrategy->selectWarehouse(warehouseList);
    }
};



class Order {

    User* user;
    Address* deliveryAddress;
    unordered_map<int, int> productCategoryAndCountMap;
    Warehouse* warehouse;
    Invoice* invoice;
    Payment* payment;
    OrderStatus orderStatus;

    public:

    Order(User* user, Warehouse* warehouse){
      this->user = user;
      this->productCategoryAndCountMap = user->getUserCart()->getCartItems();
      this->warehouse = warehouse;
      this->deliveryAddress = user->address;
      invoice = new Invoice();
      invoice->generateInvoice(this);
    }

    void checkout(){

        //1. update inventory
        warehouse->removeItemFromInventory(productCategoryAndCountMap);

        //2. make Payment
        bool isPaymentSuccess = makePayment(new UPIPaymentMode());

        //3. make cart empty
        if(isPaymentSuccess) {
            user->getUserCart()->emptyCart();
        }
        else{
            warehouse->addItemToInventory(productCategoryAndCountMap);
        }

    }

    bool makePayment(PaymentMode* paymentMode){
        payment = new Payment(paymentMode);
       return payment->makePayment();
    }

    void generateOrderInvoice(){
        invoice->generateInvoice(this);
    }
};



class Invoice {

    int totalItemPrice;
    int totalTax;
    int totalFinalPrice;

    public:

    //generate Invoice
    void generateInvoice(Order* order){

        //it will compute and update the above details
        totalItemPrice = 200;
        totalTax = 20;
        totalFinalPrice = 220;
    }
};

class WarehouseSelectionStrategy{
    public:

    virtual Warehouse* selectWarehouse(unordered_set<Warehouse*> warehouseList) = 0;
};

class NearestWarehouseSelectionStrategy : public WarehouseSelectionStrategy{

    public:

    Warehouse* selectWarehouse(unordered_set<Warehouse*> warehouseList) {
       //algo to pick the nearest algo, for now I am just picking the first warehouse for demo purpose
        return *warehouseList.begin();
    }
};


enum class OrderStatus {

    DELIVERED,
    CANCELLED,
    RETURNED,
    UNDELIVERED
};


class OrderController {

    vector<Order*> orderList;
    unordered_map<int, vector<Order*>> userIDVsOrders;

    public:

    OrderController(){
    }

    //create New Order
    Order* createNewOrder(User* user, Warehouse* warehouse){
        Order* order = new Order(user, warehouse);
        orderList.push_back(order);

        if(userIDVsOrders.find(user->userId)!=userIDVsOrders.end()){
            vector<Order*> userOrders = userIDVsOrders[user->userId];
            userOrders.push_back(order);
            userIDVsOrders[user->userId] =  userOrders;
        } else {
            vector<Order*> userOrders;
            userOrders.push_back(order);
            userIDVsOrders[user->userId] = userOrders;

        }
        return order;
    }


    //remove order
    void removeOrder(Order* order){

        //remove order capability goes here
    }

    vector<Order*> getOrderByCustomerId(int userId){
        //implement this
        vector<Order*> orders;
        return orders;
    }

    Order* getOrderByOrderId(int orderId){
        return NULL;
    }
};


class Payment {

   PaymentMode* paymentMode;

   public:

   Payment(PaymentMode* paymentMode){
       this->paymentMode = paymentMode;
   }

    bool makePayment(){
        return paymentMode->makePayment();
    }
};

class UPIPaymentMode : public PaymentMode{
    bool makePayment() {
        return true;
    }
};



class ProductDeliverySystem {

    UserController* userController;
    WarehouseController* warehouseController;
    OrderController* orderController;

    public:

    ProductDeliverySystem(vector<User*> userList, vector<Warehouse*> warehouseList){
        userController = new UserController(userList);
        warehouseController = new WarehouseController(warehouseList , NULL);
        orderController = new OrderController();
    }

    //get user object
    User* getUser(int userId){
        return userController->getUser(userId);
    }

    //get warehouse
    Warehouse* getWarehouse(WarehouseSelectionStrategy* warehouseSelectionStrategy){
        return warehouseController->selectWarehouse(warehouseSelectionStrategy);

    }

    //get inventory
    Inventory* getInventory(Warehouse* warehouse){
        return warehouse->inventory;

    }

    //add product to cart
    void addProductToCart(User* user, ProductCategory* product, int count){
        Cart* cart = user->getUserCart();
        cart->addItemInCart(product->productCategoryId, count);
    }

    //place order
    Order* placeOrder(User* user, Warehouse* warehouse){
        return orderController->createNewOrder(user, warehouse);
    }

    void checkout(Order* order){
        order->checkout();
    }

};

//---------------------client code--------------------------

Warehouse* addWarehouseAndItsInventory(){

        Warehouse* warehouse = new Warehouse();

        Inventory* inventory = new Inventory();

        inventory->addCategory(0001, "Peppsii Large Cold Drink" , 100);
        inventory->addCategory(0004, "Doovee small Soap" , 50);

        //CREATE 3 Products

        Product* product1 = new Product();
        product1->productId = 1;
        product1->productName = "Peepsii";

        Product* product2 = new Product();
        product2->productId = 2;
        product2->productName = "Peepsii";

        Product* product3 = new Product();
        product1->productId = 3;
        product1->productName = "Doovee";


        inventory->addProduct(product1, 0001);
        inventory->addProduct(product2, 0001);
        inventory->addProduct(product3, 0004);

        warehouse->inventory = inventory;
        return warehouse;
    }

void runDeliveryFlow(ProductDeliverySystem* productDeliverySystem, int userId){


    //1. Get the user object
    User* user = productDeliverySystem->getUser(userId);

    //2. get warehouse based on user preference
    Warehouse* warehouse = productDeliverySystem->getWarehouse(new NearestWarehouseSelectionStrategy());

    //3. get all the inventory to show the user
    Inventory* inventory = productDeliverySystem->getInventory(warehouse);

    ProductCategory* productCategoryIWantToOrder = NULL;
    for(ProductCategory* productCategory : inventory->productCategoryList){

        if(productCategory->categoryName == ("Peppsii Large Cold Drink")){
            productCategoryIWantToOrder = productCategory;
        }
    }


    //4. add product to the cart
    productDeliverySystem->addProductToCart(user, productCategoryIWantToOrder, 2);


    //4. place order
    Order* order = productDeliverySystem->placeOrder(user, warehouse);


    //5. checkout
    productDeliverySystem->checkout(order);
}

int main() {

        //1. create warehouses in the system
        vector<Warehouse*> warehouseList;
        warehouseList.push_back(addWarehouseAndItsInventory());

        //2. create users in the system
        vector<User*> userList;
        User* user = new User();
        User* user = new User();
        user->userId = 1;
        user->userName = "SJ";
        user->address = new Address(230011, "city", "state");
        userList.push_back(user);

        //3. feed the system with the initial informations
        ProductDeliverySystem* productDeliverySystem = new ProductDeliverySystem(userList, warehouseList);


        runDeliveryFlow(productDeliverySystem, 1);
    }
