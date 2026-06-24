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
# http://www.apache.org/licenses/LICENSE-2.0
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
import yaml
dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(dir_path)
sys.path.append(os.path.join(dir_path, "../../tests/raft/"))

command_templates_dir = os.path.join(dir_path, 'commands')
EDID_READ_CMD_TEMPLATE = os.path.join(command_templates_dir, 'hdmioutput_edid_read.yaml')
FRAME_RATE_CHANGED_CMD_TEMPLATE = os.path.join(command_templates_dir, 'hdmioutput_frame_rate_changed.yaml')
HDCP_STATUS_CMD_TEMPLATE = os.path.join(command_templates_dir, 'hdmioutput_hdcp_status.yaml')
HOTPLUG_STATE_CMD_TEMPLATE = os.path.join(command_templates_dir, 'hdmioutput_hotplug_state.yaml')

from abstractHdmiController import HdmiInterface
from framework.core.commandModules.sshConsole import sshConsole
from framework.core.utPlaneController import utPlaneController

class virtualHdmiController(HdmiInterface):
    def __init__(self, logger, address, username, password, port:int=22, prompt:str='~#', control_port:int=8080):
        super().__init__(logger, address, username, password, port, prompt, control_port)
        self.address = address
        self.username = username
        self.password = password
        self.port = port
        self.prompt = prompt
        self.control_port = control_port
        self.session = sshConsole(self._log, self.address, self.username, self.password, port=self.port, prompt=self.prompt)
        self.utPlaneController = utPlaneController(self.session, port=self.control_port)

    def sendEDIDRead(self, port: int, data: list):
        with open(EDID_READ_CMD_TEMPLATE, 'r') as f:
            msg = yaml.safe_load(f)
        msg['hdmioutput']['params']['port'] = port
        msg['hdmioutput']['params']['data'] = data
        yaml_str = yaml.dump(msg)
        return self.utPlaneController.sendMessage(yaml_str)


    def sendFrameRateChanged(self, port: int):
        with open(FRAME_RATE_CHANGED_CMD_TEMPLATE, 'r') as f:
            msg = yaml.safe_load(f)
        msg['hdmioutput']['params']['port'] = port
        yaml_str = yaml.dump(msg)
        return self.utPlaneController.sendMessage(yaml_str)


    def setHDCPStatus(self, port: int, status: str, version: str):
        with open(HDCP_STATUS_CMD_TEMPLATE, 'r') as f:
            msg = yaml.safe_load(f)
        msg['hdmioutput']['params']['port'] = port
        msg['hdmioutput']['params']['status'] = status
        msg['hdmioutput']['params']['version'] = version
        yaml_str = yaml.dump(msg)
        return self.utPlaneController.sendMessage(yaml_str)
        

    def setHotplugState(self, port: int, connected: bool, version: str):
        with open(HOTPLUG_STATE_CMD_TEMPLATE, 'r') as f:
            msg = yaml.safe_load(f)
        msg['hdmioutput']['params']['port'] = port
        msg['hdmioutput']['params']['connected'] = connected
        yaml_str = yaml.dump(msg)
        return self.utPlaneController.sendMessage(yaml_str)

