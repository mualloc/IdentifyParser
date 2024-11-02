#include "IdentifyParser.h"

#include <filesystem>


#define BYTE_SIZE (1)
#define WORD_SIZE (BYTE_SIZE * 2)


IdentifyParser::IdentifyParser(std::string_view input_file_name) : input_file_name_{ input_file_name }
{
	if (!std::filesystem::exists(input_file_name_)) {
		throw std::runtime_error("Could not find input file!");
	}
}

std::unique_ptr<IdentifyDeviceData> IdentifyParser::ProcessInputFile()
{
	input_file_.open(input_file_name_.data(), std::ios::binary | std::ios::in);
	if (!input_file_) {
		throw std::runtime_error("Could not open input file!");
	}
	return std::make_unique<IdentifyDeviceData>(GetModelNumber(), GetHighestSupportedUltraDmaMode(), GetSmartSelfTestSupported());
}

std::string IdentifyParser::GetModelNumber()
{
	static constexpr auto kWordPosition{27U};
	static constexpr auto kWordLength{20U};
	auto model_number = ReadWordsFromFile(kWordPosition, kWordLength);
	return AtaStringToString(model_number.get(), kWordLength << 1);
}

unsigned int IdentifyParser::GetHighestSupportedUltraDmaMode()
{
	static constexpr auto kWordPosition{ 88};
	static constexpr auto kWordLength{ WORD_SIZE };
	auto ultra_dma_modes = ReadWordsFromFile(kWordPosition, kWordLength);
	return GetIndexOfMsSetBit(ultra_dma_modes[0]);
}

// Note: Word 84 Bit 1 has also same information
std::optional<bool> IdentifyParser::GetSmartSelfTestSupported()
{
	static constexpr auto kWordPosition{ 87 };
	static constexpr auto kWordLength{ WORD_SIZE };
	auto features_command_sets_enabled = ReadWordsFromFile(kWordPosition, kWordLength);
	if (!CheckIfWords85To87Valid(features_command_sets_enabled[1])) {
		return {};
	}
	return std::make_optional(static_cast<unsigned char>(features_command_sets_enabled[0]) & 2U);
}


// https://people.freebsd.org/~imp/asiabsdcon2015/works/d2161r5-ATAATAPI_Command_Set_-_3.pdf, p:17
std::string IdentifyParser::AtaStringToString(const char* source, const size_t length)
{
	std::string result(source, length);
	for (auto i{ 0 }; i + 1 < static_cast<int>(result.size()); i += 2)
	{
		std::swap(result[i], result[i + 1]);
	}
	return TrimPaddingSpaces(result);
}


// https://cplusplus.com/reference/string/string/find_last_not_of/
std::string IdentifyParser::TrimPaddingSpaces(std::string& str)
{
	static constexpr auto kSpaceChar{ ' ' };
	std::size_t found = str.find_last_not_of(kSpaceChar);
	if (found != std::string::npos)
		str.erase(found + 1);
	else
		str.clear();            // str is all whitespace
	return std::move(str);
}

int IdentifyParser::GetIndexOfMsSetBit(unsigned char byte)
{
	unsigned char mask = 1 << 6;
	for (int i = 6; i >= 0; --i, mask >>= 1)
	{
		if (byte & mask) return i;
	}
	return -1;	// No set bit found!
}

// If bit 14 of word 87 is set to one and bit 15 of word 87 is cleared to zero, the contents of words 85 - 87 contain valid information.
bool IdentifyParser::CheckIfWords85To87Valid(unsigned char word)
{
	unsigned char mask{ 0b1000'0000 };
	auto is_bit_14_set{ word & (mask >> 1) };
	auto is_bit_15_cleared{ !(word & mask) };
	return is_bit_14_set && is_bit_15_cleared;
}

std::unique_ptr<char[]> IdentifyParser::ReadWordsFromFile(int word_position, int word_count)
{
	 const auto kBytePosition{ word_position << 1 };
	 const auto kByteLength{ word_count << 1 };
	 auto read_buffer = std::make_unique<char[]>(kByteLength);
	 // move current read position in the stream:
	 input_file_.seekg(kBytePosition, std::ios::beg);
	 input_file_.read(read_buffer.get(), kByteLength);
	 return read_buffer;
}
