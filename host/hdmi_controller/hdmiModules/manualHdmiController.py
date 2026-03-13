#!/usr/bin/env python3
#** *****************************************************************************
# * Copyright 2026 RDK Management
# * Licensed under the Apache License, Version 2.0
# * http://www.apache.org/licenses/LICENSE-2.0
#* ******************************************************************************
from abstractHdmiController import HdmiInterface

class manualHdmiController(HdmiInterface):
    def __init__(self, logger, address, username, password, port=22, prompt='~#', control_port=8080):
        super().__init__(logger)
        self.address = address
        self.username = username
        self.password = password
        self.port = port
        self.prompt = prompt
        self.control_port = control_port

    def sendEDIDRead(self, port: int, data: list):
        print(f"Please trigger EDID read for HDMI output port {port} with data {data} and press Enter when done.")
        input(f"Press Enter to confirm EDID read event for port {port}...")
        return True

    def sendFrameRateChanged(self, port: int):
        print(f"Please trigger frame rate changed event for HDMI output port {port} and press Enter when done.")
        input(f"Press Enter to confirm frame rate changed event for port {port}...")
        return True

    def setHDCPStatus(self, port: int, status: str, version: str):
        print(f"Please set HDCP status '{status}' (version: {version}) for HDMI output port {port} and press Enter when done.")
        input(f"Press Enter to confirm HDCP status '{status}' for port {port}...")
        return True

    def setHotplugState(self, port: int, connected: bool):
        state = 'connected' if connected else 'disconnected'
        print(f"Please set hotplug state to '{state}' for HDMI output port {port} and press Enter when done.")
        input(f"Press Enter to confirm hotplug state '{state}' for port {port}...")
        return True
