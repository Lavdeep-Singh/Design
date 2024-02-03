#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <chrono>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>
using namespace std;

/*
We have to design a message queue supporting publisher-subscriber model. It should support following operations:
• It should support multiple topics where messages can be published.
• Publisher should be able to publish a message to a particular topic.
• Subscribers should be able to subscribe to a topic.
• Whenever a message is published to a topic, all the subscribers, who are subscribed to that topic, should receive the message.
• We should be able to reset the offset for a subcriber. This means subscriber would restart reading again from that offset.
  -> It is like replaying the messages.
• Subscribers should be able to run in parallel
*/

/*
different instances will have different memories, hence threads will be operating on different memory not the same one!
Each instance of a class has its own set of member variables, including mutexes if they are defined as member variables. 
Therefore, if you have two instances of a class and each instance has its own mutex, 
operations on one instance will not affect the operations on the other instance.
*/

class Message{
    public:
    string message;

    Message(string pMessage){
      message = pMessage;
    }
};

class Subscriber{
  public:

  virtual string getId() = 0;
  virtual void consume(Message* message) = 0;
};

class SleepingSubscriber : public Subscriber{
  string id;
  int sleepingTimeInMills;

  public:

  SleepingSubscriber(string pId, int mills){
    id = pId;
    sleepingTimeInMills = mills;
  }

  string getId(){
    return id;
  }

  void consume(Message* message){
    // cout<<" subscriber "<<id<<" consuming message "<<endl;
    this_thread::sleep_for(chrono::milliseconds(sleepingTimeInMills));
    // cout<<" subscriber "<<id<<" done consuming "<<endl;
  }
};

class TopicSubscribers {
public:
    atomic<int> offset;
    Subscriber* subscriber;
    mutex mtx;
    condition_variable cv;

    TopicSubscribers(Subscriber* subscriber) {
        this->subscriber = subscriber;
        offset = 0;
    }

    int getOffset() {
        return offset;
    }

    void setOffset(int newValue) {
      offset.store(newValue);
    }

    void wait(std::unique_lock<std::mutex>& lock) {
        cv.wait(lock);
    }

    void notify() {
        cv.notify_one();
    }
};


class Topic{
    string topicName;
    string topicId;
    vector<Message*> messagesList;
    vector<TopicSubscribers*> subscribersList;
    mutex mtx;

    public:

    Topic(string pTopicName, string pTopicId){
      topicName = pTopicName;
      topicId = pTopicId;
    }

    void addMessage(Message* message){
      mtx.lock();
      messagesList.push_back(message);
      mtx.unlock();
    }

    void addSubscriber(TopicSubscribers* topicSubscriber){
      subscribersList.push_back(topicSubscriber);
    }

    vector<Message*> getMessageList(){
      return messagesList;
    }

    vector<TopicSubscribers*> getSubscribersList(){
      return subscribersList;
    }

    string getTopicId(){
      return topicId;
    }

    string getTopicName(){
      return topicName;
    }
};

class SubscriberWorker {
public:
    Topic* topic;
    TopicSubscribers* topicSubscriber;

    SubscriberWorker(Topic* topic, TopicSubscribers* topicSubscriber) {
        this->topic = topic;
        this->topicSubscriber = topicSubscriber;
    }

    void run() {
        do {
            unique_lock<mutex> lock(topicSubscriber->mtx);//aquire lock on mutex
            while (topicSubscriber->getOffset() >= topic->getMessageList().size()) {
                topicSubscriber->wait(lock);//unlock the mutex and wait for notify
                unique_lock<mutex> lock(topicSubscriber->mtx);//aquire lock on mutex
            }
            int curOffset = topicSubscriber->getOffset();
            Message* message = topic->getMessageList()[curOffset];
            topicSubscriber->subscriber->consume(message);
            cout<<"Subscriber "<<topicSubscriber->subscriber->getId()<<" consumed "<<topic->getTopicName()<<" and message "<<message->message<<endl;
            topicSubscriber->offset.compare_exchange_weak(curOffset, curOffset + 1);//compare_exchange_weak(expected, desired) if offset == expected value then only offset = desired else not
            lock.unlock();//unlock the mutex
        } while (true);
    }

