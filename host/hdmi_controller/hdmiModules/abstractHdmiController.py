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
from abc import ABC, abstractmethod

class HdmiInterface(ABC):
    def __init__(self, logger, address, username, password, port:int=22, prompt:str='~#', control_port:int=8080):
        self._log = logger

    @abstractmethod
    def sendEDIDRead(self, port: int, data: list):
        pass

    @abstractmethod
    def sendFrameRateChanged(self, port: int):
        pass

    @abstractmethod
    def setHDCPStatus(self, port: int, status: str, version: str):
        pass

    @abstractmethod
    def setHotplugState(self, port: int, connected: bool, version: str):
        pass
