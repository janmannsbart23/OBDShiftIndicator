#pragma once

#define SSID "YourAPSSID"
#define PASSWORD "YourPW"

#if IS_OTA_FUNCTIONALITY_ENABLED == true
#error "Wifi Credentials must be defined!" //Comment out if your credentials are set
#endif