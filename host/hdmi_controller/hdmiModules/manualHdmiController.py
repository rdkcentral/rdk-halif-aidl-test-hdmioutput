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
        return True

    def sendFrameRateChanged(self, port: int):
        return True

    def setHDCPStatus(self, port: int, status: str, version: str):
        return True

    def setHotplugState(self, port: int, connected: bool, version: str):
        if connected == False:
                result = self.testUserResponse.getUserYN(f"UnPlug the HDMI device of HDCP version {version} and press Y:")
        else :
                result = self.testUserResponse.getUserYN(f"Plug the HDMI device of HDCP version {version} and press Y:")
        return result
