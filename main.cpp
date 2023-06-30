#include <iostream>
#include "application.h"

int main() {
	std::shared_ptr<IP::Application> app = std::make_shared<IP::Application>();

	try {
		app->run();
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}