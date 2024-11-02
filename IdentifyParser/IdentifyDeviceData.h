#pragma once

#include <optional>
#include <string>
#include <memory>


struct IdentifyDeviceData
{
	IdentifyDeviceData(std::string model_number, unsigned int highest_supported_ultra_dma_mode, std::optional<bool> is_smart_self_test_supported) : model_number{std::move(model_number)}, highest_supported_ultra_dma_mode{ highest_supported_ultra_dma_mode }, is_smart_self_test_supported {std::move(is_smart_self_test_supported)} {}

	std::string model_number{};
	unsigned int highest_supported_ultra_dma_mode{};
	std::optional<bool> is_smart_self_test_supported{};
};