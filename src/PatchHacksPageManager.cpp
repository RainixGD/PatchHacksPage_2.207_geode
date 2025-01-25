#include "./PatchHacksPageManager.h"
#include "./PatchHack.h"
#include "./PatchHacksPopup.h"
#include "./BytePatch.h"
#include "./ErrorsManager/ErrorsManager.h"

void PatchHacksPageManager::init() {
	isOk = loadData();
	if (isOk)
		loadStatesFile();
}

bool PatchHacksPageManager::loadData() {
    std::ifstream file("Resources/hackPanel.json");
    if (!file) {
        ErrorsManager::addError("Patch Hacks Page: File 'Resources/hackPanel.json' not found or unable to open.", ErrorsManager::Error);
        return false;
    }

    std::ostringstream buffer;
    try {
        buffer << file.rdbuf();
    }
    catch (const std::ios_base::failure& e) {
        ErrorsManager::addError("Patch Hacks Page: Failed to read from file 'Resources/hackPanel.json'. IOError: " + std::string(e.what()), ErrorsManager::Error);
        file.close();
        return false;
    }

    std::string fileContent = buffer.str();
    file.close();

    if (fileContent.empty()) {
        ErrorsManager::addError("Patch Hacks Page: 'Resources/hackPanel.json' is empty.", ErrorsManager::Error);
        return false;
    }

    try {
        auto root = nlohmann::json::parse(fileContent);

        if (!root.contains("activeLayers") || !root["activeLayers"].is_array() ||
            !root.contains("hacks") || !root["hacks"].is_array()) {
            ErrorsManager::addError("Patch Hacks Page: JSON does not contain required 'activeLayers' or 'hacks' properties, or they are not arrays.", ErrorsManager::Error);
            return false;
        }

        for (const auto& layer : root["activeLayers"]) {
            if (!layer.is_string()) {
                ErrorsManager::addError("Patch Hacks Page: Each 'activeLayer' must be a string.", ErrorsManager::Error);
                return false;
            }
            activeLayers.push_back(layer.get<std::string>());
        }

        auto hacks = root["hacks"];
        if (hacks.size() == 0) {
            ErrorsManager::addError("Patch Hacks Page: No hacks found in the JSON.", ErrorsManager::Error);
            return false;
        }

        for (const auto& hack : hacks) {
            if (!hack.contains("name") || !hack["name"].is_string() ||
                !hack.contains("patches") || !hack["patches"].is_array()) {
                ErrorsManager::addError("Patch Hacks Page: Missing or invalid properties in hack (name or patches).", ErrorsManager::Error);
                return false;
            }

            auto patches = hack["patches"];
            std::vector<BytePatch*> patchesVector;

            for (const auto& patch : patches) {
                if (!patch.contains("address") || !patch["address"].is_string() ||
                    !patch.contains("on") || !patch["on"].is_string()) {
                    ErrorsManager::addError("Patch Hacks Page: Missing or invalid properties in patch (address or on bytes).", ErrorsManager::Error);
                    return false;
                }

                std::string address = patch["address"];
                std::string on = patch["on"];
                std::optional<std::string> off = (patch.contains("off") && patch["off"].is_string())
                    ? std::optional<std::string>(patch["off"])
                    : std::nullopt;

                auto newPatch = BytePatch::create(address, on, off);
                if (!newPatch) {
                    ErrorsManager::addError("Patch Hacks Page: Failed to create patch from provided data (address: " + address + ").", ErrorsManager::Error);
                    return false;
                }
                patchesVector.push_back(newPatch);
            }

            auto patchHack = PatchHack::create(hack["name"], patchesVector);
            if (!patchHack) {
                ErrorsManager::addError("Patch Hacks Page: Failed to create patch hack for " + hack["name"].get<std::string>() + ".", ErrorsManager::Error);
                return false;
            }
            this->hacks.push_back(patchHack);
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        ErrorsManager::addError("Patch Hacks Page: JSON parse error. Exception: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }
    catch (const nlohmann::json::type_error& e) {
        ErrorsManager::addError("Patch Hacks Page: JSON type error while processing data. Exception: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }
    catch (const std::bad_alloc& e) {
        ErrorsManager::addError("Patch Hacks Page: Memory allocation error. Exception: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }
    catch (const std::ios_base::failure& e) {
        ErrorsManager::addError("Patch Hacks Page: I/O operation failure. Exception: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }
    catch (const std::exception& e) {
        ErrorsManager::addError("Patch Hacks Page: Unknown error occurred. Exception: " + std::string(e.what()), ErrorsManager::Error);
        return false;
    }

    return true;
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
	if (!isOk || std::find(activeLayers.begin(), activeLayers.end(), "EditorPauseLayer") == activeLayers.end()) return;
	buildPatchHacksButton(layer);
}

void PatchHacksPageManager::onPauseLayer(PauseLayer* layer) {
	if (!isOk || std::find(activeLayers.begin(), activeLayers.end(), "PauseLayer") == activeLayers.end()) return;
	buildPatchHacksButton(layer);
}

void PatchHacksPageManager::onMenuLayer(MenuLayer* layer) {
	if (!isOk || std::find(activeLayers.begin(), activeLayers.end(), "MenuLayer") == activeLayers.end()) return;
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