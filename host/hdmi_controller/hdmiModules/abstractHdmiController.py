#!/usr/bin/env python3
#** *****************************************************************************
# * Copyright 2026 RDK Management
# * Licensed under the Apache License, Version 2.0
# * http://www.apache.org/licenses/LICENSE-2.0
#* ******************************************************************************
from abc import ABC, abstractmethod

class HdmiInterface(ABC):
    def __init__(self, logger):
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
    def setHotplugState(self, port: int, connected: bool):
        pass
