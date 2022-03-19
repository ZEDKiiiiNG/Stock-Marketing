//
// Created by Tong on 3/18/22.
//

#include "Server.h"

int main(int argc, char *argv[]) {
    std::cout << "server running\n";
    Socket socket;
    int listen_fd = socket.setupServer(PORT);
    int msg_fd = socket.acceptConn(listen_fd);
    std::vector<char> request = socket.recvMesg(msg_fd);
    std::cout << request.data() << '\n';

    std::string response = "173\n"
                           "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                           "<results>\n"
                           "    <created id=\"123456\"/>\n"
                           "    <created sym=\"SPY\" id=\"123456\"/>\n"
                           "</results>";
    socket.sendMesg(msg_fd, response);

    socket.closeConn(listen_fd);
    socket.closeConn(msg_fd);
    return EXIT_SUCCESS;
}