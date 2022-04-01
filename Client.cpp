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
//        std::cout<<"request  content: "<<"\n"<< request <<std::endl;
        socket.sendMesg(msg_fd, request);
        std::cout<<"request  send!!!!: "<<std::endl;
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
double Client::calc_time(struct timespec start, struct timespec end) {
    double start_sec = (double)start.tv_sec*1000000000.0 + (double)start.tv_nsec;
    double end_sec = (double)end.tv_sec*1000000000.0 + (double)end.tv_nsec;
    if (end_sec < start_sec) {
        return 0;
    } else {
        return end_sec - start_sec;
    }
}

std::string getAccountCreateXML(int randomBase){
    size_t accountNum = 10;
    std::stringstream ss;

    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
          "<create>\n";
    for(int i =0; i< accountNum; i++){
        int randomId = rand() % accountNum + randomBase;
        ss << "<account id= \"" << randomId << "\" balance=\"1000\"/>\n";
    }
    ss << "</create>";
    std::cout<< "test result !!!"<< ss.str() << std::endl;
    return ss.str();
}
std::string getSymCreateXML(int randomBase){
    size_t accountNum = 10;
    size_t SymNum = 5;
    size_t AccountPerSym = 3;
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
          "<create>\n";
    for(int i =0; i< SymNum; i++){
        int randomSymId = rand() % SymNum + randomBase;
        ss << "    <symbol sym=\"testSYM"<< randomSymId <<"\">\n";
        for(int j =0; j < AccountPerSym; j++){
            int randomAccountId = rand() % accountNum + randomBase;
            ss << "        <account id=\""<< randomAccountId <<"\">100000</account>\n";
        }
        ss << "    </symbol>\n";
    }
    ss << "</create>";
    std::cout<< "test result !!!"<< ss.str() << std::endl;
    return ss.str();
}
std::string getTransXML(int randomBase){
    size_t accountNum = 10;
    size_t SymNum = 5;
    size_t AccountPerSym = 3;
    size_t BuyOrderNum = 3;
    size_t SellOrderNum = 3;
    size_t queryNum = 5;
    size_t cancelNum = 5;
    std::stringstream ss;
    int accountId = randomBase + rand()%accountNum;
    ss<< "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
         "<transactions id=\""<< accountId <<"\">\n";
    for (int i =0; i< BuyOrderNum; i++){
        int randomSymId = rand() % SymNum + randomBase;
        ss << "    <order sym=\"testSYM"<< randomSymId <<"\" amount=\"100\" limit=\"2."<<rand()%10<<"\"/>\n";
    }
    for (int i =0; i< SellOrderNum; i++){
        int randomSymId = rand() % SymNum + randomBase;
        ss << "    <order sym=\"testSYM"<< randomSymId <<"\" amount=\"-100\" limit=\"2."<<rand()%10<<"\"/>\n";
    }
    for(int i= 0; i < queryNum; i++){
        int randomQueryId = rand() % queryNum + randomBase;
        ss<<"    <query id=\""<<randomQueryId<<"\"/>\n";
    }
    for(int i= 0; i < cancelNum; i++){
        int randomCancelId = rand() % cancelNum + randomBase;
        ss<<"    <cancel id=\""<<randomCancelId<<"\"/>\n";
    }
    ss<< "</transactions>";
    std::cout<< "test result !!!"<< ss.str() << std::endl;
    return ss.str();
}

int main(int argc, char *argv[]) {
    Client client;
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    int randomBase = rand()%1000;
    client.multiThreadStart();
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_ns = client.calc_time(start_time, end_time);
    printf("%f ns\n",   elapsed_ns/1000000000);
//    std::cout << "arg[]: "<< argv[1] <<std::endl;
//    client.multiThreadStart();
    return EXIT_SUCCESS;
}
