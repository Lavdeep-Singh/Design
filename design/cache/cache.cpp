#include <iostream>
#include <unordered_map>
using namespace std;

//Interface Storage
class Storage{
    protected:
    int storageCapacity;
    public:

    virtual bool add(string key, string value) = 0;

    virtual void remove(string key) = 0;

    virtual string get(string key) = 0;

    virtual double getCurrentUsage() = 0;

    virtual int getCapacity() = 0;
};

// int Storage::storageCapacity = 0;

//Storage Implementation using unordered_map, one more implementation can be using vector data structure
class MapStorage : public Storage{
    unordered_map<string, string> mp;
    // int storageCapacity;
    public:

    MapStorage(int pCapacity){
        storageCapacity = pCapacity;
    }

    bool add(string key, string value){
        if(mp.size()>= storageCapacity){
            cout<<"Storage Limit Reached"<<endl;
            return false;
        }
        mp[key] = value;
        return true;
    }

    void remove(string key){
        if(mp.find(key)==mp.end()){
            return;
        }
        mp.erase(key);
    }

    string get(string key){
        if(mp.find(key)!=mp.end()){
            return mp[key];
        }
        return "";
    }

    double getCurrentUsage(){
        return ((double) mp.size()/(double)storageCapacity);
    }

    int getCapacity(){
        return storageCapacity;
    }
};

class Node{
    public:
    string value;
    Node* prev;
    Node* next;

    Node(string pValue){
        value = pValue;
        prev = NULL;
        next = NULL;
    }

};

class NodeList{
    public:
    Node* head;
    Node* tail;

    NodeList(){
        head = new Node("head");
        tail = new Node("tail");
        head->next = tail;
        tail->prev = head;
    }

    void insertNode(Node* node){
        if(!head){
            head->next = node;
            node->prev = head;
            node->next = tail;
            tail->prev = node;
        }else{
            tail->prev->next = node;
            node->prev = tail->prev;
            node->next = tail;
            tail->prev = node;
        }
    }

    void detachNode(Node* node){
        //detach node
        node->prev->next = node->next;
        node->next->prev = node->prev;
        node->next = NULL;
        node->prev = NULL;
    }

    void attachNodeToLast(Node* node){
        //attach node to last
        NodeList:: tail->prev->next = node;
        node->prev = NodeList::tail->prev;
        node->next = NodeList::tail;
        NodeList::tail->prev = node;
    }

    Node* removeHeadOfDll(){
        //remove head of dll
        Node* n = NULL;
        if(NodeList::head -> next == NodeList::tail){
            return n;
        }else{
            n = NodeList::head -> next;
            NodeList::head -> next = NodeList::head->next->next;
            NodeList::head->next->prev = NodeList::head;
        }
        return n;
    }
};

//Interface for Eviction policy, can use different algorithms for eviction policy
class EvictionPolicy{
    protected:

    EvictionPolicy(){
    }
    public:
    virtual Node* evictKey() = 0;
    virtual void keyAccessed(string key) = 0;
};

class LruEvictionPolicy : public EvictionPolicy{
    public:
    unordered_map<string,Node*> mapper;
    NodeList* nodeList;

    LruEvictionPolicy(){
        nodeList = new NodeList();
    }

    void removeKeyFromMapper(string key){
        mapper.erase(key);
    }

    void keyAccessed(string key){
        Node* node = NULL;
        if(mapper.find(key)!=mapper.end()){
            node = mapper[key];
            nodeList->detachNode(node);
            nodeList->attachNodeToLast(node);
        }else{
            Node* node = new Node(key);
            nodeList->insertNode(node);
            mapper[key]=node;
        }
    }

    Node* evictKey(){
        Node* node = nodeList->removeHeadOfDll();
        if(node){
            removeKeyFromMapper(node->value);
        }
        return node;
    }
};

class Cache{
    Storage* storage;
    EvictionPolicy* evictionPolicy;

    public:

    Cache(Storage* pStorage, EvictionPolicy* pEvictionPolicy){
        storage = pStorage;
        evictionPolicy = pEvictionPolicy;
    }

    void put(string key, string value){
        cout<<"putting key "<<key<<endl;
        bool isSuccess = storage->add(key, value);
        if(!isSuccess){
            Node* evictedNode = evictionPolicy->evictKey();
            if(evictedNode){
                cout<<"removed "<<evictedNode->value<<endl;
                storage->remove(evictedNode->value);
            }
            storage->add(key, value);
        }
        evictionPolicy->keyAccessed(key);
    }
    string get(string key){
        string value = storage->get(key);
        evictionPolicy->keyAccessed(key);
        //will return default value when key is not found in cache
        cout<<"For key "<<key<<" Value is "<<value<<endl;
        return value;
    }

    double getCurrentUsage(){
        return storage->getCurrentUsage();
    }

    int getCapacity(){
        return storage->getCapacity();
    }
};

// int main(){
//     Storage* s = new MapStorage(2);
//     EvictionPolicy* e = new LruEvictionPolicy();

//     Cache* c = new Cache(s,e);

//     c->put("halwa","1");
//     c->put("pudi","2");
//     c->put("raita","4");
//     c->put("dhaniya","100");
//     c->get("dhaniya");
//     c->get("halwa");
//     c->get("laddu");
//     c->put("peda","0");
//     c->get("peda");

// }