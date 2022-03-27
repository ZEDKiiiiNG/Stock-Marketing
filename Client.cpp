//
// Created by Tong on 3/18/22.
//

#include "Client.h"

int main(int argc, char *argv[]) {
    std::cout << "client running...\n";
    Socket socket;
    int msg_fd = socket.setupClient(HOST, PORT);
    std::string request = "173\n"
                          "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                          "<create>\n"
                          "    <account id=\"123456\" balance=\"1000\"/>\n"
                          "    <account id=\"123456\" balance=\"1000\"/>\n"
                          "    <symbol sym=\"SPY\">\n"
                          "        <account id=\"123456\">100000</account>\n"
                          "        <account id=\"12345\">100000</account>\n"
                          "        <account id=\"1234\">100000</account>\n"
                          "    </symbol>\n"
                          "</create>";

    socket.sendMesg(msg_fd, request);
    std::vector<char> response = socket.recvMesg(msg_fd);
    std::cout << response.data() << '\n';

    socket.closeConn(msg_fd);
    //second time:
    int msg_fd2 = socket.setupClient(HOST, PORT);
    std::string request2 =
            "13\n"
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<transactions id=\"123456\">\n"
            " <order sym=\"SYM\" amount=\"123\" limit=\"2.3\"/>\n"
            " <query id=\"TRANS_ID\">\n"
            " <cancel id=\"TRANS_ID\">\n"
            "</transactions>";
    socket.sendMesg(msg_fd2, request2);
    std::vector<char> response2 = socket.recvMesg(msg_fd2);
    std::cout << response2.data() << '\n';

    socket.closeConn(msg_fd);
    return EXIT_SUCCESS;
}
