#pragma once

#include <fstream>      // std::ifstream
#include <memory>
#include <optional>
#include <string_view>
#include <string>

#include "IdentifyDeviceData.h"

class IdentifyParser {
public:
	IdentifyParser(std::string_view input_file_name);
	std::unique_ptr<IdentifyDeviceData> ProcessInputFile();
private:
	std::string GetModelNumber();
	unsigned int GetHighestSupportedUltraDmaMode();
	std::optional<bool> GetSmartSelfTestSupported();

	std::string AtaStringToString(const char* source, const size_t length);
	bool CheckIfWords85To87Valid(unsigned char word);

	std::string TrimPaddingSpaces(std::string& str);
	std::unique_ptr<char[]> ReadWordsFromFile(int word_position, int word_length);

	int GetIndexOfMsSetBit(unsigned char byte);

	std::string_view input_file_name_{};
	std::ifstream input_file_{};
};

// TODO: Reference additional headers your program requires here.
