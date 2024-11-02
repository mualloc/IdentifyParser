#pragma once

#include <string_view>
#include <fstream>      // std::ifstream
#include <string>
#include <optional>
#include <memory>

#include "IdentifyDeviceData.h"

class IdentifyParser {
public:
	IdentifyParser(std::string_view input_file_name);
	IdentifyDeviceData ProcessInputFile();
private:
	std::string GetModelNumber();
	unsigned int GetHighestSupportedUltraDmaMode();
	std::optional<bool> GetSmartSelfTestSupported();

	std::string AtaStringToString(const char* source, const size_t length);
	std::string TrimPaddingSpaces(std::string& str);
	int GetIndexOfMsSetBit(unsigned char byte);
	bool CheckIfWords85To87Valid(unsigned char word);
	std::unique_ptr<char[]> ReadWordsFromFile(int word_position, int word_length);

	std::string_view input_file_name_{};
	std::ifstream input_file_{};
};

// TODO: Reference additional headers your program requires here.
