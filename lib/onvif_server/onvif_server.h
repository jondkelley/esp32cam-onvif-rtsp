#pragma once

#include <WiFiServer.h>
#include <WiFiClient.h>
#include <Arduino.h>
#include <OV2640.h>
#include <ArduinoJson.h>
#include <list>

// ONVIF Configuration
#define ONVIF_PORT 8080
#define RTSP_PORT 554

// ONVIF Device Information
#define ONVIF_MANUFACTURER "ESP32-CAM"
#define ONVIF_MODEL "ESP32-CAM-RTSP-ONVIF"
#define ONVIF_FIRMWARE_VERSION "2.0"
#define ONVIF_SERIAL_NUMBER "ESP32CAM"
#define ONVIF_HARDWARE_ID "ESP32-CAM"

class OnvifServer : public WiFiServer
{
public:
    OnvifServer(OV2640 &cam, int port = 8080);
    ~OnvifServer();

    void doLoop();
    size_t num_connected();
    void handleClient(WiFiClient &client);

private:
    OV2640 &cam_;
    std::list<WiFiClient> clients_;
    
    // ONVIF Service handlers
    void handleDeviceService(WiFiClient &client, const String &action, const String &body);
    void handleMediaService(WiFiClient &client, const String &action, const String &body);
    void handleImagingService(WiFiClient &client, const String &action, const String &body);
    
    // Device Service methods
    String getDeviceInformation();
    String getCapabilities();
    String getNetworkInterfaces();
    String getSystemDateAndTime();
    
    // Media Service methods
    String getProfiles();
    String getStreamUri(const String &profileToken);
    String getVideoSources();
    String getVideoSourceConfigurations();
    
    // Imaging Service methods
    String getImagingSettings(const String &videoSourceToken);
    String setImagingSettings(const String &videoSourceToken, const String &settings);
    String getImagingOptions(const String &videoSourceToken);
    
    // Utility methods
    String createSoapResponse(const String &action, const String &body);
    String createSoapFault(const String &faultCode, const String &faultString);
    String parseSoapAction(const String &request);
    String parseSoapBody(const String &request);
    void sendSoapResponse(WiFiClient &client, const String &response);
    
    // Camera settings mapping
    void updateCameraFromOnvifSettings(const JsonDocument &settings);
    JsonDocument getOnvifSettingsFromCamera();
}; 