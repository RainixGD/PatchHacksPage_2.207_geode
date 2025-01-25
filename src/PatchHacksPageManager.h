#pragma once
#include "./includes.h"

class PatchHack;

class PatchHacksPageManager {

	bool isOk = false;
	std::vector<std::string> activeLayers;
	std::vector<PatchHack*> hacks;
	static PatchHacksPageManager* instance;

	void init();

	bool loadData();

	void loadStatesFile();
	void saveStatesFile();

	void buildPatchHacksButton(CCLayer* layer);
	void onPatchHacksBtn(CCObject*);

	PatchHacksPageManager() {};
public:

	void onTrySaveGame();
	void onEditorPauseLayer(CCLayer* layer);
	void onPauseLayer(PauseLayer* layer);
	void onMenuLayer(MenuLayer* layer);

	static PatchHacksPageManager* getInstance();

};