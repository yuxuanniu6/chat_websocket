#include "WebsocketClient.h"

/*
    用户A登录
 id/name/sex/age/desc
*/
int main() {
	bool done = false;
	std::string input;
	WebsocketClient endpoint;

	while (!done) {

		std::cout << "Enter Command: ";
		std::getline(std::cin, input);

		if (input == "quit") {
			done = true;
		}
		else if (input == "help") {
			std::cout
				<< "\nCommand List:\n"
				<< "connect <ws uri>\n"
				<< "send <message>\n"
				<< "close [<close code:default=1000>] [<close reason>]\n"
				<< "help: Display this help text\n"
				<< "quit: Exit the program\n"
				<< std::endl;
		}
		else if (input.substr(0, 7) == "connect") {
			endpoint.Connect(input.substr(8));
		}
		else if (input.substr(0, 4) == "send") {
			std::stringstream ss(input);

			std::string cmd;
			int id;
			std::string message;

			ss >> cmd;
			std::getline(ss, message);

			endpoint.Send(message);
		}
		else if (input.substr(0, 5) == "close") {
			std::stringstream ss(input);

			std::string cmd;
			std::string reason;

			ss >> cmd ;
			std::getline(ss, reason);

			endpoint.Close(reason);
		}
		else if (input.substr(0, 4) == "quit") {
			done = true;
		}
		else {
			std::cout << "> Unrecognized Command" << std::endl;
		}
	}

	return 0;
}

