#include "onvif_server.h"
#include <esp32-hal-log.h>
#include <ArduinoJson.h>
#include <WiFi.h>

OnvifServer::OnvifServer(OV2640 &cam, int port) : WiFiServer(port), cam_(cam)
{
    log_i("Starting ONVIF server on port %d", port);
    WiFiServer::begin();
}

OnvifServer::~OnvifServer()
{
    for (auto &client : clients_) {
        client.stop();
    }
}

void OnvifServer::doLoop()
{
    // Check for new clients
    WiFiClient newClient = accept();
    if (newClient) {
        log_i("New ONVIF client connected");
        clients_.push_back(newClient);
    }
    
    // Handle existing clients
    for (auto it = clients_.begin(); it != clients_.end();) {
        if (!it->connected()) {
            log_i("ONVIF client disconnected");
            it = clients_.erase(it);
        } else {
            handleClient(*it);
            ++it;
        }
    }
}

size_t OnvifServer::num_connected()
{
    return clients_.size();
}

void OnvifServer::handleClient(WiFiClient &client)
{
    if (!client.available()) return;
    
    String request = client.readStringUntil('\n');
    if (request.indexOf("POST") == -1) return;
    
    // Read headers
    while (client.available()) {
        String header = client.readStringUntil('\n');
        if (header == "\r") break;
    }
    
    // Read body
    String body = "";
    while (client.available()) {
        body += (char)client.read();
    }
    
    // Parse SOAP action
    String action = parseSoapAction(body);
    log_i("ONVIF request: %s", action.c_str());
    
    // Route to appropriate service
    if (action.indexOf("Device") != -1) {
        handleDeviceService(client, action, body);
    } else if (action.indexOf("Media") != -1) {
        handleMediaService(client, action, body);
    } else if (action.indexOf("Imaging") != -1) {
        handleImagingService(client, action, body);
    } else {
        sendSoapResponse(client, createSoapFault("soap:Client", "Unsupported action"));
    }
}

void OnvifServer::handleDeviceService(WiFiClient &client, const String &action, const String &body)
{
    String response;
    
    if (action.indexOf("GetDeviceInformation") != -1) {
        response = getDeviceInformation();
    } else if (action.indexOf("GetCapabilities") != -1) {
        response = getCapabilities();
    } else if (action.indexOf("GetNetworkInterfaces") != -1) {
        response = getNetworkInterfaces();
    } else if (action.indexOf("GetSystemDateAndTime") != -1) {
        response = getSystemDateAndTime();
    } else {
        response = createSoapFault("soap:Client", "Unsupported Device action");
    }
    
    sendSoapResponse(client, response);
}

void OnvifServer::handleMediaService(WiFiClient &client, const String &action, const String &body)
{
    String response;
    
    if (action.indexOf("GetProfiles") != -1) {
        response = getProfiles();
    } else if (action.indexOf("GetStreamUri") != -1) {
        // Extract profile token from request
        String profileToken = "Profile_1"; // Default
        response = getStreamUri(profileToken);
    } else if (action.indexOf("GetVideoSources") != -1) {
        response = getVideoSources();
    } else if (action.indexOf("GetVideoSourceConfigurations") != -1) {
        response = getVideoSourceConfigurations();
    } else {
        response = createSoapFault("soap:Client", "Unsupported Media action");
    }
    
    sendSoapResponse(client, response);
}

void OnvifServer::handleImagingService(WiFiClient &client, const String &action, const String &body)
{
    String response;
    String videoSourceToken = "VideoSource_1"; // Default
    
    if (action.indexOf("GetImagingSettings") != -1) {
        response = getImagingSettings(videoSourceToken);
    } else if (action.indexOf("SetImagingSettings") != -1) {
        response = setImagingSettings(videoSourceToken, body);
    } else if (action.indexOf("GetImagingOptions") != -1) {
        response = getImagingOptions(videoSourceToken);
    } else {
        response = createSoapFault("soap:Client", "Unsupported Imaging action");
    }
    
    sendSoapResponse(client, response);
}

