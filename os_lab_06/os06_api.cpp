#include <string>
#include "os06_api.h"

void create_server(int id, int parent_id, int port) {
    char* arg1 = strdup((std::to_string(id)).c_str());
    char* arg2 = strdup((std::to_string(parent_id)).c_str());
    char* arg3 = strdup((std::to_string(port)).c_str());
    char* args[] = {"./child_main", arg1, arg2, arg3, NULL};
    execv("./child_main", args);
}

bool send_message(zmq::socket_t& socket, const std::string& message_string) {
    zmq::message_t message(message_string.size());
    memcpy(message.data(), message_string.c_str(), message_string.size());
    return socket.send(message);
}

std::string recieve_message(zmq::socket_t& socket) {
    zmq::message_t message;
    bool ok;
    try {
        ok = socket.recv(&message);
    } catch (...) {
        ok = false;
    }
    std::string recv_message(static_cast<char*>(message.data()), message.size());
    if (recv_message.empty() || !ok) {
        return "Error: Node is not available";
    }
    return recv_message;
}

std::string get_port_name(int port) {
    return "tcp://127.0.0.1:" + std::to_string(port);
}
int accept_connection(zmq::socket_t& socket) {
    int port = 4040;
    while (true) {
        try {
            socket.bind(get_port_name(port));
            break;
        } catch(...) {
            port++;
        }
    }
    return port;
}
