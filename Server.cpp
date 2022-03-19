//
// Created by Tong on 3/18/22.
//

#include "Server.h"

int main(int argc, char *argv[]) {
    Socket socket;
    int listen_fd = socket.setupServer(PORT);
    int msg_fd = socket.acceptConn(listen_fd);
    std::vector<char> request = socket.recvMesg(msg_fd);
    std::cout << request.data() << '\n';
    return EXIT_SUCCESS;
}