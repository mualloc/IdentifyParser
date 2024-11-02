// IdentifyParser.cpp : Defines the entry point for the application.
//

#include "IdentifyParser.h"


#include <iostream>
#include <stdexcept> // for std::runtime_error
#include <string_view>
#include <memory> 



#include "IdentifyDeviceData.h"


auto GetOptions(int argc, char** argv)
{
	if (argc == 2) {
		return argv[1];
	}
	else {
		throw std::runtime_error("File name is not given!");
	}
}

void PrintIdentifyDeviceData(IdentifyDeviceData identify_device_data)
{
	std::cout << "Model number : " << identify_device_data.model_number << std::endl;
	std::cout << "Highest supported Ultra DMA mode : " << identify_device_data.highest_supported_ultra_dma_mode << std::endl;
	std::cout << "SMART self-test supported : " << std::boolalpha << identify_device_data.is_smart_self_test_supported.value_or("No valid information!") << std::endl;
}

int main(int argc, char** argv)
{
	std::string_view input_file_name = GetOptions(argc, argv);
	std::unique_ptr<IdentifyParser> identify_parser = std::make_unique<IdentifyParser>(input_file_name);

	PrintIdentifyDeviceData(identify_parser->ProcessInputFile());
	return 0;
}
