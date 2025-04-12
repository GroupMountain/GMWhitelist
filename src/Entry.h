#pragma once
#include "Config.h"
#include "Global.h"

namespace GMWhitelist {


class Entry {

public:
    static Entry& getInstance();

    Entry() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    /// @return True if the mod is loaded successfully.
    bool load();

    /// @return True if the mod is enabled successfully.
    bool enable();

    /// @return True if the mod is disabled successfully.
    bool disable();

    Config& getConfig();

    gmlib::i18n::LangI18n& getI18n();

private:
    ll::mod::NativeMod& mSelf;
    std::optional<Config>     mConfig;
    std::optional<gmlib::i18n::LangI18n>   mI18n;
};

} // namespace GMWhitelist

LANGI18N_LITERALS(GMWhitelist::Entry::getInstance().getI18n())