#include "Entry.h"
#include "Global.h"

struct WhitelistParam {
    enum class Action { add, remove } action;
    std::string name;
};

void RegWhitelistCmd() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        "whitelist",
        "command.whitelist.desc"_tr(),
        (CommandPermissionLevel)GMWhitelist::Entry::getInstance().getConfig().CommandPermissionLevel
    );
    ll::service::getCommandRegistry()->registerAlias("whitelist", "allowlist");
    cmd.overload<WhitelistParam>()
        .required("action")
        .required("name")
        .execute([](CommandOrigin const& origin, CommandOutput& output, WhitelistParam const& param) {
            auto type = origin.getOriginType();
            if (type == CommandOriginType::DedicatedServer || type == CommandOriginType::Player) {
                auto player = param.name;
                if (magic_enum::enum_name(param.action) == "add") {
                    auto res = addPlayer(player);
                    if (res) {
                        return output.success("command.whitelist.addSuccess"_tr(player));
                    }
                    return output.error("command.whitelist.isInWhitelist"_tr(player));
                } else {
                    auto res = removePlayer(player);
                    if (res) {
                        return output.success("command.whitelist.removeSuccess"_tr(player));
                    }
                    return output.error("command.whitelist.notInWhitelist"_tr(player));
                }
            }
            return output.error("command.error.invalidCommandOrigin"_tr());
        });
    cmd.overload<WhitelistParam>()
        .text("reload")
        .execute([](CommandOrigin const& origin, CommandOutput& output, WhitelistParam const& param) {
            auto type = origin.getOriginType();
            if (type == CommandOriginType::DedicatedServer || type == CommandOriginType::Player) {
                initDataFile();
                return output.success("command.whitelist.reload"_tr());
            }
            return output.error("command.error.invalidCommandOrigin"_tr());
        });
    cmd.overload<WhitelistParam>()
        .text("list")
        .execute([](CommandOrigin const& origin, CommandOutput& output, WhitelistParam const& param) {
            auto type = origin.getOriginType();
            if (type == CommandOriginType::DedicatedServer || type == CommandOriginType::Player) {
                return showWhitelist(output);
            }
            return output.error("command.error.invalidCommandOrigin"_tr());
        });
}

void RegisterCommands() { RegWhitelistCmd(); }