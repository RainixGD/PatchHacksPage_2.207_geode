#include "./PatchHacksPopup.h"
#include "./PatchHack.h"
#include "./PatchHackNode.h"

bool PatchHacksPopup::setup() {
	if (patchHacks.size() == 0) return true;

	auto size = CCDirector::sharedDirector()->getWinSize();

	auto nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
	nextSprite->setFlipX(true);
	nextButton = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(PatchHacksPopup::onNext));
	nextButton->setPosition({ 430, 105 });
	m_buttonMenu->addChild(nextButton);
	
	auto previousSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
	previousButton = CCMenuItemSpriteExtra::create(previousSprite, this, menu_selector(PatchHacksPopup::onPrevious));
	previousButton->setPosition({ 30, 105 });
	m_buttonMenu->addChild(previousButton);

	for (int i = 0; i < patchHacks.size(); i++) {
		auto patchHackNode = PatchHackNode::create(patchHacks[i]);
		patchHackNode->setPosition(15 + 230 + ((i / rows) % 2 == 0 ? firstColumnOffset : 0), 105 + (float)rowOffset * ((float)rows / 2 - 0.5) - (i % rows) * rowOffset);
		m_mainLayer->addChild(patchHackNode);
		this->patchHackNodes.push_back(patchHackNode);
	}

	pageCount = getPagesCount();

	updateDevNodes();
	updateControlButtons();

	return true;
}

int PatchHacksPopup::getPagesCount() {
	return std::ceil((float)patchHacks.size() / (rows * 2));
}

int PatchHacksPopup::getPageByIndex(int index) {
	return std::ceil((float)(index + 1) / (rows * 2)) - 1;
}

void PatchHacksPopup::updateControlButtons() {
	nextButton->setVisible(selectedPage == (pageCount - 1) ? 0 : 1);
	previousButton->setVisible(selectedPage == 0 ? 0 : 1);
}

void PatchHacksPopup::updateDevNodes() {
	for (int i = 0; i < patchHackNodes.size(); i++)
		patchHackNodes[i]->setVisible((selectedPage == getPageByIndex(i)) ? 1 : 0);
}

void PatchHacksPopup::onNext(CCObject*) {
	if (selectedPage < pageCount) {
		selectedPage++;
	}
	updateDevNodes();
	updateControlButtons();
}

void PatchHacksPopup::onPrevious(CCObject*) {
	if (selectedPage > 0) {
		selectedPage--;
	}
	updateDevNodes();
	updateControlButtons();
}

PatchHacksPopup* PatchHacksPopup::create(const std::vector<PatchHack*>& patchHacks) {
	auto ret = new PatchHacksPopup;
	ret->patchHacks = patchHacks;
	if (ret && ret->initAnchored(460, 210)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}