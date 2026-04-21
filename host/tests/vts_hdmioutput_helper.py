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
import tarfile
import yaml
import time

dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(dir_path, "../"))

# Placeholder for raft imports
# from raft.framework.plugins.ut_raft.configRead import ConfigRead
# from raft.framework.plugins.ut_raft import utHelperClass
# from raft.framework.core.logModule import logModule
# from raft.framework.plugins.ut_raft.utSuiteNavigator import UTSuiteNavigatorClass
# from raft.framework.plugins.ut_raft.utBaseUtils import utBaseUtils

HDMIOUTPUT_TEST_MENU_CONFIGURATION_FILE = os.path.join(dir_path, "vts_hdmioutput_testMenuConfiguration.yaml")
HDMIOUTPUT_TESTCASES_CONFIGURATION_FILE = os.path.join(os.path.join(dir_path, "../configs"), "vts_hdmioutput_testcase_configuration.yaml")
HDMIOUTPUT_L3_TEST_CONFIG_FILE = os.path.join(os.path.join(dir_path, "../../test_configuration"), "vts_l3_HdmiOutput.yaml")

from raft.framework.plugins.ut_raft.configRead import ConfigRead
from raft.framework.plugins.ut_raft import utHelperClass
from raft.framework.core.logModule import logModule
from raft.framework.plugins.ut_raft.utSuiteNavigator import UTSuiteNavigatorClass
from raft.framework.plugins.ut_raft.utBaseUtils import utBaseUtils

class hdmiOutputHelperClass(utHelperClass):
    def __init__(self, testName:str, qcId:str, log:logModule=None ):
        self.moduleName = "hdmioutput"
        self.testLevel = "L3"
        super().__init__(testName, qcId, log)

        self.log = log
        if log is None:
            self.log = logModule(self.__class__.__name__)
            self.log.setLevel(self.log.INFO)

        # Parse device configuration
        self.deviceTestSetup = self.cpe.get("test")
        self.targetWorkspace = os.path.join(self.cpe.get("target_directory"), self.moduleName)
        self.moduleHFPFile = os.path.join(dir_path, self.deviceTestSetup.get("profile"))
        profileOnTarget = os.path.join(self.targetWorkspace, "profiles")
        profileOnTarget = os.path.join(profileOnTarget, os.path.basename(self.moduleHFPFile))
        profileOnTarget = profileOnTarget.replace("\\", "/")
        self.waitTime = 500

        # Load test menu and test case configuration files
        self.l3TestConfigFile = HDMIOUTPUT_L3_TEST_CONFIG_FILE
        self.testMenuConfiguration = ConfigRead(HDMIOUTPUT_TEST_MENU_CONFIGURATION_FILE, self.moduleName)
        self.testCaseConfiguration = ConfigRead(HDMIOUTPUT_TESTCASES_CONFIGURATION_FILE, self.moduleName)
        self.testMenuConfiguration.test.execute = self.testMenuConfiguration.test.execute + f" -p {profileOnTarget}"

        # Establish HAL session
        self.hal_session = self.dut.getConsoleSession("ssh_hal_test")

        # Initialize UT menu navigator and utilities
        self.utMenu = UTSuiteNavigatorClass(self.testMenuConfiguration, None, self.hal_session)
        self.utilities = utBaseUtils()

        # Copy required artifacts to target device
        self.utilities.change_directory(self.hal_session, self.targetWorkspace)
        for artifact in self.testMenuConfiguration.test.artifacts:
            filesPath = os.path.join(dir_path, artifact)
            self.utilities.sftpCopy(self.hal_session, filesPath, self.targetWorkspace)

            if tarfile.is_tarfile(filesPath):
                target_tarfile = os.path.join(self.targetWorkspace, os.path.basename(artifact))
                target_tarfile = target_tarfile.replace('\\', '/')
                self.utilities.untar(self.hal_session, target_tarfile, self.targetWorkspace)

    def hdmiOutputReadYaml(self, yaml_path: str) -> dict:
        """
        Reads a YAML file and returns its content as a dictionary.
        Args:
            yaml_path (str): Path to the YAML file
        Returns:
            dict: Content of the YAML file as a dictionary
        """
        with open(yaml_path, "r") as f:
            return yaml.safe_load(f)

    def hdmiOutputWriteYaml(self, yaml_path: str, data: dict):
        """
        Writes a dictionary to a YAML file.
        Args:
            yaml_path (str): Path to the YAML file
            data (dict): Content to write to the YAML file
        """
        with open(yaml_path, "w") as f:
            yaml.dump(data, f, sort_keys=False, default_flow_style=None)

    def testEndFunction(self, powerOff=True):
        """
        Ends the test case.
        Args:
            powerOff (bool): Whether to power off the device
        """
        super().testEndFunction(powerOff)
