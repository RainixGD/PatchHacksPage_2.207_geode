#include "./PatchHacksPageManager.h"
#include "./PatchHack.h"
#include "./PatchHacksPopup.h"
#include "./BytePatch.h"

void PatchHacksPageManager::init() {
	loadingStatus = loadData();
	if (loadingStatus == OK)
		loadStatesFile();
}

PatchHacksPageManager::DataLoadingResult PatchHacksPageManager::loadData() {

	std::ifstream file("Resources/hackPanel.json");
	if (!file) return FileNotFound;
	std::ostringstream buffer;
	buffer << file.rdbuf();
	std::string fileContent = buffer.str();

	file.close();

	try {
		auto root = nlohmann::json::parse(fileContent);

		if (!root.contains("activeLayers") || !root["activeLayers"].is_array() ||
			!root.contains("hacks") || !root["hacks"].is_array()) {
			return ParsingError;
		}

		for (const auto& layer : root["activeLayers"]) {
			if (!layer.is_string()) return ParsingError;
			activeLayers.push_back(layer.get<std::string>());
		}

		auto hacks = root["hacks"];

		if (hacks.size() == 0) return EmptyMenuError;

		for (const auto& hack : hacks) {
			if (!hack.contains("name") || !hack["name"].is_string() ||
				!hack.contains("patches") || !hack["patches"].is_array()) {
				return ParsingError;
			}

			auto patches = hack["patches"];
			std::vector<BytePatch*> patchesVector;

			for (const auto& patch : patches) {
				if (!patch.contains("address") || !patch["address"].is_string() ||
					!patch.contains("on") || !patch["on"].is_string()) {
					return ParsingError;
				}

				std::string address = patch["address"];
				std::string on = patch["on"];
				std::optional<std::string> off = (patch.contains("off") && patch["off"].is_string())
					? std::optional<std::string>(patch["off"])
					: std::nullopt;

				auto newPatch = BytePatch::create(address, on, off);
				if (!newPatch) return PatchFormatError;
				patchesVector.push_back(newPatch);
			}

			auto patchHack = PatchHack::create(hack["name"], patchesVector);
			this->hacks.push_back(patchHack);
		}
	}
	catch (const nlohmann::json::exception& e) {
		return ParsingError;
	}
	catch (...) {
		return ParsingError;
	}

	return OK;
}

void PatchHacksPageManager::loadStatesFile() {
	std::ifstream inFile("Resources/hackPanel.bin", std::ios::binary);
	if (!inFile) return;

	size_t i = 0;
	while (inFile && i < hacks.size()) {
		bool value;
		inFile.read(reinterpret_cast<char*>(&value), sizeof(value));

		if (inFile.gcount() < sizeof(value)) break;

		if (value) hacks[i]->toggle();
		++i;
	}
}

void PatchHacksPageManager::saveStatesFile() {
	std::ofstream outFile("Resources/hackPanel.bin", std::ios::binary);
	if (!outFile) return;

	for (auto hack : hacks) {
		bool value = hack->getActive();
		outFile.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}

	outFile.close();
}

void PatchHacksPageManager::buildPatchHacksButton(CCLayer* layer) {
	auto modSprite = CCSprite::create("GJ_ModBtn.png");
	if (modSprite == NULL) {
		modSprite = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
		modSprite->setScale(1.5);
	}
	else
		modSprite->setScale(0.6);
	auto modBtn = CCMenuItemSpriteExtra::create(modSprite, layer, menu_selector(PatchHacksPageManager::onPatchHacksBtn));
	modBtn->setPosition({ float(float(-1 * CCDirector::sharedDirector()->getWinSize().width) / float(2.4)), CCDirector::sharedDirector()->getWinSize().height / 6 });
	auto modMenu = CCMenu::create();
	modMenu->addChild(modBtn);
	layer->addChild(modMenu, 5, 22);
}

void PatchHacksPageManager::onPatchHacksBtn(CCObject*) {
	PatchHacksPopup::create(PatchHacksPageManager::getInstance()->hacks)->show();
}

void PatchHacksPageManager::onTrySaveGame() {
	saveStatesFile();
}

void PatchHacksPageManager::onEditorPauseLayer(CCLayer* layer) {
	if (std::find(activeLayers.begin(), activeLayers.end(), "EditorPauseLayer") == activeLayers.end()) return;
	buildPatchHacksButton(layer);
}

void PatchHacksPageManager::onPauseLayer(PauseLayer* layer) {
	if (std::find(activeLayers.begin(), activeLayers.end(), "PauseLayer") == activeLayers.end()) return;
	buildPatchHacksButton(layer);
}

void PatchHacksPageManager::onMenuLayer(MenuLayer* layer) {

	if (loadingStatus != OK) {

		std::string errorText;
		switch (loadingStatus) {
		case FileNotFound:
			errorText = "Can't find 'hackPanel.json' in ./Resources";
			break;
		case ParsingError:
			errorText = "Can't parse 'hackPanel.json'";
			break;
		case PatchFormatError:
			errorText = "Can't parse patch format";
			break;
		case EmptyMenuError:
			errorText = "There are no patchhacks to load";
			break;
		}

		auto size = CCDirector::sharedDirector()->getWinSize();

		auto errorLabel = CCLabelBMFont::create(errorText.c_str(), "bigFont.fnt");
		errorLabel->setColor({ 255, 0, 0 });
		errorLabel->setScale(0.4);
		errorLabel->setPosition({ size.width / 2, size.height - 10 });
		layer->addChild(errorLabel);

		return;
	}

	if (std::find(activeLayers.begin(), activeLayers.end(), "MenuLayer") == activeLayers.end()) return;
	buildPatchHacksButton(layer);
}

PatchHacksPageManager* PatchHacksPageManager::getInstance() {
	if (!instance) {
		instance = new PatchHacksPageManager();
		instance->init();
	}
	return instance;
}

PatchHacksPageManager* PatchHacksPageManager::instance = nullptr;