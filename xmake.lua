add_rules("mode.debug", "mode.release", "mode.releasedbg")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")
add_repositories("groupmountain-repo https://github.com/GroupMountain/xmake-repo.git")
add_repositories("miracleforest https://github.com/MiracleForest/xmake-repo")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

add_requires("levilamina")
add_requires("levibuildscript")
add_requires("gmlib")
add_requires("ilistenattentively")

target("GMWhitelist") -- Change this to your plugin name.
    add_cxflags( "/EHa", "/utf-8", "/W4", "/w44265", "/w44289", "/w44296", "/w45263", "/w44738", "/w45204")
    add_defines("NOMINMAX", "UNICODE")
    add_files(
        "src/**.cpp",
        "src/**.rc"
    )
    add_includedirs(
        "src"
    )
    add_packages(
        "levilamina",
		"gmlib",
        "ilistenattentively"
    )
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")
    set_exceptions("none") -- To avoid conflicts with /EHa
    set_kind("shared")
    set_languages("cxx20")
    set_symbols("debug")
