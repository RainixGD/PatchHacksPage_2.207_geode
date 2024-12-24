#pragma once
#include "./includes.h"

class BytePatch;

class PatchHack {
	std::string name;
	std::vector<BytePatch*> patches;
	bool isActive = 0;

	bool init(std::string name, std::vector<BytePatch*> patches);
	void apply();

	PatchHack() {};

public:

	void setActive(bool isActive);
	void toggle();
	bool getActive() { return isActive; }
	std::string getName() { return name; }

	static PatchHack* create(std::string name, std::vector<BytePatch*> patches);
};