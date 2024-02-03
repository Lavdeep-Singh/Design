
using namespace std;

//user defined type of xml data and getter
class XmlData{
    string xmlData;
public:
    XmlData(string pXmlData){
        xmlData = pXmlData;
    }
    string getXmlData(){
        return xmlData;
    }
};

class DataAnalyticsTool{
    string jsonData;

    public:

    DataAnalyticsTool(string pjJsonData){
        jsonData = pjJsonData;
    }
    virtual void analyseData(){
        cout<<"Analysing Data "<<jsonData<<endl;
    }
};

//adapter that joind DataAnalyticsTool(accepts json as i/p) and xmlData
class Adapter : public DataAnalyticsTool{
    XmlData* xmlData;
public:
    Adapter(XmlData* pXmlData) : DataAnalyticsTool(pXmlData->getXmlData()), xmlData(pXmlData) {}

    void analyseData() override{
        cout<<"Converting Data to JSON and forwading it to analyze the data"<<endl;
        DataAnalyticsTool :: analyseData();
    }
};

class Client{
    public:
    void processData(DataAnalyticsTool* tool){
        tool->analyseData();
    }
};

int main(){
    XmlData* xmlData = new XmlData("This is a sample Xml Data");
    DataAnalyticsTool* tool = new Adapter(xmlData);
    Client* client = new Client;
    client->processData(tool);
    return 0;
}