    void wakeUpIfNeeded() {
        topicSubscriber->notify();
    }
};

class TopicHandler{
  Topic* topic;
  unordered_map<string, SubscriberWorker*> subscriberWorkers;

  public:

  TopicHandler(Topic* pTopic){
    topic = pTopic;
  }

  void publish(){
    for(TopicSubscribers* topicSubscriber : topic->getSubscribersList()){
      startSubscriberWorker(topicSubscriber);
    }
  }

  void startSubscriberWorker(TopicSubscribers* topicSubscriber) {
    string subscriberId = topicSubscriber->subscriber->getId();
    if (subscriberWorkers.find(subscriberId) == subscriberWorkers.end()) {
        SubscriberWorker* subscriberWorker = new SubscriberWorker(topic, topicSubscriber);
        subscriberWorkers[subscriberId] = subscriberWorker;

        // Use a lambda expression to invoke the run method:
        thread thread3([subscriberWorker]() {//lambda function can't access member variables of this object as this is not passed as parameter
          subscriberWorker->run(); 
        });
        thread3.detach();

        // subscriberWorker->run();
    }
    SubscriberWorker* subscriberWorker = subscriberWorkers[subscriberId];
    subscriberWorker->wakeUpIfNeeded();
  }

};

class Queue{
  unordered_map<string,TopicHandler*> topicProcessors;//{topic->id, topicHandler}
  public:

  Topic* createTopic(string topicName, string id){
    Topic* topic = new Topic(topicName, id);
    TopicHandler* topicHandler = new TopicHandler(topic);
    topicProcessors[topic->getTopicId()] = topicHandler;
    cout<<"Created topic "<<topicName<<endl;
    return topic;
  }

  void subscribe(Subscriber* subscriber, Topic* topic){
    topic->addSubscriber(new TopicSubscribers(subscriber));
    cout<<subscriber->getId()<< " subscribed to " << topic->getTopicName()<<endl;
  }

  void publish(Topic* topic, Message* message){
    topic->addMessage(message);
    cout<<message->message <<" is published to "<< topic->getTopicName()<<endl;

    thread t([this, topic]() {
       topicProcessors[topic->getTopicId()]->publish();
        });
    t.detach();
  }

  void resetOffset(Topic* topic, Subscriber* subscriber, int newOffset){
    for(TopicSubscribers* topicSubscriber : topic->getSubscribersList()){
      if(topicSubscriber->subscriber == subscriber){
        topicSubscriber->setOffset(newOffset);
        cout<<subscriber->getId()<<" offset reset to "<<topicSubscriber->getOffset()<<endl;

        thread t([this, topic, topicSubscriber]() {//lambda function can access member variables of current object as this is passed as parameter
                    topicProcessors[topic->getTopicId()]->startSubscriberWorker(topicSubscriber);
                });
        t.join();

        break;
      }
    }
  }
};


int main() {
        Queue* queue = new Queue();
        Topic* topic1 = queue->createTopic("t1", "uid1");
        Topic* topic2 = queue->createTopic("t2", "uid2");
        SleepingSubscriber* sub1 = new SleepingSubscriber("sub1", 1000);
        SleepingSubscriber* sub2 = new SleepingSubscriber("sub2", 1000);
        queue->subscribe(sub1, topic1);
        queue->subscribe(sub2, topic1);

        SleepingSubscriber* sub3 = new SleepingSubscriber("sub3", 1000);
        queue->subscribe(sub3, topic1);
        queue->publish(topic1, new Message("t1->m1"));
        queue->publish(topic1, new Message("t1->m2"));
        SleepingSubscriber* sub4 = new SleepingSubscriber("sub4", 1000);
        queue->subscribe(sub4, topic2);
        queue->publish(topic2, new Message("t2->m1"));
        queue->publish(topic2, new Message("t2->m2"));
        queue->publish(topic1, new Message("t1->m3"));

        this_thread::sleep_for(std::chrono::milliseconds(1200));

        queue->resetOffset(topic1, sub1, 0);
        queue->resetOffset(topic2, sub4, 0);

        this_thread::sleep_for(std::chrono::milliseconds(25000));
};