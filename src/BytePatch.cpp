#include "./BytePatch.h"

std::optional<std::vector<uint8_t>> BytePatch::stringToBytes(const std::string& byteString) {
	std::vector<uint8_t> bytes;
	std::istringstream stream(byteString);
	std::string byteStr;

	try {
		while (stream >> byteStr) {
			uint8_t byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
			bytes.push_back(byte);
		}
	}
	catch (const std::exception&) {
		return std::nullopt;
	}

	return bytes;
}

std::optional<size_t> BytePatch::hexStringToAddress(const std::string& hexString) {
	size_t address = 0;
	try {
		address = std::stoull(hexString, nullptr, 16);
	}
	catch (const std::exception&) {
		return std::nullopt;
	}
	return address;
}

bool BytePatch::init(std::string addressString, std::string onBytesString, std::optional<std::string> offBytesString) {

	auto addressOpt = hexStringToAddress(addressString);
	auto onBytesOpt = stringToBytes(onBytesString);


	if (!onBytesOpt.has_value() || !addressOpt.has_value()) return false;

	this->address = addressOpt.value();
	this->onBytes = onBytesOpt.value();
	
	if (offBytesString.has_value()) {
		auto offBytesOpt = stringToBytes(offBytesString.value());
		if (!offBytesOpt.has_value()) return false;
		this->offBytes = offBytesOpt.value();
	}
	else {
		this->offBytes.resize(this->onBytes.size());
		size_t absolute_address = geode::prelude::base::get() + this->address;
		memcpy(this->offBytes.data(), reinterpret_cast<void*>(absolute_address), this->onBytes.size());
	}

	return true;
}

void BytePatch::patch(size_t address, std::vector<std::uint8_t> bytes) {
	auto size = bytes.size();
	DWORD old_prot;

	size_t absolute_address = geode::prelude::base::get() + address;

	VirtualProtect(reinterpret_cast<void*>(absolute_address), size, PAGE_EXECUTE_READWRITE, &old_prot);
	memcpy(reinterpret_cast<void*>(absolute_address), bytes.data(), size);
	VirtualProtect(reinterpret_cast<void*>(absolute_address), size, old_prot, &old_prot);
}

void BytePatch::apply(bool isActive) {
	patch(address, isActive ? onBytes : offBytes);
}

BytePatch* BytePatch::create(std::string addressString, std::string onBytesString, std::optional<std::string> offBytesString) {
	auto ret = new BytePatch();
	if (ret && ret->init(addressString, onBytesString, offBytesString)) {
		return ret;
	}
	delete ret;
	return nullptr;
}