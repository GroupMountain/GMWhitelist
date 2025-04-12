#include "Entry.h"
#include "resource.h"

namespace GMWhitelist {

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

bool Entry::load() { return true; }

bool Entry::enable() {
    mConfig.emplace();
    if (!ll::config::loadConfig(*mConfig, getSelf().getConfigDir() / u8"config.json")) {
        ll::config::saveConfig(*mConfig, getSelf().getConfigDir() / u8"config.json");
    }
    mI18n.emplace(getSelf().getLangDir(), mConfig->language);
    mI18n->updateOrCreateLanguage("en_US", en_US);
    mI18n->updateOrCreateLanguage("zh_CN", zh_CN);
    mI18n->loadAllLanguages();
    mI18n->chooseLanguage(mConfig->language);
    auto& commandPermissionLevel = mConfig->CommandPermissionLevel;
    if (commandPermissionLevel < 0 || commandPermissionLevel > 4) {
        mConfig->CommandPermissionLevel = 4;
        getSelf().getLogger().error("permission.error.invalidLevel"_tr());
    }
    if (commandPermissionLevel == 0) {
        getSelf().getLogger().warn("permission.warning.dangerousLevel"_tr());
    }
    initDataFile();
    RegisterCommands();
    listenEvent();
    getSelf().getLogger().info("GMWhitelist Loaded!");
    getSelf().getLogger().info("Author: KobeBryant114514");
    getSelf().getLogger().info("Repository: https://github.com/GroupMountain/GMWhitelist");
    return true;
}

bool Entry::disable() {
    mConfig.reset();
    return true;
}

Config& Entry::getConfig() { return mConfig.value(); }

gmlib::i18n::LangI18n& Entry::getI18n() { return *mI18n; }



} // namespace GMWhitelist

LL_REGISTER_MOD(GMWhitelist::Entry, GMWhitelist::Entry::getInstance());
