// IdentifyParser.cpp : Defines the entry point for the application.
//

#include "IdentifyParser.h"

#include <stdexcept> // for std::runtime_error
#include <string_view>
#include <fstream>      // std::ifstream
#include <filesystem>
#include <string>
#include <optional>


#include "IdentifyDeviceData.h"


#define BYTE_SIZE 1
#define WORD_SIZE (BYTE_SIZE * 2)


auto GetOptions(int argc, char** argv)
{
	if (argc == 2) {
		return argv[1];
	}
	else {
		throw std::runtime_error("File name is not given!");
	}
	
}


// https://cplusplus.com/reference/string/string/find_last_not_of/
std::string TrimPaddingSpaces(std::string& str)
{
	static constexpr auto kSpaceChar{ ' '};
	std::size_t found = str.find_last_not_of(kSpaceChar);
	if (found != std::string::npos)
		str.erase(found + 1);
	else
		str.clear();            // str is all whitespace
	return std::move(str);
}

// https://people.freebsd.org/~imp/asiabsdcon2015/works/d2161r5-ATAATAPI_Command_Set_-_3.pdf, p:17
std::string AtaStringToString(const char* source, const size_t length)
{
	std::string result(source, length);
	for (auto i{ 0 }; i + 1 < result.size() ; i += 2)
	{
		std::swap(result[i], result[i + 1]);
	}
	return TrimPaddingSpaces(result);
}


std::string GetModelNumber(std::ifstream& input_file)
{
	static constexpr auto kFieldLength{ 20 * 2 };
	static constexpr auto kFieldPosition{ 27 * 2 };

	char model_number[kFieldLength] = {};
	// move current read position in the stream:
	input_file.seekg(kFieldPosition, std::ios::beg);
	input_file.read(model_number, kFieldLength);
	return AtaStringToString(model_number, kFieldLength);
}


int GetIndexOfSetMsb(unsigned char byte)
{
	unsigned char mask = 1 << 6;
	for (int i = 6; i >= 0; --i, mask >>= 1)
	{
		if (byte & mask) return i;
	}
	return -1;	// No set bit found!
}


unsigned int GetHighestSupportedUltraDmaMode(std::ifstream& input_file)
{
	static constexpr auto kFieldLength{ WORD_SIZE };
	static constexpr auto kFieldPosition{ 88 * 2 };

	char ultra_dma_modes[2] = {};
	// move current read position in the stream:
	input_file.seekg(kFieldPosition, std::ios::beg);
	input_file.read(ultra_dma_modes, kFieldLength);
	/*for (int i = 0; i < 2; ++i)
	{
		cout << (int)ultra_dma_modes[i];
	}
	cout << endl;*/
	return GetIndexOfSetMsb(ultra_dma_modes[0]);
}

// If bit 14 of word 87 is set to one and bit 15 of word 87 is cleared to zero, the contents of words 85 - 87 contain valid information.
bool CheckIfWordValid(unsigned char word)
{
	unsigned char mask{ 0b1000'0000 };
	auto is_bit_15_cleared{ !(word & mask) };
	auto is_bit_14_set{ word & (mask >> 1)};
	return is_bit_14_set && is_bit_15_cleared;
}

std::optional<bool> GetSmartSelfTestSupported(std::ifstream& input_file)
{
	static constexpr auto kFieldLength{ WORD_SIZE };
	static constexpr auto kFieldPosition{ 87 * 2 };

	char  features_command_sets_enabled[2] = {};
	// move current read position in the stream:
	input_file.seekg(kFieldPosition, std::ios::beg);
	input_file.read(features_command_sets_enabled, kFieldLength);
	/*for (int i = 0; i < 2; ++i)
	{
		cout << (int)features_command_sets_enabled[i];
	}
	cout << endl;*/
	if (!CheckIfWordValid(features_command_sets_enabled[1])) {
		return {};
	}

	return std::make_optional(static_cast<unsigned char>(features_command_sets_enabled[0]) & 2U);
}

IdentifyDeviceData ProcessInputFile(std::string_view input_file_name)
{
	if (!std::filesystem::exists(input_file_name)) {
		throw std::runtime_error("Could not find input file!");
	}

	std::ifstream input_file(input_file_name.data(), std::ios::binary | std::ios::in);
	if (!input_file) {
		throw std::runtime_error("Could not open input file!");
	}

	return IdentifyDeviceData{ GetModelNumber(input_file) , GetHighestSupportedUltraDmaMode(input_file), GetSmartSelfTestSupported(input_file) };
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
	PrintIdentifyDeviceData(ProcessInputFile(input_file_name));

	return 0;
}
