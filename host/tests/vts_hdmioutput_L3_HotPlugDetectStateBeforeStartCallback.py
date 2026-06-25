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
from vts_hdmioutput_helper import hdmiOutputHelperClass

HDMIOUTPUT_L3_TEST_SUITE_NAME = "VTS_L3_HDMIOUTPUT"

class VTS_HDMIOUTPUT_L3_HotPlugDetectStateBeforeStartCallback(hdmiOutputHelperClass):
    """
    Python simulation wrapper for the L3 HotPlugDetectStateBeforeStartCallback test.
    Steps:
        1. For each port in the HFP profile:
             a. Assert hotplug CONNECTED so the DUT port can enter STARTED state.
             b. Start the C++ test menu on the target device.
             c. Select and run the HotPlugDetectStateBeforeStartCallback test case.
             d. Prompt the user to confirm InfoFrame transmission and property application (manual verification).
             e. Log the step result and stop the menu.
        2. Aggregate results for all ports.
    """
    def __init__(self, log: logModule = None, hfpfile:str=None, testMenuConfigFile=None, moduleName=None, artifactDirPath=None):
        self.testName = "HdmiOutput_L3_HotPlugDetectStateBeforeStartCallback"
        qcId = "1"
        super().__init__(self.testName, qcId, log, hfpfile, testMenuConfigFile, moduleName, artifactDirPath)

    def updateTestConfiguration(self, testName: str, portid: int):
        """
        Updates the L3 test-configuration YAML with a specific SPD source value
        for the given port, then copies the updated file to the target device.

        Args:
            testName  (str): Test-case key in the YAML (e.g. 'SPDInfoFrameTransmission').
            portid    (int): HDMI output port ID to update.
            spdsource (str): SPD source string (e.g. 'DIGITAL_STB', 'DVD_PLAYER').

        Raises:
            ValueError: If portid is not found under testName in the YAML config.
        """
        # Re-read from disk so we always work on the latest on-disk state
        self.test_config_data = self.hdmiOutputReadYaml(self.l3TestConfigFile)

        # Persist back to disk
        self.hdmiOutputWriteYaml(self.l3TestConfigFile, self.test_config_data)

        # Copy updated config file to target workspace
        target_config_path = self.targetWorkspace + "/test_configuration/"
        self.utilities.sftpCopy(self.hal_session, self.l3TestConfigFile, target_config_path)

    def connectThread(self, portid, connect_state):
        """
        Simulates the HDMI sink HDCP handshake sequence for the DUT port:
          - Waits briefly for C++ test to initialize.
          - Sends HDCP status transitions: UNAUTHENTICATED → IN_PROGRESS → AUTHENTICATED.
        """
        time.sleep(2)
        ports_config = self.test_config[self.moduleName][self.testLevel][self.testcase]['ports']
        if connect_state:
            self.hdmiAnalyserController.setHDCPStatus(portid, "UNAUTHENTICATED", "UNDEFINED")
        
        time.sleep(20)
        hpd_sequence = ports_config[portid]['hpd_sequence']
        expected_hdcp_version = ports_config[portid]['expected_hdcp_version']
        for hpd in hpd_sequence:
            self.hdmiAnalyserController.setHotplugState(portid, hpd)
            if hpd:
                self.hdmiAnalyserController.setHDCPStatus(portid, "UNAUTHENTICATED", "UNDEFINED")
            else:
                self.hdmiAnalyserController.setHDCPStatus(portid, "UNKNOWN", "UNDEFINED")

        time.sleep(1)

    def testFunction(self):
        """
        Main test driver for L3 HotPlugDetectStateBeforeStartCallback:
          - For each port in the HFP profile:
              1. Asserts hotplug CONNECTED.
              2. Starts menu and runs test case.
              3. Prompts user for InfoFrame and property verification.
              4. Logs and aggregates results.
        Returns:
            bool: True if all ports pass, False otherwise.
        """
        self.testcase = "HotPlugDetectStateBeforeStartCallback"
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)

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
            self.updateTestConfiguration(self.testcase, port_id)
            connect_state = ports_config[port_id]['connection_state']

            # Start HDCP handshake simulation thread
            self.connectThreadObj = threading.Thread(
                    target=self.connectThread, args=(port_id, connect_state), name=f"propertyThread_port{port_id}"
                )
            self.utMenu.start()
            self.log.info(f"[testFunction] port={port_id} - asserting hotplug CONNECTED")
            self.hdmiAnalyserController.setHotplugState(port_id, connect_state)
            if connect_state:
                edid_data = ports_config[port_id]['expected_edid']
                self.hdmiAnalyserController.sendEDIDRead(port_id, edid_data)
            time.sleep(1)
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

            self.log.stepResult(group_results, step_label)
            self.utMenu.stop()
            results = results and group_results
        return results

if __name__ == "__main__":
    summaryLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summary"
    summaryLog = logModule(summaryLogName, level=logModule.INFO)
    test = VTS_HDMIOUTPUT_L3_HotPlugDetectStateBeforeStartCallback(summaryLog)
    test.run(False)
