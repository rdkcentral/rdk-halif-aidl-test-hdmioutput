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

from hdmi_controller.hdmiController import HdmiController
from raft.framework.plugins.ut_raft.configRead import ConfigRead
from raft.framework.core.logModule import logModule
from raft.framework.plugins.ut_raft import utHelperClass
from raft.framework.plugins.ut_raft.utSuiteNavigator import UTSuiteNavigatorClass
from raft.framework.plugins.ut_raft.utBaseUtils import utBaseUtils
from raft.framework.plugins.ut_raft.utUserResponse import utUserResponse
from vts_hdmioutput_helper import hdmiOutputHelperClass

HDMIOUTPUT_L3_TEST_SUITE_NAME = "VTS_L3_HDMIOUTPUT"

class VTS_HDMIOUTPUT_L3_VideoPlaybackEventVerification(hdmiOutputHelperClass):
    """
    Python simulation wrapper for the L3 VideoPlaybackEventVerification test.
    Steps:
        1. For each port in the HFP profile:
             a. Start HDCP handshake simulation thread.
             b. Assert hotplug CONNECTED.
             c. Start menu and run test case.
             d. Wait for thread to finish.
             e. Prompt user for InfoFrame event verification.
             f. Log step result and stop menu.
        2. Aggregate results for all ports.
    """
    def __init__(self, log: logModule = None, hfpfile:str=None, testMenuConfigFile=None, moduleName=None, artifactDirPath=None):
        self.testName = "HdmiOutput_L3_VideoPlaybackEventVerification"
        qcId = "5"
        super().__init__(self.testName, qcId, log, hfpfile, testMenuConfigFile, moduleName, artifactDirPath)
        self.hdmiController = HdmiController(
            self.log, self.dut.rawConfig.get("dut").get("hdmiController")
        )

    def connectThread(self, portid, connect_state):
        """
        Simulates the HDMI sink HDCP handshake sequence for the DUT port:
          - Waits briefly for C++ test to initialize.
          - Sends HDCP status transitions: UNAUTHENTICATED → IN_PROGRESS → AUTHENTICATED.
        """
        time.sleep(5)
        if connect_state:
            self.hdmiController.setHDCPStatus(portid, "UNAUTHENTICATED", "UNDEFINED")
            self.hdmiController.setHDCPStatus(portid, "AUTHENTICATION_IN_PROGRESS", "UNDEFINED")
            self.hdmiController.setHDCPStatus(portid, "AUTHENTICATED", "VERSION_2_X")
        else:
            self.hdmiController.setHDCPStatus(portid, "UNKNOWN", "UNDEFINED")
        time.sleep(1)

    def testFunction(self):
        """
        Main test driver for L3 VideoPlaybackEventVerification:
          - For each port in the HFP profile:
              1. Starts HDCP handshake simulation thread.
              2. Asserts hotplug CONNECTED.
              3. Starts menu and runs test case.
              4. Waits for thread to finish.
              5. Prompts user for InfoFrame event verification.
              6. Logs and aggregates results.
        Note: Only execute for actual device/HAL
        Returns:
            bool: True if all ports pass, False otherwise.
        """
        self.testcase = "VideoPlaybackEventVerification"
        
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)
        self.testUserResponse = utUserResponse()

        # Load HFP profile to get port list
        profile_data = self.hdmiOutputReadYaml(self.moduleHFPFile)
        resources = profile_data[self.moduleName].get("resources", [])
        port_ids = [r.get("id") for r in resources]

        results = True
        # Load test config YAML for port validation
        test_config = self.hdmiOutputReadYaml(self.l3TestConfigFile)
        ports_config = test_config[self.moduleName][self.testLevel][self.testcase]["ports"]
        valid_port_ids = set(p.get("id") for p in ports_config)

        for port_id in port_ids:
            if port_id not in valid_port_ids:
                self.log.warning(f"[testFunction] port={port_id} not found in test config, skipping.")
                continue
            connect_state = True

            # Start HDCP handshake simulation thread
            self.connectThreadObj = threading.Thread(
                    target=self.connectThread, args=(port_id, connect_state), name=f"playbackThread_port{port_id}"
                )
            self.utMenu.start()
            self.log.info(f"[testFunction] port={port_id} - asserting hotplug CONNECTED")
            self.hdmiController.setHotplugState(port_id, connect_state)
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

            # Prompt user for InfoFrame event verification
            results = self.testUserResponse.getUserYN(
                f"Are InfoFrame events(AudioInfoFrame, DRMInfoFrame etc) generated during video playback for port {port_id}? (Y/N):"
            )
            self.log.stepResult(group_results, step_label)
            self.utMenu.stop()
            results = results and group_results
        return results


if __name__ == "__main__":
    summaryLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summary"
    summaryLog = logModule(summaryLogName, level=logModule.INFO)
    test = VTS_HDMIOUTPUT_L3_VideoPlaybackEventVerification(summaryLog)
    test.run(False)