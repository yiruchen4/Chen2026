## Choose the SocialDoor code that fits your needs

**SocialDoor_102225**  
Recommended for long-term experiments. This version includes a sleep mode to conserve power. Proximity data are recorded only when the door is triggered and continue until the door closes.

**SocialDoor_prox_on_alltime_020326**  
Recommended for continuous recording. This version records proximity data at all times, regardless of whether the door is open or closed. Because the sensors are always active, battery life will be shorter. Please monitor the battery level frequently and replace batteries when they become low.

**pcf8523_modified_code_for_door**  *(required for all cases)*  
Used to program the PCF8523 real-time clock module. The PCF8523 requires a coin-cell battery for power. The clock should be flashed before each use because its internal time may not match the actual time. We recommend flashing the PCF8523 and the door device (Adafruit Feather M0 Adalogger) **separately** before connecting the PCF8523 to the door device.
