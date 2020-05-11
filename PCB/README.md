# OBDShiftIndicator PCBs
There are two PCBs:

  - OBDMainboard: The Mainboard containing the ESP32, OBD-Connector, Powermanagement and CAN-Tranciever

![](Pictures/MainboardSchematic.png)

  - Shiftlight: Contains the LEDs for the Shiftlight

![](Pictures/ShiftlightsSchematic.png)

# OBDMainboard

  - BOM is included. I got the OBD-Connector from Aliexpress due to a lack of availability. The project can also be found on EasyEDA

  
  https://easyeda.com/mannsbart.jan/obdshiftindicator-mainboard

# Shiftlights

  - BOM is included. The project can also be found on EasyEDA 
  - A photoresistor and a button can also be placed on the pcb. Currently these features are not supported in the firmware, but can easily be implemented
  - Important: To easily connect the Shiftlight-PCB to the Mainboard-PCB you have to cut an RJ45 Cable in half and solder 3 wires to the Shiftlight-PCB (VCC/GND/WS2812). Please check the RJ45 Pinout of the Mainboard-PCB and the corresponding wire colors

https://easyeda.com/mannsbart.jan/obdshiftindicator-shiftlights