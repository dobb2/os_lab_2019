#include <iostream>
#include <zmq.hpp>
#include <string>
#include <sstream>
#include <zconf.h>
#include <exception>
#include <csignal>
#include <map>
#include <ctime>
#include <utility>
#include "os06_api.h"


int main(int argc, char* argv[]) {

	time_t seconds, now;
  	time_t stop = 0;
  	bool bool_time;

	int id = std::stoi(argv[1]);
	int parent_id = std::stoi(argv[2]);
	int parent_port = std::stoi(argv[3]);

	zmq::context_t context (3);// сыну, брату и отцу
	zmq::socket_t parent_socket(context, ZMQ_REP);//ZMQ_REP by servers

	parent_socket.connect(get_port_name(parent_port)); // подключаемся к сокету родителя

	std::map<int, zmq::socket_t> sockets;
    std::map<int, int> pids;
    std::map<int, int> ports;

	while (true) {
		int linger = 0;
		std::string request;

		request = recieve_message(parent_socket);
		std::istringstream command_stream(request);
		std::string command;
		command_stream >> command;

		if (command == "pid"){
			std::string answer = "Ok:" + std::to_string(getpid());
			send_message(parent_socket, answer);
		} else if (command == "create") {
			int new_id, size;
			command_stream >> size;
			std::vector<int> path(size);
			for (int i = 0; i < size; ++i){
				command_stream >> path[i];
			}
			command_stream >> new_id;
			if (path.size() == 0) {
				sockets.emplace(new_id, zmq::socket_t(context, ZMQ_REQ));
				sockets.at(new_id).setsockopt(ZMQ_SNDTIMEO, 20);
				sockets.at(new_id).setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
				int port = accept_connection(sockets.at(new_id));//создаем точку для соединений с с сыном
				int pid = fork();
				if (pid == -1) {
					send_message(parent_socket, "Cannot fork");
					continue;
				} else if (pid == 0) {
					create_server(new_id, id, port);
				} else {
					//сохраняем порт нового процесса и его pid
					ports[new_id] = port;
					pids[new_id] = pid;
					send_message(sockets.at(new_id), "pid");
					send_message(parent_socket, recieve_message(sockets.at(new_id)));
				}
			} else {
				int next_id = path.front();
				path.erase(path.begin());
				std::ostringstream msg_stream;
				msg_stream << "create " << path.size();
				for(int i : path) {
					msg_stream << " " << i;
				}
				msg_stream << " " << new_id;
				
				send_message(sockets.at(next_id), msg_stream.str());
				//std::string res = recieve_message(sockets.at(next_id));
				//res = res + " " + msg_stream.str();
				//send_message(parent_socket, res);
				send_message(parent_socket, recieve_message(sockets.at(next_id)));
			}
		} else if (command == "remove") {
			int size, remove_id;
			command_stream >> size;
			std::vector<int> path(size);
			for (int i = 0; i < size; ++i){
				command_stream >> path[i];
			}
			command_stream >> remove_id;
			if (size == 0) {
				if (sockets.count(remove_id) == 0){
					send_message(parent_socket, "Not found");
					continue;
				}
				send_message(sockets.at(remove_id), "kill");
				recieve_message(sockets.at(remove_id));
				kill(pids.at(remove_id), SIGTERM);
				kill(pids.at(remove_id), SIGKILL);
				pids.erase(remove_id);
				sockets.at(remove_id).disconnect(get_port_name(ports[remove_id]));
				ports.erase(remove_id);
				sockets.erase(remove_id);
				send_message(parent_socket, "Ok");
			} else {
				int next_id = path.front();
				path.erase(path.begin());
				std::ostringstream msg_stream;
				msg_stream << "remove " << path.size();
				if(path.size() != 0){
					for (int i : path) {
						msg_stream << " " << i;
					}
				}
				msg_stream << " " << remove_id;
				send_message(sockets.at(next_id), msg_stream.str());
				send_message(parent_socket, recieve_message(sockets.at(next_id)));
			}
		} else if (command == "exec") {
			int size;
			std::string subcommand;
			command_stream >> id >> subcommand >> size;
			std::vector<int> path(size);
			for (int i = 0; i < size; ++i){
				command_stream >> path[i];
			}

			if (size == 0) { //дошли до нужного узла
				std::ostringstream msg_stream;
				if (subcommand == "start") {
					seconds = time(NULL);
					bool_time = true;
					msg_stream << "Ok:" << id;
				} else if (subcommand == "time") {
					if(!bool_time){
        				now = stop;
      				} else {
       					 now = time(NULL) - seconds + stop;
      				}
      				int time = now * 1000;
					msg_stream << "Ok:" << id << ": " << std::to_string(time);
				} else if (subcommand == "stop"){
					stop = time(NULL) - seconds;
      				bool_time = false;
					msg_stream << "Ok:" << id;
				}
				send_message(parent_socket, msg_stream.str());
			} else {
				int next_id = path.front();
				path.erase(path.begin());
				std::ostringstream msg_stream;
				msg_stream << "exec " << id << subcommand << " " <<  path.size();
				for(int i : path) {
					msg_stream << " " << i;
				}
				send_message(sockets.at(next_id), msg_stream.str());
				send_message(parent_socket, recieve_message(sockets.at(next_id)));
			}	
		} else if (command == "ping") {
			int size;
			command_stream >> size;
			std::vector<int> path(size);
			for (int i = 0; i < size; ++i){
				command_stream >> path[i];
			}
			
			if(size == 0){//дошли до нужного нам сокета
				std::ostringstream msg_stream;
				msg_stream << "Ok:" << id;
				send_message(parent_socket, msg_stream.str());
			} else {
				int next_id = path.front();
				path.erase(path.begin());
				std::ostringstream msg_stream;
				msg_stream << "ping " << path.size();
				for(int i: path) {
					msg_stream << " " << i;
				}
				send_message(sockets.at(next_id), msg_stream.str());
				send_message(parent_socket, recieve_message(sockets.at(next_id)));
			}
		} else if (command == "kill") {
			for (auto& [child_id, child_socket] : sockets) {
				send_message(child_socket, "kill");
				recieve_message(child_socket);
				kill(pids.at(child_id), SIGTERM);
				kill(pids.at(child_id), SIGKILL);
			}
			send_message(parent_socket, "Ok");
 		} else {
 			send_message(parent_socket, "WTF");
 		}
 		if (parent_port == 0) {//если будет кол-во узлов больше int
 			break;
 		}
	}
}