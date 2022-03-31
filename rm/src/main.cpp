#include "config.h"
#include "constants.h"

#include <iostream> // Just for testing purposes

int main() {
	auto &c = konro::Config::get(CONFIG_PATH);

	std::cout << c.read<std::string>("test", "name") << std::endl;
	std::cout << c.read<int>("test", "number") << std::endl;
	
	return 0;
}
