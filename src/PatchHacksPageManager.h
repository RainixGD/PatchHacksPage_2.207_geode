#pragma once
#include "./includes.h"

class PatchHack;

class PatchHacksPageManager {

	enum DataLoadingResult {
		OK,
		FileNotFound,
		ParsingError,
		PatchFormatError,
	};

	std::vector<std::string> activeLayers;
	std::vector<PatchHack*> hacks;
	DataLoadingResult loadingStatus;
	static PatchHacksPageManager* instance;

	void init();

	DataLoadingResult loadData();

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