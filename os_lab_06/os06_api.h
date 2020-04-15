#pragma once
#include <string>
#include <zconf.h>
#include "zmq.hpp"

void create_server(int id, int parent_id, int port);

bool send_message(zmq::socket_t& socket, const std::string& message_string);

std::string recieve_message(zmq::socket_t& socket);

std::string get_port_name(int port);

int accept_connection(zmq::socket_t& socket);
