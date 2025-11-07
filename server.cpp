#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using namespace std;
using namespace boost::asio;
using ip::tcp;

void receive_messages(tcp::socket &socket) {
    boost::system::error_code ec;
    char reply[1024];
    while (true) {
        size_t len = socket.read_some(buffer(reply), ec); // eof true khi len = 0
        if (ec) {
            cout << "Client disconnected." << endl;
            socket.shutdown(tcp::socket::shutdown_send); // 👈 phản hồi FIN
            break;
        }

        string msg(reply, len);
        cout << "\n[Client]: " << msg << endl;
        cout << "> " << flush;
    }
}

int main() {
    io_context io;
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 8080));
    cout << "Server running on port 8080...\n";

    tcp::socket socket(io);
    acceptor.accept(socket);
    cout << "Client connected!\n";

    thread receiver(receive_messages, ref(socket));

    string message;
    while (getline(cin, message)) {
        if (message == "out") {
            socket.shutdown(tcp::socket::shutdown_send);
            break;
        }
        write(socket, buffer(message));
    }

    socket.close();
    receiver.join();
}