String OnvifServer::getDeviceInformation()
{
    String body = R"(
        <tds:GetDeviceInformationResponse>
            <tds:Manufacturer>)" + String(ONVIF_MANUFACTURER) + R"(</tds:Manufacturer>
            <tds:Model>)" + String(ONVIF_MODEL) + R"(</tds:Model>
            <tds:FirmwareVersion>)" + String(ONVIF_FIRMWARE_VERSION) + R"(</tds:FirmwareVersion>
            <tds:SerialNumber>)" + String(ONVIF_SERIAL_NUMBER) + R"(</tds:SerialNumber>
            <tds:HardwareId>)" + String(ONVIF_HARDWARE_ID) + R"(</tds:HardwareId>
        </tds:GetDeviceInformationResponse>)";
    
    return createSoapResponse("GetDeviceInformationResponse", body);
}

String OnvifServer::getCapabilities()
{
    String body = R"(
        <tds:GetCapabilitiesResponse>
            <tds:Capabilities>
                <tt:Device>
                    <tt:XAddr>http://)" + WiFi.localIP().toString() + ":" + String(ONVIF_PORT) + R"(/onvif/device_service</tt:XAddr>
                    <tt:Network>
                        <tt:IPFilter>false</tt:IPFilter>
                        <tt:ZeroConfiguration>false</tt:ZeroConfiguration>
                        <tt:IPVersion6>false</tt:IPVersion6>
                        <tt:DynDNS>false</tt:DynDNS>
                    </tt:Network>
                    <tt:System>
                        <tt:DiscoveryResolve>false</tt:DiscoveryResolve>
                        <tt:DiscoveryBye>false</tt:DiscoveryBye>
                        <tt:RemoteDiscovery>false</tt:RemoteDiscovery>
                        <tt:SystemBackup>false</tt:SystemBackup>
                        <tt:SystemLogging>false</tt:SystemLogging>
                        <tt:FirmwareUpgrade>false</tt:FirmwareUpgrade>
                    </tt:System>
                </tt:Device>
                <tt:Media>
                    <tt:XAddr>http://)" + WiFi.localIP().toString() + ":" + String(ONVIF_PORT) + R"(/onvif/media_service</tt:XAddr>
                    <tt:StreamingCapabilities>
                        <tt:RTPMulticast>false</tt:RTPMulticast>
                        <tt:RTP_TCP>true</tt:RTP_TCP>
                        <tt:RTP_RTSP_TCP>true</tt:RTP_RTSP_TCP>
                    </tt:StreamingCapabilities>
                </tt:Media>
                <tt:Imaging>
                    <tt:XAddr>http://)" + WiFi.localIP().toString() + ":" + String(ONVIF_PORT) + R"(/onvif/imaging_service</tt:XAddr>
                </tt:Imaging>
            </tds:Capabilities>
        </tds:GetCapabilitiesResponse>)";
    
    return createSoapResponse("GetCapabilitiesResponse", body);
}

String OnvifServer::getProfiles()
{
    String body = R"(
        <trt:GetProfilesResponse>
            <trt:Profiles token="Profile_1" fixed="true">
                <tt:Name>Profile_1</tt:Name>
                <tt:VideoSourceConfiguration token="VideoSourceConfig_1">
                    <tt:Name>VideoSourceConfig_1</tt:Name>
                    <tt:UseCount>1</tt:UseCount>
                    <tt:SourceToken>VideoSource_1</tt:SourceToken>
                    <tt:Bounds>
                        <tt:left>0</tt:left>
                        <tt:top>0</tt:top>
                        <tt:width>640</tt:width>
                        <tt:height>480</tt:height>
                    </tt:Bounds>
                </tt:VideoSourceConfiguration>
                <tt:VideoEncoderConfiguration token="VideoEncoderConfig_1">
                    <tt:Name>VideoEncoderConfig_1</tt:Name>
                    <tt:UseCount>1</tt:UseCount>
                    <tt:Encoding>JPEG</tt:Encoding>
                    <tt:Resolution>
                        <tt:Width>640</tt:Width>
                        <tt:Height>480</tt:Height>
                    </tt:Resolution>
                    <tt:Quality>0.8</tt:Quality>
                    <tt:RateControl>
                        <tt:FrameRateLimit>5</tt:FrameRateLimit>
                        <tt:BitrateLimit>0</tt:BitrateLimit>
                    </tt:RateControl>
                </tt:VideoEncoderConfiguration>
            </trt:Profiles>
        </trt:GetProfilesResponse>)";
    
    return createSoapResponse("GetProfilesResponse", body);
}

