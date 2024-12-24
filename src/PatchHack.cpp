#include "./PatchHack.h"
#include "./BytePatch.h"

bool PatchHack::init(std::string name, std::vector<BytePatch*> patches) {
	this->name = name;
	this->patches = patches;

	return true;
}

void PatchHack::apply() {
	for (auto patch : patches)
		patch->apply(isActive);
}

void PatchHack::setActive(bool isActive) {
	this->isActive = isActive;
	apply();
}

void PatchHack::toggle() {
	this->isActive = !this->isActive;
	apply();
}

PatchHack* PatchHack::create(std::string name, std::vector<BytePatch*> patches) {
	auto ret = new PatchHack();
	if (ret && ret->init(name, patches)) {
		return ret;
	}
	delete ret;
	return nullptr;
}