#pragma once
#include "./includes.h"


class BytePatch {
	size_t address;
	std::vector<uint8_t> onBytes;
	std::vector<uint8_t> offBytes;

	std::optional<std::vector<uint8_t>> stringToBytes(const std::string& byteString);
	std::optional<size_t> hexStringToAddress(const std::string& hexString);
	bool init(std::string addressString, std::string onBytesString, std::optional<std::string> offBytesString = std::nullopt);
	void patch(size_t address, std::vector<std::uint8_t> bytes);

	BytePatch() {}

public:

	void apply(bool isActive);

	static BytePatch* create(std::string addressString, std::string onBytesString, std::optional<std::string> offBytesString = std::nullopt);
};