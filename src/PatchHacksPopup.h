#pragma once
#include "./includes.h"

class PatchHack;
class PatchHackNode;

class PatchHacksPopup : public Popup<> {
	std::vector<PatchHack*> patchHacks;
	std::vector<PatchHackNode*> patchHackNodes;
	CCMenuItemSpriteExtra* nextButton;
	CCMenuItemSpriteExtra* previousButton;
	int selectedPage = 0;

	const int rows = 7;
	const int rowOffset = 28;
	const int firstColumnOffset = -180;
	int pageCount;
protected:
	bool setup() override;

	int getPagesCount();
	int getPageByIndex(int index);

	void updateControlButtons();
	void updateDevNodes();

	void onNext(CCObject*);
	void onPrevious(CCObject*);
public:
	static PatchHacksPopup* create(const std::vector<PatchHack*>& patchHacks);
};