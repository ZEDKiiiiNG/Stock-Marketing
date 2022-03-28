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
    double balance = std::atof(pAttr->Value());
    try {
        db.saveAccount(id, balance);
        TiXmlElement *newChildElement = new TiXmlElement("created");//根元素
        newChildElement->SetAttribute("id", id); //属性
        rootResultElement->LinkEndChild(newChildElement);
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        //<error id="ACCOUNT_ID">Msg</error> #For account create error
        TiXmlElement *newChildElement = new TiXmlElement("error");//根元素
        newChildElement->SetAttribute("id", id); //属性
        newChildElement->LinkEndChild(new TiXmlText(e.what()));
        rootResultElement->LinkEndChild(newChildElement);
    }
}
void Server::handleSymbolCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement){
    //current root is symbol create
    /*<symbol sym="SPY">
        <account id="123456">100000</account>
    </symbol>*/
    TiXmlAttribute *pAttr = rootElement->FirstAttribute();//第一个属性 which is sym
    const char *sym = pAttr->Value();
    for(TiXmlElement *accountElement = rootElement->FirstChildElement();accountElement != nullptr; accountElement = accountElement->NextSiblingElement() ){
//        TiXmlElement *accountElement = rootElement->FirstChildElement();
        TiXmlAttribute *aAttr = accountElement->FirstAttribute();//第一个属性 which is id
        int id = std::atoi(aAttr->Value());
        double amount = std::atof(accountElement->FirstChild()->Value());
        //check existance
        try {
            //<created sym="SYM" id="ACCOUNT_ID"/>
            db.updatePosition(sym, id, amount);
            TiXmlElement *newChildElement = new TiXmlElement("created");//根元素
            newChildElement->SetAttribute("sym", sym); //属性sym
            newChildElement->SetAttribute("id", id); //属性id
            rootResultElement->LinkEndChild(newChildElement);
        } catch (std::invalid_argument & e) {
            //<error sym="SYM" id="ACCOUNT_ID">Msg</error>
            TiXmlElement *newChildElement = new TiXmlElement("error");//根元素
            newChildElement->SetAttribute("sym", sym); //属性sym
            newChildElement->SetAttribute("id", id); //属性id
            newChildElement->LinkEndChild(new TiXmlText(e.what()));
            rootResultElement->LinkEndChild(newChildElement);
        }
    }
}
void Server::handleCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement){
    //Handle Create
    for (TiXmlNode *SubItem = rootElement->FirstChild(); SubItem != nullptr; SubItem = SubItem->NextSibling()) {
        std::cout << "!!!current Node " <<SubItem->Value() << ": "<<std::endl;

        //TODO: check whether can be transfer to element or not
        TiXmlElement *createdElement = SubItem->ToElement();
        // if just a child node not element then return
        if (strcmp(createdElement ->Value(), "account") == 0){
//            std::cout << "acount Node " <<SubItem->Value() << ": "<<std::endl;
            handleAccountCreate(createdElement, rootResultElement);
        }else if (strcmp(createdElement ->Value(), "symbol") == 0){
//            std::cout << "sym Node " <<SubItem->Value() << ": "<<std::endl;
            handleSymbolCreate(createdElement, rootResultElement);
        }else{
            return;
        }
    }
}
void Server::handleOrderTransection(TiXmlElement* rootElement, TiXmlElement* rootResultElement, int accountId){
    //handle Order Transection
    //<order sym="SYM" amount="AMT" limit="LMT"/>
    TiXmlAttribute *pAttr = rootElement->FirstAttribute();//第一个属性
    const char * sym = pAttr->Value();
    pAttr = pAttr->Next();
    double amount = std::atof(pAttr->Value());
    pAttr = pAttr->Next();
    double limit = std::atof(pAttr->Value());
    std::string limitString = std::to_string(limit);
    std::string amountString = std::to_string(amount);
    try {
        //<opened sym="SYM" amount="AMT" limit="LMT" id="TRANS_ID"/>
        orderId++;
        db.saveOrder( orderId, sym, accountId,  amount,  limit);
        TiXmlElement *newChildElement = new TiXmlElement("opened");//根元素
        newChildElement->SetAttribute("sym", sym); //属性
        newChildElement->SetAttribute("amount", amountString.c_str()); //属性
        newChildElement->SetAttribute("limit", limitString.c_str()); //属性
        newChildElement->SetAttribute("id", orderId); //属性
        rootResultElement->LinkEndChild(newChildElement);
    } catch (std::invalid_argument & e) {
        //<error sym="SYM" amount="AMT" limit="LMT">Message</error>
        TiXmlElement *newChildElement = new TiXmlElement("error");//根元素
        newChildElement->SetAttribute("sym", sym); //属性
        newChildElement->SetAttribute("amount", amount); //属性
        newChildElement->SetAttribute("limit", limit); //属性
        newChildElement->LinkEndChild(new TiXmlText(e.what()));
        rootResultElement->LinkEndChild(newChildElement);
    }

}
void Server::handleQueryTransection(TiXmlElement* rootElement, TiXmlElement* rootResultElement, int accountId){
    //handle Query Transection
    //<query id="TRANS_ID">
    TiXmlAttribute *pAttr = rootElement->FirstAttribute();//第一个属性
    int transId = std::atoi(pAttr->Value());
    try{
        /*
         <status id="TRANS_ID">
             <open shares=.../>
             <canceled shares=... time=.../>
             <executed shares=... price=... time=.../>
         </status>
         */
        //TODO: database cancel
        pqxx::result r = db.getOrder(transId, accountId);
        TiXmlElement *newChildElement = new TiXmlElement("status");//根元素
        newChildElement->SetAttribute("id", transId); //属性
        for(pqxx::result::const_iterator c = r.begin(); c != r.end(); c++){
//            std::string statusString = c[4].as<std::string>();
            const char *status = c[4].as<std::string>().c_str();
            TiXmlElement *newGrandChildElement = new TiXmlElement(status);//根元素
            newGrandChildElement->SetAttribute("shares", c[2].as<double>());
            if(std::strcmp(status, "executed") == 0){
                newGrandChildElement->SetAttribute("price", c[6].as<double>());
            }
            if(std::strcmp(status, "executed") == 0 || std::strcmp(status, "cancelled") == 0){
                newGrandChildElement->SetAttribute("time", c[5].as<int>());
            }
            newChildElement ->LinkEndChild(newGrandChildElement);
        }
        rootResultElement->LinkEndChild(newChildElement);
    }catch (std::invalid_argument & e){
        //<error sym="SYM" amount="AMT" limit="LMT">Message</error>
        TiXmlElement *newChildElement = new TiXmlElement("error");//根元素
        newChildElement->SetAttribute("id", transId); //属性
        newChildElement->LinkEndChild(new TiXmlText(e.what()));
        rootResultElement->LinkEndChild(newChildElement);
    }
}
void Server::handleCancelTransection(TiXmlElement* rootElement, TiXmlElement* rootResultElement, int accountId){
    //handle Cancel Transection
    //<cancel id="TRANS_ID">
    TiXmlAttribute *pAttr = rootElement->FirstAttribute();//第一个属性
    int transId = std::atoi(pAttr->Value());
    try{
        /*
         <canceled id="TRANS_ID">
             <canceled shares=... time=.../>
             <executed shares=... price=... time=.../>
         </canceled>
         */
        //TODO: database cancel
        pqxx::result r = db.cancelOrder(transId, accountId);
        TiXmlElement *newChildElement = new TiXmlElement("canceled");//根元素
        newChildElement->SetAttribute("id", transId); //属性
        for(pqxx::result::const_iterator c = r.begin(); c != r.end(); c++){
//            std::string statusString = c[4].as<std::string>();
            const char *status = c[4].as<std::string>().c_str();
            TiXmlElement *newGrandChildElement = new TiXmlElement(status);//根元素
            newGrandChildElement->SetAttribute("shares", c[2].as<double>());
            if(std::strcmp(status, "executed") == 0){
                newGrandChildElement->SetAttribute("price", c[6].as<double>());
            }
            if(std::strcmp(status, "executed") == 0 || std::strcmp(status, "cancelled") == 0){
                newGrandChildElement->SetAttribute("time", c[5].as<int>());
            }
            newChildElement ->LinkEndChild(newGrandChildElement);
        }
        rootResultElement->LinkEndChild(newChildElement);
    }catch (std::invalid_argument & e){
        //<error sym="SYM" amount="AMT" limit="LMT">Message</error>
        TiXmlElement *newChildElement = new TiXmlElement("error");//根元素
        newChildElement->SetAttribute("id", transId); //属性
        newChildElement->LinkEndChild(new TiXmlText(e.what()));
        rootResultElement->LinkEndChild(newChildElement);
    }
}

