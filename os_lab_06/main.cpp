#include "os06_api.h"
#include <iostream>
#include <unistd.h>
#include <zmq.hpp>
#include <string>
#include <zconf.h>
#include <csignal>
#include <sstream>
#include <set>
#include <algorithm>
#include <map>
#include <utility>
#include <memory> //shared_ptr
#include <vector> //vector

struct TNode {
	TNode(int id, std::shared_ptr<TNode> son, std::shared_ptr<TNode> brother)
			: id_(id), son_(son), brother_(brother) {}
	int id_;
	std::shared_ptr<TNode> son_;
	std::shared_ptr<TNode> brother_;

};

class TreeFunc {
public:
	TreeFunc() = default;
	~TreeFunc() = default;

	bool Insert(int new_id, int parent_id) {
		if (root_ == nullptr && parent_id == -1) {
			root_ = std::make_shared<TNode> (parent_id, std::make_shared<TNode> (new_id, nullptr, nullptr), nullptr);
			return true;
		}
		std::vector<std::pair<int, int>> path = GetPath(parent_id);
		if (path.empty()) {
			return false;
		}
		int p;
		path.erase(path.begin());//delete -1 root
		std::shared_ptr<TNode> node = root_->son_;
		for (auto i : path) {
			if(path.back().first == i.first){
				p = i.second;///????
				break;
			}
			if (i.second == 0 && node->son_ != nullptr) {
				node = node->son_;
			} else if(i.second == 1 && node->brother_ != nullptr) {
				node = node->brother_;
			}
		}
		if(p == 0){
			node->son_ = std::make_shared<TNode>(new_id, nullptr, nullptr);//new node аля
			return true;
		} else if (p == 1){
			node->brother_ = std::make_shared<TNode>(new_id, nullptr, nullptr);
			return true;
		}
		return false;
	}

	bool Erase(int elem) {
		std::vector<std::pair<int, int>> path = GetPath(elem);
		if (path.empty()) {
			return false;
		}
		path.erase(path.begin());
		while(path.back().first != elem){
			path.pop_back();
		}
		path.pop_back();
		std::shared_ptr<TNode> node = root_;
		node = node->son_;
		for (auto i : path) {
			if (i.second == 0){
				if (node->son_->id_ == elem){
					std::shared_ptr<TNode> ptr = node;
					ptr = ptr->son_;
					node->son_ = ptr->brother_;
				}//сохранить ссылку на node
				node = node->son_;
			} else if(i.second == 1){
				if (node->brother_->id_ == elem){
					std::shared_ptr<TNode> ptr = node;
					ptr = ptr->brother_;
					node->brother_ = ptr->brother_;
				}//сохранить ссылку на node
				node = node->brother_;
			} else {
				return false;
			}
		}
		return true;
	}
	std::vector< std::pair<int, int> > GetPath(int id) {
		std::vector<std::pair<int, int>> v;
		if (Search(root_, id, v)) {
			return v;
		} else return v;
	}


private:
	bool Search(std::shared_ptr<TNode> node, int id, std::vector<std::pair<int, int>>& v) {//b=0
		if (node == nullptr) {
			return false;
		}
		if (node->id_ == id && node->son_ == nullptr) {
			auto p = std::make_pair(node->id_, 0);//to son
			v.push_back(p);
			return true;
		}
		if (node->id_ == id && node->son_ != nullptr){
			auto p = std::make_pair(node->id_, 0);//положили сына, затем от этого сына идем по братьям
			v.push_back(p);
			node = node->son_;
			p = std::make_pair(node->id_, 1);
			v.push_back(p);
			while(node->brother_ != nullptr){
				node = node->brother_;
				auto p = std::make_pair(node->id_, 1);
				v.push_back(p);
			}
			return true;
		}
		auto p = std::make_pair(node->id_, 0);
		v.push_back(p);
		std::shared_ptr<TNode> son_node = node;
		while(son_node->son_ != nullptr) {
			son_node = son_node->son_;
			if (Search(son_node, id, v)) {
				return true;
			}
		}
		v.pop_back();
		p = std::make_pair(node->id_, 1);
		v.push_back(p);
		std::shared_ptr<TNode> brother_node = node;
		while(brother_node->brother_ != nullptr) {
			brother_node = brother_node->brother_;
			if (Search(brother_node, id, v)) {
				return true;
			}
		}
		v.pop_back();
		return false;
	}
	std::shared_ptr<TNode> root_ = nullptr;
};


