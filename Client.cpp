//
// Created by Tong on 3/18/22.
//

#include "Client.h"

int main(int argc, char *argv[]) {
    std::cout << "client running...\n";
    Socket socket;
    int client_fd = socket.setupClient(HOST, PORT);
    std::string request = "173\n"
                          "  <?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                          "  <create>\n"
                          "    <account id=\"123456\" balance=\"1000\"/>\n"
                          "    <symbol sym=\"SPY\">\n"
                          "      <account id=\"123456\">100000</account>\n"
                          "    </symbol>\n"
                          "</create>";
    socket.sendMesg(client_fd, request);
    socket.closeConn(client_fd);
    return EXIT_SUCCESS;
}
