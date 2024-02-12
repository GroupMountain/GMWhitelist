#include "Config.h"
#include "Global.h"
#include "Language.h"

GMLIB::Files::JsonConfig*     Config                 = nullptr;
GMLIB::Files::I18n::LangI18n* Language               = nullptr;
int                           commandPermissionLevel = 4;

void initPlugin() {
    Config = new GMLIB::Files::JsonConfig("./plugins/GMWhitelist/config/config.json", defaultConfig);
    Config->init();
    std::string langPath = "./plugins/GMWhitelist/language/";
    std::string language = Config->getValue<std::string>({"language"}, "zh_CN");
    Language             = new GMLIB::Files::I18n::LangI18n(langPath, language);
    Language->loadLanguage("en_US", defaultLanguage_en_US);
    Language->loadLanguage("zh_CN", defaultLanguage_zh_CN);
    Language->chooseLanguage(language);
    commandPermissionLevel = Config->getValue<int>({"CommandPermissionLevel"}, 4);
    if (commandPermissionLevel < 0 || commandPermissionLevel > 4) {
        Config->setValue<int>({"CommandPermissionLevel"}, 4);
        logger.error(tr("permission.error.invalidLevel"));
    }
    if (commandPermissionLevel == 0) {
        logger.warn(tr("permission.warning.dangerousLevel"));
    }
    initDataFile();
}

std::string tr(std::string key, std::vector<std::string> data) { return Language->translate(key, data); }