int main() {
	std::string command;
	TreeFunc Topology;
	int child_pid;
	zmq::context_t context(1);//1 -означает размер пула потоков для передачи сообщений.
	std::map<int, zmq::socket_t> sockets;
    std::map<int, int> pids;
    std::map<int, int> ports;//id and ports 
	
	while (std::cin >> command) {
		std::string res;
		int linger = 0;
		if (command == "create") {
			int new_id, parent_id;
			std::cin >> new_id >> parent_id;
			if (parent_id == -1 && pids.count(new_id) == 0) {//добавляем в корень
				sockets.emplace(new_id, zmq::socket_t(context, ZMQ_REQ));
				sockets.at(new_id).setsockopt(ZMQ_SNDTIMEO, 20);
				sockets.at(new_id).setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
				int port = accept_connection(sockets.at(new_id));
				child_pid = fork();
				if(child_pid == -1) {
					std::cout << "Unable to create worker node\n";
					child_pid = 0;
					exit(1);
				} else if (child_pid == 0) {//потомок
					create_server(new_id, parent_id, port);
				} else {//родитель
					//parent_id = 0; лишнее
					ports[new_id] = port;
                    pids[new_id] = child_pid;
					send_message(sockets.at(new_id), "pid");
    				res = recieve_message(sockets.at(new_id));
				}
			} else {
				if(!Topology.GetPath(new_id).empty()) {
					std::cout << "Error: Node already exists\n";
					continue;
				}
				std::vector<std::pair<int, int>> path = Topology.GetPath(parent_id);
				path.erase(path.begin());//убираем -1 корень
				if (path.empty()){
					std::cout << "Error: No parent node\n";
					continue;
				}
				std::vector <int> path1;
				for (auto i : path) {
					if(i.second == 0){
						path1.push_back(i.first);
					}
				}
				int id = path1.front();
				path1.erase(path1.begin());
				std::ostringstream msg_stream;
				msg_stream << "create " << path1.size();
				for (auto i : path1) {		
					msg_stream << " " << i;
				}
				msg_stream << " " << new_id;

				send_message(sockets.at(id), msg_stream.str());
    			res = recieve_message(sockets.at(id));
			}
			if (res.substr(0,2) == "Ok") {
				Topology.Insert(new_id, parent_id);	
			}		
			std::cout << res << "\n";

		} else if (command == "remove") {
			if (pids.size() == 0) {
				std::cout << "Error: No such node\n";
				continue;
			}
			int remove_id;
			std::cin >> remove_id;
			if (pids.count(remove_id) != 0) {//child_id у родителя хранить PID потомка
				send_message(sockets.at(remove_id), "kill");
				res = recieve_message(sockets.at(remove_id));
				kill(pids.at(remove_id), SIGTERM);
				//SIGTERM - этот сигнал запрашивает остановку работы процесса.
				//Сигнал SIGTERM завершает лишь те процессы, которые не обрабатывают его приход
				kill(pids.at(remove_id), SIGKILL); //немедленное завершение процесса
				//eckb SIGTERM не сработал, то SIGKILL сработает точно
				child_pid = 0;
				pids.erase(remove_id);
				sockets.at(remove_id);
				ports.erase(remove_id);
				if(res.substr(0,2)== "Ok"){
					Topology.Erase(remove_id);
				}
				std::cout << res << "\n";
				continue;
			}
			std::vector<std::pair<int, int>> path = Topology.GetPath(remove_id);
			if (path.empty()) {
				std::cout << "Error: No such node\n";
				continue;
			}

			path.erase(path.begin());	
			std::vector <int> path1;
			for (auto i : path) {
				if(i.second == 0){
					path1.push_back(i.first);
				}
			}

			std::ostringstream msg_stream;
			int next_id = path1.front();
			path1.erase(path1.begin());
			path1.pop_back();

			msg_stream << "remove " << path1.size();
			for (int i : path1) {		
				msg_stream << " " << i;
			}
			msg_stream << " " << remove_id;
			
			send_message(sockets.at(next_id), msg_stream.str());

			res = recieve_message(sockets.at(next_id));

			if(res.substr(0,2)== "Ok"){
				Topology.Erase(remove_id);
			}
			std::cout << res << "\n";

		} else if (command == "exec") {
			int id;
			std::string subcommand;
			std::cin >> id >> subcommand;

			std::vector<std::pair<int, int>> path = Topology.GetPath(id);
			if (path.empty()) {
				std::cout << "Error: No such node\n";
				continue;
			}
			path.erase(path.begin());
			std::vector <int> path1;
			for (auto i : path) {
				if(i.second == 0){
					path1.push_back(i.first);
				}
			}
			int next_id = path1.front();
			path1.erase(path1.begin());
			std::ostringstream msg_stream;
			msg_stream << "exec " << id << " " << subcommand << " "<< path1.size();
 	   		for (int i : path1) {
    	    	msg_stream << " " << i;
    		}
    	
			send_message(sockets.at(next_id), msg_stream.str());
			res = recieve_message(sockets.at(next_id));
			std::cout << res << "\n";

		} else if (command == "ping") {
			int ping_id;
			std::cin >> ping_id;
			
			std::vector<std::pair<int, int>> path = Topology.GetPath(ping_id);
			if(path.empty()) {
				std::cout << "Error: Not found\n";
				continue;
			}
			path.erase(path.begin());
			std::vector <int> path1;
			for (auto i : path) {
				if(i.second == 0){
					path1.push_back(i.first);
				}
			}
			std::ostringstream msg_stream;
			int next_id = path1.front();
			path1.erase(path1.begin());
			msg_stream << "ping " << path1.size();
			for (int i : path1) {		
				msg_stream << " " << i;
			}
			send_message(sockets.at(next_id), msg_stream.str());
			res = recieve_message(sockets.at(next_id));
			if(res.substr(0,5) == "Error") {
				std::cout << "Ok: 0\n";
				continue;
			}
			std::cout << res << "\n";
		} else if (command == "exit") {
			break;
		}
	}//while
	return 0;
}//main
