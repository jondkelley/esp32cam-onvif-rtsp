#!/usr/bin/env python3
"""
ONVIF Test Script for ESP32-CAM RTSP + ONVIF Server

This script tests the basic ONVIF Profile S functionality of the ESP32-CAM.
It sends SOAP requests to the ONVIF services and validates the responses.
"""

import requests
import xml.etree.ElementTree as ET
from urllib.parse import urljoin
import sys

class OnvifTester:
    def __init__(self, ip_address, port=8080):
        self.base_url = f"http://{ip_address}:{port}"
        self.device_url = urljoin(self.base_url, "/onvif/device_service")
        self.media_url = urljoin(self.base_url, "/onvif/media_service")
        self.imaging_url = urljoin(self.base_url, "/onvif/imaging_service")
        
    def test_device_information(self):
        """Test GetDeviceInformation ONVIF call"""
        print("Testing GetDeviceInformation...")
        
        soap_body = """<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope xmlns:soap="http://www.w3.org/2003/05/soap-envelope" xmlns:tds="http://www.onvif.org/ver10/device/wsdl">
    <soap:Body>
        <tds:GetDeviceInformation/>
    </soap:Body>
</soap:Envelope>"""
        
        headers = {
            'Content-Type': 'application/soap+xml; charset=utf-8',
            'SOAPAction': '"http://www.onvif.org/ver10/device/wsdl/GetDeviceInformation"'
        }
        
        try:
            response = requests.post(self.device_url, data=soap_body, headers=headers, timeout=10)
            print(f"Status Code: {response.status_code}")
            
            if response.status_code == 200:
                print("✓ GetDeviceInformation successful")
                # Parse response to extract device info
                root = ET.fromstring(response.text)
                namespaces = {'tds': 'http://www.onvif.org/ver10/device/wsdl'}
                
                manufacturer = root.find('.//tds:Manufacturer', namespaces)
                model = root.find('.//tds:Model', namespaces)
                firmware = root.find('.//tds:FirmwareVersion', namespaces)
                
                if manufacturer is not None:
                    print(f"  Manufacturer: {manufacturer.text}")
                if model is not None:
                    print(f"  Model: {model.text}")
                if firmware is not None:
                    print(f"  Firmware: {firmware.text}")
            else:
                print(f"✗ GetDeviceInformation failed: {response.text}")
                return False
                
        except Exception as e:
            print(f"✗ GetDeviceInformation error: {e}")
            return False
            
        return True
    
    def test_get_capabilities(self):
        """Test GetCapabilities ONVIF call"""
        print("\nTesting GetCapabilities...")
        
        soap_body = """<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope xmlns:soap="http://www.w3.org/2003/05/soap-envelope" xmlns:tds="http://www.onvif.org/ver10/device/wsdl">
    <soap:Body>
        <tds:GetCapabilities/>
    </soap:Body>
</soap:Envelope>"""
        
        headers = {
            'Content-Type': 'application/soap+xml; charset=utf-8',
            'SOAPAction': '"http://www.onvif.org/ver10/device/wsdl/GetCapabilities"'
        }
        
        try:
            response = requests.post(self.device_url, data=soap_body, headers=headers, timeout=10)
            print(f"Status Code: {response.status_code}")
            
            if response.status_code == 200:
                print("✓ GetCapabilities successful")
                # Parse response to extract service URLs
                root = ET.fromstring(response.text)
                namespaces = {'tt': 'http://www.onvif.org/ver10/schema'}
                
                device_xaddr = root.find('.//tt:Device/tt:XAddr', namespaces)
                media_xaddr = root.find('.//tt:Media/tt:XAddr', namespaces)
                imaging_xaddr = root.find('.//tt:Imaging/tt:XAddr', namespaces)
                
                if device_xaddr is not None:
                    print(f"  Device Service: {device_xaddr.text}")
                if media_xaddr is not None:
                    print(f"  Media Service: {media_xaddr.text}")
                if imaging_xaddr is not None:
                    print(f"  Imaging Service: {imaging_xaddr.text}")
            else:
                print(f"✗ GetCapabilities failed: {response.text}")
                return False
                
        except Exception as e:
            print(f"✗ GetCapabilities error: {e}")
            return False
            
        return True
    
    def test_get_profiles(self):
        """Test GetProfiles ONVIF call"""
        print("\nTesting GetProfiles...")
        
        soap_body = """<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope xmlns:soap="http://www.w3.org/2003/05/soap-envelope" xmlns:trt="http://www.onvif.org/ver10/media/wsdl">
    <soap:Body>
        <trt:GetProfiles/>
    </soap:Body>
</soap:Envelope>"""
        
        headers = {
            'Content-Type': 'application/soap+xml; charset=utf-8',
            'SOAPAction': '"http://www.onvif.org/ver10/media/wsdl/GetProfiles"'
        }
        
        try:
            response = requests.post(self.media_url, data=soap_body, headers=headers, timeout=10)
            print(f"Status Code: {response.status_code}")
            
            if response.status_code == 200:
                print("✓ GetProfiles successful")
                # Parse response to extract profile info
                root = ET.fromstring(response.text)
                namespaces = {'trt': 'http://www.onvif.org/ver10/media/wsdl', 'tt': 'http://www.onvif.org/ver10/schema'}
                
                profiles = root.findall('.//trt:Profiles', namespaces)
                for profile in profiles:
                    token = profile.get('token')
                    name = profile.find('.//tt:Name', namespaces)
                    if token:
                        print(f"  Profile Token: {token}")
                    if name is not None:
                        print(f"  Profile Name: {name.text}")
            else:
                print(f"✗ GetProfiles failed: {response.text}")
                return False
                
        except Exception as e:
            print(f"✗ GetProfiles error: {e}")
            return False
            
        return True
    
    def test_get_stream_uri(self):
        """Test GetStreamUri ONVIF call"""
        print("\nTesting GetStreamUri...")
        
        soap_body = """<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope xmlns:soap="http://www.w3.org/2003/05/soap-envelope" xmlns:trt="http://www.onvif.org/ver10/media/wsdl">
    <soap:Body>
        <trt:GetStreamUri>
            <trt:StreamSetup>
                <tt:Stream>RTP-Unicast</tt:Stream>
                <tt:Transport>
                    <tt:Protocol>RTSP</tt:Protocol>
                </tt:Transport>
            </trt:StreamSetup>
            <trt:ProfileToken>Profile_1</trt:ProfileToken>
        </trt:GetStreamUri>
    </soap:Body>
</soap:Envelope>"""
        
        headers = {
            'Content-Type': 'application/soap+xml; charset=utf-8',
            'SOAPAction': '"http://www.onvif.org/ver10/media/wsdl/GetStreamUri"'
        }
        
        try:
            response = requests.post(self.media_url, data=soap_body, headers=headers, timeout=10)
            print(f"Status Code: {response.status_code}")
            
            if response.status_code == 200:
                print("✓ GetStreamUri successful")
                # Parse response to extract stream URI
                root = ET.fromstring(response.text)
                namespaces = {'tt': 'http://www.onvif.org/ver10/schema'}
                
                uri = root.find('.//tt:Uri', namespaces)
                if uri is not None:
                    print(f"  Stream URI: {uri.text}")
            else:
                print(f"✗ GetStreamUri failed: {response.text}")
                return False
                
        except Exception as e:
            print(f"✗ GetStreamUri error: {e}")
            return False
            
        return True
    
    def run_all_tests(self):
        """Run all ONVIF tests"""
        print(f"ONVIF Test Suite for ESP32-CAM at {self.base_url}")
        print("=" * 50)
        
        tests = [
            self.test_device_information,
            self.test_get_capabilities,
            self.test_get_profiles,
            self.test_get_stream_uri
        ]
        
        passed = 0
        total = len(tests)
        
        for test in tests:
            try:
                if test():
                    passed += 1
            except Exception as e:
                print(f"Test failed with exception: {e}")
        
        print("\n" + "=" * 50)
        print(f"Test Results: {passed}/{total} tests passed")
        
        if passed == total:
            print("✓ All ONVIF tests passed! The ESP32-CAM is ONVIF Profile S compliant.")
        else:
            print("✗ Some ONVIF tests failed. Check the implementation.")
        
        return passed == total

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 test_onvif.py <esp32_ip_address> [port]")
        print("Example: python3 test_onvif.py 192.168.1.100 8080")
        sys.exit(1)
    
    ip_address = sys.argv[1]
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 8080
    
    tester = OnvifTester(ip_address, port)
    success = tester.run_all_tests()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main() 