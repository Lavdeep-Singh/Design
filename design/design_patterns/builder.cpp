#include <iostream>

using namespace std;

//product class
class Desktop{
    string keyboard;
    string monitor;
    string mouse;
    string ram;
    string processor;

public:
    void setKeyboard(string pKeyboard){
        keyboard = pKeyboard;
    }
    void setMonitor(string pMonitor){
        monitor = pMonitor;
    }
    void setMouse(string pMouse){
        mouse = pMouse;
    }
    void setRam(string pRam){
        ram = pRam;
    }
    void setProcessor(string pProcessor){
        processor = pProcessor;
    }

    void showSpecs(){
        cout<<" Monitor is "<<monitor<<" keyboard is "<<keyboard<<" mouse is "<<mouse<<" ram is "<<ram<<" processor is "<<processor<<endl;
    }
};

//builder class
class DesktopBuilder{
protected:
    Desktop* desktop;
public:
    DesktopBuilder(){
        desktop = new Desktop();
    }
    virtual void buildMonitor() = 0;
    virtual void buildKeyboard() = 0;
    virtual void buildMouse() = 0;
    virtual void buildRam() = 0;
    virtual void buildProcessor() = 0;
    virtual Desktop* getDesktop(){
        return desktop;
    }
};

class DellDesktopBuilder : public DesktopBuilder{

    void buildMonitor(){
        desktop->setMonitor("Dell Monitor");
    }

    void buildKeyboard(){
        desktop->setKeyboard("Dell Keyboard");
    }

    void buildMouse(){
        desktop->setMouse("Dell Mouse");
    }

    void buildRam(){
        desktop->setRam("Dell Ram");
    }

    void buildProcessor(){
        desktop->setProcessor("Dell Processor");
    }

};

class HpDesktopBuilder : public DesktopBuilder{

    void buildMonitor(){
        desktop->setMonitor("Hp Monitor");
    }

    void buildKeyboard(){
        desktop->setKeyboard("Hp Keyboard");
    }

    void buildMouse(){
        desktop->setMouse("Hp Mouse");
    }

    void buildRam(){
        desktop->setRam("Hp Ram");
    }

    void buildProcessor(){
        desktop->setProcessor("Hp Processor");
    }

};

//Director class -> uses builder class to build the prodcut

class DesktopDirector{
private:
    DesktopBuilder* desktopBuilder;
public:
    DesktopDirector(DesktopBuilder* pDesktopBuilder){
        desktopBuilder = pDesktopBuilder;
    }
    Desktop* getDesktop(){
        return desktopBuilder->getDesktop();
    }
    Desktop* builDesktop(){
        desktopBuilder->buildMonitor();
        desktopBuilder->buildKeyboard();
        desktopBuilder->buildMouse();
        desktopBuilder->buildRam();
        desktopBuilder->buildProcessor();
        return desktopBuilder->getDesktop();
    }
};

int main(){
    HpDesktopBuilder* hpDesktopBuilder = new HpDesktopBuilder();
    DellDesktopBuilder* dellDesktopBuilder = new DellDesktopBuilder();

    DesktopDirector* director1 = new DesktopDirector(hpDesktopBuilder);
    DesktopDirector* director2 = new DesktopDirector(dellDesktopBuilder);

    Desktop* desk1 = director1 -> builDesktop();
    Desktop* desk2 = director2 -> builDesktop();

    desk1->showSpecs();
    desk2->showSpecs();
    return 0;
}