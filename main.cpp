#include "IdentifyDeviceData.h"

#include <stdexcept> // for std::runtime_error
#include <string_view>


auto GetOptions(int argc, char** argv)
{
	if (argc == 2) {
		return argv[1];
	}
	else {
		throw std::runtime_error("File name is not given!");
	}
}

int main(int argc, char** argv)
{
	std::string_view input_file_name = GetOptions(argc, argv);
	//IdentifyDeviceCommandProcessor(input_file_name)
	PrintIdentifyDeviceData(ProcessInputFile(input_file_name));

	return 0;
}