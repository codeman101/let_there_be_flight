// LTBF R42a (macOS): a stand-in dependency plugin for deps_test.cpp. Only Query(), reporting STUB_MAJOR.STUB_MINOR.STUB_PATCH.
#include <RED4ext/Api/v1/SemVer.hpp>
#include <RED4ext/Api/v1/PluginInfo.hpp>

extern "C" __attribute__((visibility("default"))) void Query(RED4ext::v1::PluginInfo* aInfo)
{
    aInfo->version = RED4EXT_V1_SEMVER(STUB_MAJOR, STUB_MINOR, STUB_PATCH);
}
