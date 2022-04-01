//
// Created by Tong on 3/18/22.
//

#include "Client.h"

void Client::start() {
    std::cout << "client running...\n";

    //get all the path of test cases
    std::string filePath = "testCases/testcase";
    size_t MAX_TEST_NUM = 8;
    ////获取该路径下的所有文件
    std::stringstream ss;
    for(int i =0; i< MAX_TEST_NUM; i++){
        //notice this use of stringstream
        ss.str("");
        Socket socket;
        int msg_fd = socket.setupClient(HOST, PORT);
        ss<< filePath << i << ".xml";
        std::string testFileName = ss.str();
        std::cout<<"test file name : "<< testFileName <<std::endl;
        TiXmlDocument requestDocument(testFileName.c_str());
        bool loadOk = requestDocument.LoadFile();
        if (!loadOk){
            std::cout << "could load:" << requestDocument.ErrorDesc() << std::endl;
        }
        TiXmlPrinter *printer = new TiXmlPrinter();
        requestDocument.Accept(printer);
        std::string stringBuffer= printer->CStr();
        std::string request = std::to_string(stringBuffer.length());
        request.append("\n");
        request.append(stringBuffer);
        std::cout<<"request  content: "<<"\n"<< request <<std::endl;
        socket.sendMesg(msg_fd, request);
        std::vector<char> response = socket.recvMesg(msg_fd);
        std::cout << response.data() << '\n';
        socket.closeConn(msg_fd);
    }
}

void Client::multiThreadStart(){
    size_t  numOfThread = 10;
//    std::vector<std::thread> threadVector(10);
    std::thread thArr[numOfThread];
    for(int i =0; i< numOfThread; i++){
//        std::thread t(&Client::start, this);
//        threadVector.push_back(t);
        thArr[i] = std::thread(&Client::start, this);
    }
    for (auto &a : thArr)
        a.join();
}

int main(int argc, char *argv[]) {
    Client client;
    client.multiThreadStart();
    return EXIT_SUCCESS;
}
