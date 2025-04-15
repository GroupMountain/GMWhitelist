#include "Entry.h"
#include "Global.h"

std::unordered_map<mce::UUID, std::string> mWhiteListMap;
std::unordered_set<std::string>            mNameCache;

void saveWhitelistFile() {
    std::string    path = "./whitelist.json";
    nlohmann::json data;
    for (auto& [uuid, name] : mWhiteListMap) {
        auto info    = nlohmann::json::object();
        info["uuid"] = uuid.asString();
        info["name"] = name;
        data.push_back(info);
    }
    for (auto& name : mNameCache) {
        auto info    = nlohmann::json::object();
        info["name"] = name;
        data.push_back(info);
    }
    gmlib::json_utils::writeFile(path, data);
}

void initDataFile() {
    auto data = gmlib::json_utils::initJson("./whitelist.json", nlohmann::json::array());
    for (auto& info : data) {
        if (info.contains("uuid")) {
            std::string uuid                           = info["uuid"];
            mWhiteListMap[mce::UUID::fromString(uuid)] = info["name"];
        } else {
            mNameCache.insert(info["name"]);
        }
    }
}

bool isInWhitelist(mce::UUID const& uuid, std::string const& name) {
    if (mWhiteListMap.contains(uuid)) {
        if (mWhiteListMap[uuid] != name) {
            mWhiteListMap[uuid] = name;
            saveWhitelistFile();
        }
        return true;
    }
    if (mNameCache.contains(name)) {
        mWhiteListMap[uuid] = name;
        mNameCache.erase(name);
        saveWhitelistFile();
        return true;
    }
    return false;
}

bool isNameInWhitelist(std::string const& name) {
    if (mNameCache.contains(name)) {
        return true;
    }
    if (auto uuid = gmlib::UserCache::getUuidByName(name)) {
        if (mWhiteListMap.contains(uuid.value())) {
            return true;
        }
    }
    return false;
}

bool addPlayer(std::string const& name) {
    if (isNameInWhitelist(name)) {
        return false;
    }
    if (auto uuid = gmlib::UserCache::getUuidByName(name)) {
        mWhiteListMap[uuid.value()] = name;
    } else {
        mNameCache.insert(name);
    }
    saveWhitelistFile();
    return true;
}

bool removePlayer(std::string const& name) {
    auto pl = ll::service::getLevel()->getPlayer(name);
    if (pl) {
        auto msg = "disconnect.notAllowed"_tr();
        pl->disconnect(msg);
    }
    for (auto& [uuid, playername] : mWhiteListMap) {
        if (playername == name) {
            mWhiteListMap.erase(uuid);
            saveWhitelistFile();
            return true;
        }
    }
    for (auto& playername : mNameCache) {
        if (playername == name) {
            mNameCache.erase(name);
            saveWhitelistFile();
            return true;
        }
    }
    return false;
}

void showWhitelist(CommandOutput& output) {
    if (mWhiteListMap.empty()) {
        return output.error("command.whitelist.noInfo"_tr());
    }
    output.success("command.whitelist.showInfo"_tr());
    for (auto& [uuid, name] : mWhiteListMap) {
        output.success("command.whitelist.whitelistInfo"_tr(name, uuid.asString()));
    }
    for (auto& name : mNameCache) {
        output.success("command.whitelist.whitelistInfo"_tr(name, ""));
    }
}

void listenEvent() {
    auto& eventBus = ll::event::EventBus::getInstance();
    eventBus.emplaceListener<ila::mc::ClientLoginAfterEvent>([](ila::mc::ClientLoginAfterEvent& event) {
        if (event.clientAuthXuid().empty()) {
            return event.disConnectClient("disconnect.clientNotAuth"_tr());
        }
        if (event.serverAuthXuid().empty() && ll::service::getPropertiesSettings()->mIsOnlineMode) {
            return event.disConnectClient("disconnectionScreen.notAuthenticated"_tr());
        }
        auto uuid     = event.uuid();
        auto realName = event.realName();
        if (!isInWhitelist(uuid, realName)) {
            auto msg = "disconnect.notAllowed"_tr(realName);
            event.disConnectClient(msg);
            if (GMWhitelist::Entry::getInstance().getConfig().ConsoleOutput) {
                ll::io::LoggerRegistry::getInstance().getOrCreate(MOD_NAME)->info("logger.notAllowed"_tr(realName));
            }
        }
    });
}

LL_AUTO_STATIC_HOOK(
    FuckVanillaCmd, // Vinalla Command Registry
    ll::memory::HookPriority::Normal,
    &AllowListCommand::setup,
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
    &PropertiesSettings::$ctor,
    void*,
    ::std::string const& filename
) {
    auto res                                                  = origin(filename);
    reinterpret_cast<PropertiesSettings*>(res)->mUseAllowList = false;
    return res;
}