String OnvifServer::getStreamUri(const String &profileToken)
{
    String body = R"(
        <trt:GetStreamUriResponse>
            <trt:MediaUri>
                <tt:Uri>rtsp://)" + WiFi.localIP().toString() + ":" + String(RTSP_PORT) + R"(/mjpeg/1</tt:Uri>
                <tt:InvalidAfterConnect>false</tt:InvalidAfterConnect>
                <tt:InvalidAfterReboot>false</tt:InvalidAfterReboot>
                <tt:Timeout>PT60S</tt:Timeout>
            </trt:MediaUri>
        </trt:GetStreamUriResponse>)";
    
    return createSoapResponse("GetStreamUriResponse", body);
}

String OnvifServer::createSoapResponse(const String &action, const String &body)
{
    String response = R"(<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope xmlns:soap="http://www.w3.org/2003/05/soap-envelope" xmlns:tds="http://www.onvif.org/ver10/device/wsdl" xmlns:trt="http://www.onvif.org/ver10/media/wsdl" xmlns:tt="http://www.onvif.org/ver10/schema">
    <soap:Body>
        <)" + action + ">" + body + "</" + action + R"(>
    </soap:Body>
</soap:Envelope>)";
    
    return response;
}

String OnvifServer::createSoapFault(const String &faultCode, const String &faultString)
{
    String response = R"(<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope xmlns:soap="http://www.w3.org/2003/05/soap-envelope">
    <soap:Body>
        <soap:Fault>
            <soap:Code>
                <soap:Value>)" + faultCode + R"(</soap:Value>
            </soap:Code>
            <soap:Reason>
                <soap:Text>)" + faultString + R"(</soap:Text>
            </soap:Reason>
        </soap:Fault>
    </soap:Body>
</soap:Envelope>)";
    
    return response;
}

String OnvifServer::parseSoapAction(const String &request)
{
    int start = request.indexOf("SOAPAction:");
    if (start == -1) return "";
    
    start = request.indexOf("\"", start);
    if (start == -1) return "";
    
    int end = request.indexOf("\"", start + 1);
    if (end == -1) return "";
    
    return request.substring(start + 1, end);
}

String OnvifServer::parseSoapBody(const String &request)
{
    int start = request.indexOf("<soap:Body>");
    if (start == -1) return "";
    
    start = request.indexOf(">", start) + 1;
    int end = request.indexOf("</soap:Body>");
    if (end == -1) return "";
    
    return request.substring(start, end);
}

void OnvifServer::sendSoapResponse(WiFiClient &client, const String &response)
{
    String httpResponse = "HTTP/1.1 200 OK\r\n";
    httpResponse += "Content-Type: application/soap+xml; charset=utf-8\r\n";
    httpResponse += "Content-Length: " + String(response.length()) + "\r\n";
    httpResponse += "\r\n";
    httpResponse += response;
    
    client.print(httpResponse);
}

// Placeholder implementations for other required methods
String OnvifServer::getNetworkInterfaces() { return createSoapFault("soap:Server", "Not implemented"); }
String OnvifServer::getSystemDateAndTime() { return createSoapFault("soap:Server", "Not implemented"); }
String OnvifServer::getVideoSources() { return createSoapFault("soap:Server", "Not implemented"); }
String OnvifServer::getVideoSourceConfigurations() { return createSoapFault("soap:Server", "Not implemented"); }
String OnvifServer::getImagingSettings(const String &videoSourceToken) { return createSoapFault("soap:Server", "Not implemented"); }
String OnvifServer::setImagingSettings(const String &videoSourceToken, const String &settings) { return createSoapFault("soap:Server", "Not implemented"); }
String OnvifServer::getImagingOptions(const String &videoSourceToken) { return createSoapFault("soap:Server", "Not implemented"); } 