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
    std::string response = "127\n"
                           "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                           "<results>\n"
                           "    <created id=\"123456\"/>\n"
                           "    <created sym=\"SPY\" id=\"123456\"/>\n"
                           "</results>";

    //目前是print 所有的值，之后应该会用map存

    Server server;
    TiXmlDocument* myDocument = new TiXmlDocument();
    myDocument->Parse(server.getXmlContent(request.data()));
//    myDocument->Parse(request.data());
    std::cout << "Parse Complete" << '\n';
    TiXmlElement* rootElement = myDocument->RootElement();
    printXml(rootElement, true);

    socket.sendMesg(msg_fd, response);

    socket.closeConn(listen_fd);
    socket.closeConn(msg_fd);
    return EXIT_SUCCESS;
}