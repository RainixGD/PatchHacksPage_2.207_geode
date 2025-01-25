#include "./includes.h"
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/AppDelegate.hpp>

#include "./PatchHacksPageManager.h"
#include "./ErrorsManager/ErrorsManager.h"

class $modify(MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		PatchHacksPageManager::getInstance()->onMenuLayer(this);
		ErrorsManager::onMenuLayer(this);
		return true;
	}
};

class $modify(PauseLayer) {
	void customSetup() {
		PauseLayer::customSetup();
		PatchHacksPageManager::getInstance()->onPauseLayer(this);
	}
};

class $modify(EditorPauseLayer) {
	bool init(LevelEditorLayer* p0) {
		if (!EditorPauseLayer::init(p0)) return false;
		PatchHacksPageManager::getInstance()->onEditorPauseLayer(this);
		return true;
	}
};

class $modify(AppDelegate) {
	void trySaveGame(bool p0) {
		AppDelegate::trySaveGame(p0);
		PatchHacksPageManager::getInstance()->onTrySaveGame();
	}
};

$execute{
	PatchHacksPageManager::getInstance();// to patch the game before gamemanager init (to fix the unlock icons saving)
}