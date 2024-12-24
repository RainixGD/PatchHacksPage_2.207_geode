#pragma once
#include "./includes.h"

class PatchHack;

class PatchHackNode : public CCMenu {
	PatchHack* patchHack;
	CCSprite* onSprite;
	CCSprite* offSprite;
	CCMenuItemToggler* button;

	bool init(PatchHack* patchHack);
	void onCheckbox(CCObject*);

public:

	static PatchHackNode* create(PatchHack* patchHack);
};