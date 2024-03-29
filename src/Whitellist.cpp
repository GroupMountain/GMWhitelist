#include "Global.h"

nlohmann::json mWhiteList;

void saveWhitelistFile() {
    std::string path = "./whitelist.json";
    GMLIB::Files::JsonFile::writeFile(path, mWhiteList);
}

void initDataFile() {
    auto emptyFile = nlohmann::json::array();
    try {
        mWhiteList = GMLIB::Files::JsonFile::initJson("./whitelist.json", emptyFile);
    } catch (...) {
        mWhiteList = emptyFile;
        logger.error(tr("error.fileIsBroken", {"'whitelist.json'"}));
        saveWhitelistFile();
    }
}

bool isInWhitelist(std::string& uuid, std::string& name) {
    for (auto& key : mWhiteList) {
        if (key.contains("uuid")) {
            if (key["uuid"] == uuid) {
                return true;
            }
        } else {
            if (key["name"] == name) {
                key["uuid"] = uuid;
                saveWhitelistFile();
                return true;
            }
        }
    }
    return false;
}

bool isNameInWhitelist(std::string& name) {
    for (auto& key : mWhiteList) {
        if (key.contains("name")) {
            if (key["name"] == name) {
                return true;
            }
        }
    }
    return false;
}

bool isUuidInWhitelist(std::string& uuid) {
    for (auto& key : mWhiteList) {
        if (key.contains("uuid")) {
            if (key["uuid"] == uuid) {
                return true;
            }
        }
    }
    return false;
}

bool addPlayer(std::string& name) {
    if (isNameInWhitelist(name)) {
        return false;
    }
    auto key    = nlohmann::json::object();
    key["name"] = name;
    mWhiteList.push_back(key);
    saveWhitelistFile();
    return true;
}

bool removePlayer(std::string& name) {
    auto pl = ll::service::getLevel()->getPlayer(name);
    if (pl) {
        auto msg = tr("disconnect.notAllowed");
        pl->disconnect(msg);
    }
    for (auto it = mWhiteList.begin(); it != mWhiteList.end(); ++it) {
        if (it.value().at("name") == name) {
            mWhiteList.erase(it);
            --it;
            saveWhitelistFile();
            return true;
        }
    }
    return false;
}

void showWhitelist(CommandOutput& output) {
    if (mWhiteList.empty()) {
        return output.error(tr("command.whitelist.noInfo"));
    }
    output.success(tr("command.whitelist.showInfo"));
    for (auto info : mWhiteList) {
        std::string uuid;
        if (info.contains("uuid")) {
            uuid = info["uuid"].get<std::string>();
        }
        std::string name;
        if (info.contains("name")) {
            name = info["name"].get<std::string>();
        }
        output.success(tr("command.whitelist.whitelistInfo", {name, uuid}));
    }
}

void listenEvent() {
    auto& eventBus = ll::event::EventBus::getInstance();
    eventBus.emplaceListener<GMLIB::Event::PacketEvent::ClientLoginAfterEvent>(
        [](GMLIB::Event::PacketEvent::ClientLoginAfterEvent& event) {
            auto uuid     = event.getUuid().asString();
            auto realName = event.getRealName();
            if (!isInWhitelist(uuid, realName)) {
                auto msg = tr("disconnect.notAllowed");
                event.disConnectClient(msg);
            }
        },
        ll::event::EventPriority::High
    );
}

LL_AUTO_STATIC_HOOK(
    FuckVanillaCmd,  // Vinalla Command Registry
    ll::memory::HookPriority::Normal, 
    "?setup@AllowListCommand@@SAXAEAVCommandRegistry@@AEAVAllowListFile@@@Z", 
    void, 
    class CommandRegistry& registry, 
    class AllowListFile&
) {
    return;
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    DisableVanillaAllowlist, // server.properties
    ll::memory::HookPriority::Normal,
    PropertiesSettings,
    "?useAllowList@PropertiesSettings@@QEBA_NXZ",
    bool
) {
    return false;
}