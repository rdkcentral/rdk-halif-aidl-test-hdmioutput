#!/usr/bin/env python3
#** *****************************************************************************
# *
# * If not stated otherwise in this file or this component's LICENSE file the
# * following copyright and licenses apply:
# *
# * Copyright 2026 RDK Management
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# * http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *
#* ******************************************************************************
import os
import sys

dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(dir_path)

from hdmiModules.virtualHdmiController import virtualHdmiController
from hdmiModules.manualHdmiController import manualHdmiController

class HdmiController:
    """
    High-level HDMI output controller, similar to HDMI input controller.
    Instantiates the correct controller type based on config.
    """
    def __init__(self, log, config: dict):
        self._log = log
        self.controllerType = config.get('type', 'virtual-hdmi-controller')

        if self.controllerType == 'virtual-hdmi-controller':
            self.controller = virtualHdmiController(self._log,
                                                    address=config.get('address'),
                                                    username=config.get('username',''),
                                                    password=config.get('password',''),
                                                    port=config.get('port',22),
                                                    prompt=config.get('prompt', '~#'),
                                                    control_port=config.get('control_port', 8080))
        elif self.controllerType == 'manual-hdmi-controller':
            self.controller = manualHdmiController(self._log,
                                                    address=config.get('address'),
                                                    username=config.get('username',''),
                                                    password=config.get('password',''),
                                                    port=config.get('port',22),
                                                    prompt=config.get('prompt', '~#'),
                                                    control_port=config.get('control_port', 8080))
        else:
            raise ValueError(f"Unsupported HDMI controller type: {self.controllerType}")

    def sendEDIDRead(self, port: int, data: list):
        """
        Send an EDID read event for the HDMI output port.

        Args:
            port (int): HDMI output port number.
            data (list): EDID data bytes.

        Returns:
            bool: True if message/event handled successfully.
        """
        return self.controller.sendEDIDRead(port, data)

    def sendFrameRateChanged(self, port: int):
        """
        Send a frame rate changed event for the HDMI output port.

        Args:
            port (int): HDMI output port number.

        Returns:
            bool: True if message/event handled successfully.
        """
        return self.controller.sendFrameRateChanged(port)

    def setHDCPStatus(self, port: int, status: str, version: str):
        """
        Set HDCP status for the HDMI output port.

        Args:
            port (int): HDMI output port number.
            status (str): HDCP status string.
            version (str): HDCP version string.

        Returns:
            bool: True if message/event handled successfully.
        """
        return self.controller.setHDCPStatus(port, status, version)

    def setHotplugState(self, port: int, connected: bool):
        """
        Set hotplug state for the HDMI output port.

        Args:
            port (int): HDMI output port number.
            connected (bool): True if connected, False if disconnected.

        Returns:
            bool: True if message/event handled successfully.
        """
        return self.controller.setHotplugState(port, connected)


# Backward compatibility alias
HdmiOutputController = HdmiController
