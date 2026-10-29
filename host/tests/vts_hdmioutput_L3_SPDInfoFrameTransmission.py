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

from rdk_raft.controller import Controller
from raft.framework.plugins.ut_raft.configRead import ConfigRead
from raft.framework.core.logModule import logModule
from raft.framework.plugins.ut_raft import utHelperClass
from raft.framework.plugins.ut_raft.utSuiteNavigator import UTSuiteNavigatorClass
from raft.framework.plugins.ut_raft.utBaseUtils import utBaseUtils
from raft.framework.plugins.ut_raft.utUserResponse import utUserResponse
from vts_hdmioutput_helper import hdmiOutputHelperClass

HDMIOUTPUT_L3_TEST_SUITE_NAME = "VTS_L3_HDMIOUTPUT"

class VTS_HDMIOUTPUT_L3_SPDInfoFrameTransmission(hdmiOutputHelperClass):
    """
    Python simulation wrapper for the L3 SPDInfoFrameTransmission test.
    Steps:
        1. For each port in the HFP profile:
             a. Assert hotplug CONNECTED so the DUT port can enter STARTED state.
             b. Start the C++ test menu on the target device.
             c. Select and run the SPDInfoFrameTransmission test case.
             d. Prompt the user to confirm InfoFrame transmission (manual verification).
             e. Log the step result and stop the menu.
        2. Aggregate results for all ports.
    A user-response prompt is presented at the end of each SPD source variant
    to allow optional manual verification on an HDMI analyser.
    """

    def __init__(self, log: logModule = None, hfpfile:str=None, testMenuConfigFile=None, moduleName=None, artifactDirPath=None):
        """
        Initializes the test class.

        Args:
            log (logModule, optional): Logger instance. Defaults to None.
        """
        self.testName = "HdmiOutput_L3_SPDInfoFrameTransmission"
        qcId = "4"
        super().__init__(self.testName, qcId, log, hfpfile, testMenuConfigFile, moduleName, artifactDirPath)

        self.hdmiController = Controller.hdmi_output(
            self.log, self.dut.rawConfig.get("dut").get("hdmiController")
        )

    def updateTestConfiguration(self, testName: str, portid: int, spdsource: str):
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

        ports = self.test_config_data[self.moduleName][self.testLevel][testName]["ports"]
        port_entry = next((p for p in ports if p.get("id") == portid), None)
        if port_entry is None:
            raise ValueError(
                f"Port id {portid} not found in '{testName}' config inside {self.l3TestConfigFile}"
            )

        port_entry["spd_info"]["source_information"] = spdsource

        # Persist back to disk
        self.hdmiOutputWriteYaml(self.l3TestConfigFile, self.test_config_data)

        # Copy updated config file to target workspace
        target_config_path = self.targetWorkspace + "/test_configuration/"
        self.utilities.sftpCopy(self.hal_session, self.l3TestConfigFile, target_config_path)

        self.log.info(
            f"[updateTestConfiguration] port={portid}, testName={testName}, "
            f"source_information={spdsource} — config synced to target"
        )

    def connectThread(self, portid, connect_state):
        """
        Simulation thread that drives the vcomponent (HDMI sink side) while the
        C++ SPDInfoFrameTransmission test case is running on the DUT.

        Steps performed for each port configured under SPDInfoFrameTransmission:
          1. Wait briefly so the C++ test has time to initialise.
          2. Assert hotplug CONNECTED — the DUT needs a sink present to start the
             port and begin transmitting InfoFrames.
          3. Supply EDID data — taken from the 'EDIDCallback' section of the same
             YAML config, falling back to the 'HotPlugDetectStateBeforeStartCallback'
             section; an empty list is passed when no EDID data is configured.
        """
        time.sleep(2)
        if connect_state:
            self.hdmiController.setHDCPStatus(portid, "UNAUTHENTICATED", "UNDEFINED")
        time.sleep(1)

    def testFunction(self):
        """
        Main test driver for the L3 SPDInfoFrameTransmission test.

        For every port listed under 'SPDInfoFrameTransmission' in the L3 YAML:
          - Determines the SPD source variants to exercise (from the HFP profile's
            'supportedSPDSources' list, falling back to the single source_information
            value already present in the YAML config).
          - For each source variant:
              1. Updates the L3 YAML config with the current SPD source and syncs
                 the file to the target device.
              2. Prepares a thread that will send an initial HDCP status
                 (UNAUTHENTICATED / UNDEFINED) after a short delay, simulating the
                 early sink HDCP handshake.
              3. Starts the C++ test menu on the target device.
              4. Asserts hotplug CONNECTED so the DUT port can start, then kicks
                 off the HDCP simulation thread in parallel.
              5. Selects and runs the SPDInfoFrameTransmission test case, collecting
                 the automated pass/fail result from the C++ test output.
              6. Waits for the simulation thread to complete.
              7. Prompts the user to optionally confirm SPD transmission on an
                 HDMI analyser (manual verification step).
              8. Stops the menu, logs the step result, and aggregates into the
                 overall pass/fail.

        Returns:
            bool: True if all variants on all ports passed, False otherwise.
        """
        self.testcase = "SPDInfoFrameTransmission"
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)
        self.testUserResponse = utUserResponse()

        # Load YAML configs
        self.test_config_data = self.hdmiOutputReadYaml(self.l3TestConfigFile)
        profile_data = self.hdmiOutputReadYaml(self.moduleHFPFile)

        ports_config = self.test_config_data[self.moduleName][self.testLevel][self.testcase]["ports"]

        results = True

        for port in ports_config:
            port_id = port.get("id")
            spd_info = port.get("spd_info", {})
            connect_state = True

            # Resolve the list of SPD sources to iterate over.
            # The HFP profile may advertise 'supportedSPDSources'; otherwise fall back
            # to the single source_information value already in the YAML.
            resource = next(
                (
                    r
                    for r in profile_data[self.moduleName].get("resources", [])
                    if r.get("id") == port_id
                ),
                {},
            )
            spd_sources = resource.get(
                "supportedSPDSources",
                [spd_info.get("source_information", "DIGITAL_STB")],
            )

            for spdsource in spd_sources:
                self.log.info(
                    f"[testFunction] port={port_id}, SPD source variant='{spdsource}'"
                )

                # 1. Patch YAML config with current SPD source and sync to target
                self.updateTestConfiguration(self.testcase, port_id, spdsource)

                # 2. Prepare the sink-simulation thread
                self.connectThreadObj = threading.Thread(
                    target=self.connectThread, args=(port_id, connect_state), name=f"spdThread_port{port_id}"
                )

                # 3. Start the C++ test menu on the target device
                self.utMenu.start()

                # 4. Assert hotplug CONNECTED so the DUT port can enter STARTED state,
                #    then start the HDCP simulation thread in parallel.
                self.hdmiController.setHotplugState(port_id, connect_state)
                time.sleep(2)
                self.connectThreadObj.start()

                # 5. Select and run the SPDInfoFrameTransmission test case
                step_label = (
                    f"Test Suit: {HDMIOUTPUT_L3_TEST_SUITE_NAME} "
                    f"Test Case: {self.testcase} "
                    f"Port: {port_id} SPDSource: {spdsource}"
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

                # 6. Wait for the simulation thread to finish
                self.connectThreadObj.join()

                results = self.testUserResponse.getUserYN(f"Is SPD frames transmitted on active video transmission? (Y/N):")
                self.log.stepResult(group_results, step_label)

                # 7. Stop the menu and clean up before the next iteration
                self.utMenu.stop()

                results = results and group_results

        return results


if __name__ == "__main__":
    summaryLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summary"
    summaryLog = logModule(summaryLogName, level=logModule.INFO)
    test = VTS_HDMIOUTPUT_L3_SPDInfoFrameTransmission(summaryLog)
    test.run(False)