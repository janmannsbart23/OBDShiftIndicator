# Mainboard Code
- Import the project in PlatformIO
- If you want to have OTA update functionality, go to "CWifiCredentials.hpp" and enter the Wifi credentials for the wifi it should connects to. Currently only client mode is implemented. After that, open "DConfig.hpp" and set "IS_OTA_FUNCTIONALITY_ENABLED" to true
- Upload the Project to the ESP32 via the onboard USB2Serial Chip. (Very lickely that you have to adjust the COM-Port in the platformio.ini)
- The code also saves the brightness values and if the shifter is activated in eeprom. This can be changed via the android app. After the first time uploading, there will me most likely 0xFF in the eeprom, which means the shifter is deactivated and will never light up. This can be changed via the app. If you dont want to use the app, you can override the values via the "DConfig.hpp" file. Set "FIRST_LAUNCH_SETTINGS_OVERRIDE" to true and upload the code. Connect with a terminal to the COM-Port and check the response. If it was successful, change the value back to "false". Now it will work as intended.

- The Code waits until a rpm of at least 500 is detected to ensure that the motor was started. After that, a animation with blue lights will show up on your shiftlights. After that, it waits until an engine oil temperature of 90 degree is reached (to be sure ur engine is hot enough :) ). A green led animation will show up. After that the shiftlight-mode is active and will light up depending on your rpms. Those values can be changed in "CShiftLedTask.cpp"

# Android APP Code
- The Android App is under heavy construction at the moment. It's an proof of concept and working fine at the moment. The design is just not great... If you can design nice apps, please contact me :D