void Server::handleTransection(TiXmlElement* rootElement, TiXmlElement* rootResultElement){
    //Hanle Transction
    /*<transactions id="ACCOUNT_ID"> #contains 1 or more of the below children
         <order sym="SYM" amount="AMT" limit="LMT"/>
         <query id="TRANS_ID">
         <cancel id="TRANS_ID">
        </transactions>
     * */
    std::cout<< rootElement->Value()<< ":"<<rootElement->FirstAttribute()->Value()  << std::endl;
    int accountId = std::atoi(rootElement->FirstAttribute()->Value());
    for (TiXmlNode *SubItem = rootElement->FirstChild(); SubItem != nullptr; SubItem = SubItem->NextSibling()) {
        std::cout << "!!!current Node " <<SubItem->Value() << ": "<<std::endl;

        //TODO: check whether can be transfer to element or not
        TiXmlElement *createdElement = SubItem->ToElement();
        // if just a child node not element then return
        if (strcmp(createdElement ->Value(), "order") == 0){
            std::cout << "!!!!!current Node " <<createdElement ->Value() <<std::endl;
            handleOrderTransection(createdElement, rootResultElement, accountId);
        }else if (strcmp(createdElement ->Value(), "query") == 0){
            handleQueryTransection(createdElement, rootResultElement, accountId);
        }else if (strcmp(createdElement ->Value(), "cancel") == 0){
            handleCancelTransection(createdElement, rootResultElement, accountId);
        }else{
            return;
        }
    }
}
// handle all type of request
void Server::handleRequest(TiXmlElement* rootElement, TiXmlElement* rootResultElement){
    if (rootElement == nullptr) return;
    if (strcmp(rootElement->Value() , "create") == 0 ){
        handleCreate(rootElement, rootResultElement);
    } else if (strcmp(rootElement->Value() , "transactions") == 0){
        std::cout <<"-current root " <<rootElement->Value() << std::endl;
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
void Server::serveRequest(Socket socket){
    int listen_fd = socket.setupServer(PORT);
    int msg_fd = socket.acceptConn(listen_fd);
    std::vector<char> request = socket.recvMesg(msg_fd);
    std::cout << request.data() << '\n';

    TiXmlDocument* myDocument = new TiXmlDocument();
    myDocument->Parse(getXmlContent(request.data()));
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
    handleRequest(rootElement, rootResultElement);
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
}

int main(int argc, char *argv[]) {
    std::cout << "server running\n";
    Socket socket;
    Server server;
    while(true){
        server.serveRequest(socket);
    }
    return EXIT_SUCCESS;
}