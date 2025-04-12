#pragma once
#include <gmlib/include_all.h>
#include <ila/include_all.h>
#include <sstream>

#define MOD_NAME "GMWhitelist"

extern void initDataFile();
extern void showWhitelist(CommandOutput& output);
extern void RegisterCommands();
extern void listenEvent();

extern bool addPlayer(std::string const& name);
extern bool removePlayer(std::string const& name);