#include <Arduino.h>
#include "Ota.h"
#include "Logger.h"
#include "RestartReason.h"

#define FULL_PACKET 1436 // HTTP_UPLOAD_BUFLEN in WebServer,h

void Ota::updateFirmware(uint8_t* buf, size_t size)
{
    if(!_updateStarted && size == 0)
    {
        Log->println("OTA upload cancelled, size is 0.");
        return;
    }

    if (!_updateStarted)
    { //If it's the first packet of OTA since bootup, begin OTA
        Log->println("BeginOTA");
        esp_ota_begin(esp_ota_get_next_update_partition(NULL), OTA_SIZE_UNKNOWN, &otaHandler);
        _updateStarted = true;
    }
    esp_ota_write(otaHandler, buf, size);
    if (size != FULL_PACKET)
    {
        esp_ota_end(otaHandler);
        Log->println("EndOTA");
        if (ESP_OK == esp_ota_set_boot_partition(esp_ota_get_next_update_partition(NULL)))
        {
            delay(2000);
            restartEsp(RestartReason::OTACompleted);
        }
        else
        {
            Log->println("Upload Error");
        }
    }
}

bool Ota::updateStarted()
{
    return _updateStarted;
}
