//
// Created by Tong on 3/18/22.
//

#include "Server.h"
/*
std::string Server::getXmlContent(const std::string& raw){
    size_t content_start = 0;
    content_start = raw.find('\n',raw.find('\n')+1) +1 ;
    std::string rest = raw.substr(content_start,raw.length() - content_start);
    return rest;
}
*/
const char * Server::getXmlContent(const char* raw){
    const char* content_start;
    const char space[] = "\n";
    content_start = strstr(strstr(raw,space)+ 1,space);
    return content_start+1;
    //stringstream
}

void Server::handleAccountCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement){
    //current root is account create
    //<account id="ACCOUNT_ID" balance="BALANCE"/> #0 or more
    TiXmlAttribute *pAttr = rootElement->FirstAttribute();//第一个属性
    int id = std::atoi(pAttr->Value());
    pAttr = pAttr->Next();
    int balance = std::atoi(pAttr->Value());
    if(db.hasAccount(id)){
        //<error id="ACCOUNT_ID">Msg</error> #For account create error
        TiXmlElement *newChildElement = new TiXmlElement("error");//根元素
        newChildElement->SetAttribute("id", id); //属性
        newChildElement->LinkEndChild(new TiXmlText("Account Already exits"));
        rootResultElement->LinkEndChild(newChildElement);
    }else{
        //<created id="ACCOUNT_ID"/> #For account create
        db.saveAccount(id, balance);
        TiXmlElement *newChildElement = new TiXmlElement("created");//根元素
        newChildElement->SetAttribute("id", id); //属性
        rootResultElement->LinkEndChild(newChildElement);
    }
}
void Server::handleSymbolCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement){
    //current root is symbol create
}
void Server::handleCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement){
    //Handle Create
    for (TiXmlNode *SubItem = rootElement->FirstChild(); SubItem != nullptr; SubItem = SubItem->NextSibling()) {
        std::cout << "!!!current Node " <<SubItem->Value() << ": "<<std::endl;

        //TODO: check whether can be transfer to element or not
        TiXmlElement *createdElement = SubItem->ToElement();
        // if just a child node not element then return
        if (strcmp(createdElement ->Value(), "account") == 0){
            std::cout << "acount Node " <<SubItem->Value() << ": "<<std::endl;
            handleAccountCreate(createdElement, rootResultElement);
        }else if (strcmp(createdElement ->Value(), "symbol") == 0){
            std::cout << "sym Node " <<SubItem->Value() << ": "<<std::endl;
            handleSymbolCreate(createdElement, rootResultElement);
        }else{
            return;
        }
    }
}
void Server::handleTransection(TiXmlElement* rootElement, TiXmlElement* rootResultElement){
    //Hanle Transction
}
// handle all type of request
void Server::handleRequest(TiXmlElement* rootElement, TiXmlElement* rootResultElement){
    if (rootElement == nullptr) return;
    if (strcmp(rootElement->Value() , "create") == 0 ){
        handleCreate(rootElement, rootResultElement);
    } else if (strcmp(rootElement->Value() , "transection") == 0){
        handleTransection(rootElement, rootResultElement);
    } else{
        // Exception
        return;
    }
}


void printXml(TiXmlElement* rootElement, bool isElement) {
    if (rootElement == nullptr) return;
    std::cout <<"current root " <<rootElement->Value() << std::endl;
    for (TiXmlNode *SubItem = rootElement->FirstChild(); SubItem != nullptr;) {
        std::cout << "current Node " <<SubItem->Value() << ": "<<std::endl;
        // if just a child node not element then return
        if(!isElement) return;
        TiXmlElement *createdElement = SubItem->ToElement();

        TiXmlAttribute *pAttr = createdElement->FirstAttribute();//第一个属性

        while (nullptr != pAttr) //输出所有属性
        {
            std::cout << pAttr->Name() << ": " << pAttr->Value() << " ";
            pAttr = pAttr->Next();
        }
        std::cout << std::endl;

        TiXmlNode *sonNode = createdElement->FirstChild();
        TiXmlNode *sonElement = createdElement->FirstChildElement();
        if (sonNode) {
            bool isSonElement = (sonElement != nullptr);
            printXml(createdElement, isSonElement);
        }
        SubItem = SubItem->NextSibling();
    }
}

int main(int argc, char *argv[]) {
    std::cout << "server running\n";
    Socket socket;
    int listen_fd = socket.setupServer(PORT);
    int msg_fd = socket.acceptConn(listen_fd);
    std::vector<char> request = socket.recvMesg(msg_fd);
    std::cout << request.data() << '\n';
//    std::string response = "127\n"
//                           "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
//                           "<results>\n"
//                           "    <created id=\"123456\"/>\n"
//                           "    <created sym=\"SPY\" id=\"123456\"/>\n"
//                           "</results>";

    //目前是print 所有的值，之后应该会用map存

    Server server;
    TiXmlDocument* myDocument = new TiXmlDocument();
    myDocument->Parse(server.getXmlContent(request.data()));
//    myDocument->Parse(request.data());
    std::cout << "Parse Complete" << '\n';
    TiXmlElement* rootElement = myDocument->RootElement();
    //Create Result document
    TiXmlDocument* resDocument = new TiXmlDocument();
    //Format Declare
    //TODO: check declaration format
    TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "UTF-8", "yes");
    resDocument->LinkEndChild(decl); //写入文档
    //<results>
    TiXmlElement* rootResultElement = new TiXmlElement("results");
    resDocument->LinkEndChild(rootResultElement);
    server.handleRequest(rootElement, rootResultElement);
    TiXmlPrinter *printer = new TiXmlPrinter();
    resDocument->Accept(printer);
    std::string stringBuffer= printer->CStr();
    std::string response = std::to_string(stringBuffer.length());
    response.append("\n");
    response.append(stringBuffer);
//    printXml(rootElement, true);

    socket.sendMesg(msg_fd, response);

    socket.closeConn(listen_fd);
    socket.closeConn(msg_fd);
    return EXIT_SUCCESS;
}