# ESP32-CAM RTSP + ONVIF Profile S Server

A comprehensive ESP32-CAM firmware that provides both RTSP streaming and ONVIF Profile S compliance, making it compatible with UniFi Protect, Blue Iris, and other ONVIF-compliant systems.

## Features

### RTSP Streaming
- Real-time video streaming via RTSP protocol
- Configurable frame rate, resolution, and JPEG quality
- Multiple simultaneous client connections
- MJPEG over RTSP support

### ONVIF Profile S Compliance
- **Device Service**: Device information and capabilities discovery
- **Media Service**: Video stream configuration and profile management
- **Imaging Service**: Camera settings control (brightness, contrast, etc.)
- Full ONVIF Profile S specification compliance
- Compatible with UniFi Protect, Blue Iris, and other ONVIF systems

### Camera Configuration
- Extensive camera parameter control
- Real-time settings adjustment
- Web-based configuration interface
- Automatic camera initialization with retry logic

### Network Features
- WiFi configuration via captive portal
- mDNS service discovery
- IPv4 and IPv6 support
- Configurable network settings

## Supported Boards

This firmware supports various ESP32-CAM boards:

- AI Thinker ESP32-CAM
- ESP-EYE
- ESP32-S2 Camera Board
- ESP32-S3 Camera LCD
- ESP32-S3 Eye
- M5Stack Camera variants
- TTGO T-Camera
- Seeed XIAO ESP32S3 Sense
- And more...

## Quick Start

1. **Flash the firmware** to your ESP32-CAM board
2. **Connect to WiFi** - The device will create an access point named "ESP32CAM-RTSP-ONVIF"
3. **Configure WiFi** - Visit the device's IP address to configure your network
4. **Access streams**:
   - RTSP: `rtsp://[IP]:554/mjpeg/1`
   - ONVIF: Available on port 8080

## ONVIF Integration

### Device Discovery
The ESP32-CAM will appear as an ONVIF-compliant device with the following information:
- Manufacturer: ESP32-CAM
- Model: ESP32-CAM-RTSP-ONVIF
- Firmware Version: 2.0
- Serial Number: ESP32CAM

### Supported ONVIF Services

#### Device Service (`http://[IP]:8080/onvif/device_service`)
- `GetDeviceInformation` - Returns device details
- `GetCapabilities` - Returns supported services and capabilities
- `GetNetworkInterfaces` - Network interface information
- `GetSystemDateAndTime` - System time information

#### Media Service (`http://[IP]:8080/onvif/media_service`)
- `GetProfiles` - Returns available video profiles
- `GetStreamUri` - Returns RTSP stream URL for a profile
- `GetVideoSources` - Returns available video sources
- `GetVideoSourceConfigurations` - Video source configuration

#### Imaging Service (`http://[IP]:8080/onvif/imaging_service`)
- `GetImagingSettings` - Current camera imaging settings
- `SetImagingSettings` - Update camera imaging settings
- `GetImagingOptions` - Available imaging options

### UniFi Protect Integration

To add this camera to UniFi Protect:

1. In UniFi Protect, go to **Settings** → **Cameras** → **Add Camera**
2. Select **ONVIF Camera**
3. Enter the camera's IP address
4. Use default ONVIF credentials (if any)
5. The camera should be discovered and added automatically

### Blue Iris Integration

To add this camera to Blue Iris:

1. In Blue Iris, right-click and select **Add Camera**
2. Choose **ONVIF** as the camera type
3. Enter the camera's IP address and port (8080)
4. Blue Iris will automatically discover the camera capabilities
5. Configure the stream settings as needed

## Configuration

### Web Interface
Access the web interface at `http://[IP]` to configure:
- WiFi settings
- Camera parameters (brightness, contrast, etc.)
- Frame rate and resolution
- JPEG quality settings

### Camera Parameters
- **Frame Duration**: Time between frames (ms)
- **Frame Size**: Resolution (VGA, SVGA, etc.)
- **JPEG Quality**: Image quality (1-100)
- **Brightness**: Brightness adjustment (-2 to 2)
- **Contrast**: Contrast adjustment (-2 to 2)
- **Saturation**: Color saturation (-2 to 2)
- **White Balance**: Auto/manual white balance
- **Exposure Control**: Auto/manual exposure
- **Gain Control**: Auto/manual gain control

## API Endpoints

### RTSP Streams
- `rtsp://[IP]:554/mjpeg/1` - Main RTSP stream

### HTTP Endpoints
- `http://[IP]/` - Web interface
- `http://[IP]/config` - Configuration page
- `http://[IP]/snapshot` - Single JPEG snapshot
- `http://[IP]/stream` - MJPEG stream

### ONVIF Endpoints
- `http://[IP]:8080/onvif/device_service` - Device service
- `http://[IP]:8080/onvif/media_service` - Media service
- `http://[IP]:8080/onvif/imaging_service` - Imaging service

## Building

### Prerequisites
- PlatformIO
- ESP32 development tools

### Build Commands
```bash
# Build for specific board
pio run -e esp32cam_ai_thinker

# Build for all boards
pio run

# Upload to device
pio run -e esp32cam_ai_thinker -t upload
```

## Troubleshooting

### Camera Not Initializing
- Check camera module connections
- Verify power supply is adequate
- Try different frame sizes or lower frame rates
- Check serial output for error messages

### ONVIF Discovery Issues
- Ensure port 8080 is not blocked by firewall
- Verify ONVIF client supports Profile S
- Check network connectivity to the device

### RTSP Stream Issues
- Verify RTSP client supports MJPEG
- Check network bandwidth for high frame rates
- Try reducing JPEG quality or frame rate

## Technical Details

### ONVIF Profile S Compliance
This implementation follows the ONVIF Profile S specification for video streaming:
- Device discovery and information
- Media stream configuration
- Imaging settings control
- SOAP-based web services

### Memory Usage
- RTSP server: ~50KB RAM
- ONVIF server: ~30KB RAM
- Camera buffer: ~100KB (configurable)
- Web interface: ~20KB

### Performance
- Maximum frame rate: 30 FPS (depends on resolution)
- Maximum resolution: UXGA (1600x1200)
- Concurrent clients: 5+ (depends on memory)

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## Acknowledgments

- Based on the original ESP32-CAM RTSP project
- Uses Micro-RTSP library for RTSP implementation
- ONVIF Profile S specification compliance
- IotWebConf for WiFi configuration
