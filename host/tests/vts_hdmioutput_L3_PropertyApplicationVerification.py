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
import time
import threading

dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(dir_path, "../"))

from raft.framework.plugins.ut_raft.configRead import ConfigRead
from raft.framework.core.logModule import logModule
from raft.framework.plugins.ut_raft import utHelperClass
from raft.framework.plugins.ut_raft.utSuiteNavigator import UTSuiteNavigatorClass
from raft.framework.plugins.ut_raft.utBaseUtils import utBaseUtils
from raft.framework.plugins.ut_raft.utUserResponse import utUserResponse
from rdk_raft.controller import Controller
from vts_hdmioutput_helper import hdmiOutputHelperClass

HDMIOUTPUT_L3_TEST_SUITE_NAME = "VTS_L3_HDMIOUTPUT"

class VTS_HDMIOUTPUT_L3_PropertyApplicationVerification(hdmiOutputHelperClass):
    """
    Python simulation wrapper for the L3 PropertyApplicationVerification test.
    Steps:
        1. For each port in the HFP profile:
             a. Assert hotplug CONNECTED so the DUT port can enter STARTED state.
             b. Start the C++ test menu on the target device.
             c. Select and run the PropertyApplicationVerification test case.
             d. Prompt the user to confirm InfoFrame transmission and property application (manual verification).
             e. Log the step result and stop the menu.
        2. Aggregate results for all ports.
    """
    def __init__(self, log: logModule = None, hfpfile:str=None, testMenuConfigFile=None, moduleName=None, artifactDirPath=None):
        self.testName = "HdmiOutput_L3_PropertyApplicationVerification"
        qcId = "3"
        super().__init__(self.testName, qcId, log, hfpfile, testMenuConfigFile, moduleName, artifactDirPath)
        self.hdmiController = Controller.hdmi_output(
            self.log, self.dut.rawConfig.get("dut").get("hdmiController")
        )

    def connectThread(self, portid, connect_state):
        """
            Step 1: Wait for C++ test initialization.
            Step 2: For the matching port in test config, extract VIC values.
            Step 3: Log VIC values for debug.
            Step 4: If connect_state is True, simulate HDCP handshake:
                    a. Set HDCP status transitions: UNAUTHENTICATED → IN_PROGRESS → AUTHENTICATED.
                    b. For each VIC value, send frame rate changed event.
            Step 5: If connect_state is False, set HDCP status to UNKNOWN.
            Step 6: Wait briefly to allow state propagation.
        """
        time.sleep(2)
        ports = self.test_config[self.moduleName][self.testLevel][self.testcase]['ports']
        for port in ports:
            if port.get('id') != portid:
                continue
            vic_values = []
            for prop in port.get('test_properties', []):
                if prop.get('property') == "VIC":
                    vic_values = prop.get('value', [])
            self.log.info(f"[connectThread] port={port.get('id')} VIC values from test config: {vic_values}")

        if connect_state:
            self.hdmiController.setHDCPStatus(portid, "UNAUTHENTICATED", "UNDEFINED")
            time.sleep(1)
            self.hdmiController.setHDCPStatus(portid, "AUTHENTICATION_IN_PROGRESS", "UNDEFINED")
            self.hdmiController.setHDCPStatus(portid, "AUTHENTICATED", "VERSION_2_X")
            for value in vic_values:
                self.hdmiController.sendFrameRateChanged(portid)
        else:
            self.hdmiController.setHDCPStatus(portid, "UNKNOWN", "UNDEFINED")
        time.sleep(1)

    def testFunction(self):
        """
        Step 1: Set test case name and change directory to target workspace.
        Step 2: Load HFP profile and enumerate port IDs.
        Step 3: Load test config and validate port IDs.
        Step 4: For each port in HFP profile:
            a. If port not in test config, skip.
            b. Assert hotplug CONNECTED.
            c. Start HDCP handshake simulation thread.
            d. Start test menu and run PropertyApplicationVerification.
            e. Log step start.
            f. Collect test results.
            g. Wait for HDCP thread to finish.
            h. Prompt user for InfoFrame/property verification.
            i. Log step result and stop menu.
        Step 5: Aggregate results for all ports and return overall status.
        """
        self.testcase = "PropertyApplicationVerification"
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)
        self.testUserResponse = utUserResponse()

        # Load HFP profile to get port list
        profile_data = self.hdmiOutputReadYaml(self.moduleHFPFile)
        resources = profile_data[self.moduleName].get("resources", [])
        port_ids = [r.get("id") for r in resources]

        # Load test config YAML for port validation
        self.test_config = self.hdmiOutputReadYaml(self.l3TestConfigFile)
        ports_config = self.test_config[self.moduleName][self.testLevel][self.testcase]["ports"]
        valid_port_ids = set(p.get("id") for p in ports_config)

        results = True
        for port_id in port_ids:
            if port_id not in valid_port_ids:
                self.log.warning(f"[testFunction] port={port_id} not found in test config, skipping.")
                continue
            connect_state = True

            # Start HDCP handshake simulation thread
            self.connectThreadObj = threading.Thread(
                    target=self.connectThread, args=(port_id, connect_state), name=f"propertyThread_port{port_id}"
                )
            self.utMenu.start()
            self.log.info(f"[testFunction] port={port_id} - asserting hotplug CONNECTED")
            self.hdmiController.setHotplugState(port_id, connect_state)
            time.sleep(2)
            self.connectThreadObj.start()

            step_label = (
                f"Test Suit: {HDMIOUTPUT_L3_TEST_SUITE_NAME} "
                f"Test Case: {self.testcase} "
                f"Port: {port_id}"
            )
            self.log.stepStart(step_label)

            output = self.utMenu.select(
                HDMIOUTPUT_L3_TEST_SUITE_NAME,
                self.testcase,
                None,
                self.waitTime,
                False,
            )
            group_results = self.utMenu.collect_results(output, True)
            if group_results is None:
                group_results = False

            # Wait for HDCP simulation thread to finish
            self.connectThreadObj.join()

            results = self.testUserResponse.getUserYN(
                f"Are InfoFrames and properties (VIC, CONTENT_TYPE, HDR_OUTPUT_MODE, etc) correctly applied for port {port_id}? (Y/N):"
            )
            self.log.stepResult(group_results, step_label)
            self.utMenu.stop()
            results = results and group_results
        return results

if __name__ == "__main__":
    summaryLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summary"
    summaryLog = logModule(summaryLogName, level=logModule.INFO)
    test = VTS_HDMIOUTPUT_L3_PropertyApplicationVerification(summaryLog)
    test.run(False)
