#pragma once

#include <vector>

namespace inst::config {
    static const std::string appDir = "sdmc:/config/AtmoXL-Titel-Installer";
    static const std::string configPath = appDir + "/config.json";
    static const std::string appVersion = "1.4.3";

    extern std::string gAuthKey;
    extern std::string lastNetUrl;
    extern int languageSetting;
    extern bool ignoreReqVers;
    extern bool validateNCAs;
    extern bool overClock;
    extern bool deletePrompt;
    extern bool usbAck;

    void setConfig();
    void parseConfig();
}