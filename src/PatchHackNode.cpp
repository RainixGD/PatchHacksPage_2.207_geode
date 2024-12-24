#include "./PatchHackNode.h"
#include "./PatchHack.h"


bool PatchHackNode::init(PatchHack* patchHack) {
	if (!CCMenu::init()) return false;

	this->patchHack = patchHack;

	onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
	onSprite->setScale(0.7);
	offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
	offSprite->setScale(0.7);
	onSprite->setContentSize(offSprite->getContentSize());
	button = CCMenuItemToggler::create(offSprite, onSprite, this, menu_selector(PatchHackNode::onCheckbox));
	if (patchHack->getActive()) button->toggle(1);
	this->addChild(button);

	auto nameLabel = CCLabelBMFont::create(patchHack->getName().c_str(), "bigFont.fnt");
	nameLabel->setAnchorPoint({ 0, 0.5 });
	nameLabel->setPosition(20, 0);
	nameLabel->limitLabelWidth(145, 0.6, 0.1);
	this->addChild(nameLabel);

	return true;
}

void PatchHackNode::onCheckbox(CCObject*) {
	patchHack->toggle();
}


PatchHackNode* PatchHackNode::create(PatchHack* patchHack) {
	auto ret = new PatchHackNode;
	if (ret && ret->init(patchHack)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}