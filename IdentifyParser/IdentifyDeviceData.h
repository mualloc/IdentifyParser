#pragma once

#include <string>
#include <optional>


struct IdentifyDeviceData
{
	std::string model_number{};
	unsigned int highest_supported_ultra_dma_mode{};
	std::optional<bool> is_smart_self_test_supported{};
};