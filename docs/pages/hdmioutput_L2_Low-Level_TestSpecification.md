# HDMI OUTPUT L2 Low Level Test Specification

## Table of Contents
- [Overview](#overview)
- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [References](#references)
- [Level 2 Test Specification](#level-2-test-specification)

## Overview
This document describes the level 2 low level test specification for the `HDMI OUTPUT` module.

### Acronyms, Terms and Abbreviations
- `HAL `   - Hardware Abstraction Layer
- `VTS `   - Vendor Test Suite
- `HDMI`   - High Definition Multimedia Interface
- `L1  `   - Level 1 Testing
- `L2  `   - Level 2 Testing
- `L3  `   - Level 3 Testing
- `DUT `   - Device Under Test
- `RAFT`   - Rapid Automation Framework for Testing
- `YAML`   - YAML Ain't Markup Language
- `AIDL`   - Android Interface Definition Language

### References
- High Level Test Specification - [hdmioutput_High-Level_TestSpecification.md](hdmioutput_High-Level_TestSpecification.md)

## Level 2 Test Specification
The following functions are intended to test the `HDMI OUTPUT` `HAL` module according to the `L2` Test specification.

### Test 1
| Title | Details |
| -- | -- |
| Function Name | StateTransitions |
| Description | This test validates the state machine behaviour of the HDMI OUTPUT interface. It ensures that the interface transitions correctly between CLOSED, READY, STARTED, and other states, and that state change notifications are received by the event listener |
| Test Group | 02 |
| Test Case ID | 001 |
| Priority | High |

**Pre-Conditions :**
- No external HDMI devices connected to the DUT
- HDMI OUTPUT interface must be available in State::CLOSED initially

**Dependencies :**
- None

**User Interaction :**
None

**Test Procedure :**
| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Register Event Listeners | None | Register the event listener |
| 2 | Get the State before Open | None | State should be CLOSED |
| 3 | Open the HDMI OUTPUT with event listener | event listener handle | Function should return valid controller handle |
| 4 | Get the State | None | State should be OPENING |
| 5 | Check the event listener callback | | Previous State should be CLOSED and current state should be OPENING |
| 6 | Get the State | None | State should be READY |
| 7 | Check the event listener callback | | Previous State should be OPENING and current state should be READY |
| 8 | Start the HDMI OUTPUT | | Function should return True |
| 9 | Get the State | None | State should be STARTING |
| 10 | Check the event listener callback | | Previous State should be READY and current state should be STARTING |
| 11 | Get the State | None | State should be STARTED |
| 12 | Check the event listener callback | | Previous State should be STARTING and current state should be STARTED |
| 13 | Stop the HDMI OUTPUT | | Function should return True |
| 14 | Get the State | None | State should be STOPPING |
| 15 | Check the event listener callback | | Previous State should be STARTED and current state should be STOPPING |
| 16 | Get the State | None | State should be READY |
| 17 | Check the event listener callback | | Previous State should be STOPPING and current state should be READY |
| 18 | Close the HDMI OUTPUT | | Function should return True |
| 19 | Get the State | None | State should be CLOSING |
| 20 | Check the event listener callback | | Previous State should be READY and current state should be CLOSING |
| 21 | Get the State | None | State should be CLOSED |
| 22 | Check the event listener callback | | Previous State should be CLOSING and current state should be CLOSED |

### Test 2
| Title | Details |
| -- | -- |
| Function Name | HPDStateTransition |
| Description | This test verifies the Hot Plug Detect (HPD) state when no HDMI device is connected. It expects HPD to be FALSE and no EDID callback. |
| Test Group | 02 |
| Test Case ID | 002 |
| Priority | High |

**Pre-Conditions :**
- No external HDMI device should be connected for this test.
- HDMI OUTPUT interface must be available in State::CLOSED initially

**Dependencies :**
- None

**User Interaction :**
None

**Test Procedure :**
| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Open the HDMI OUTPUT | None | Function should return valid controller handle |
| 2 | Wait for callbacks | None | Wait for HPD callback |
| 3 | Get HPD state | None | HPD state should be FALSE |
| 4 | Verify HPD callback | None | Callback received with HPD state FALSE |
| 5 | Verify no EDID callback | None | No EDID callback received |
| 6 | Close the HDMI OUTPUT | None | Function should return True |

### Test 3
| Title | Details |
| -- | -- |
| Function Name | SetGetIndividualProperty |
| Description | This test validates setting and getting individual HDMI output properties. It loops through all properties and tests all supported values from profile. |
| Test Group | 02 |
| Test Case ID | 003 |
| Priority | High |

**Pre-Conditions :**
- No external HDMI device required. Properties can be set/get without device connected.
- HDMI OUTPUT interface must be available in State::CLOSED initially

**Dependencies :**
- None

**User Interaction :**
None

**Test Procedure :**
| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Open the HDMI OUTPUT | None | Function should return valid controller handle |
| 2 | Start the HDMI OUTPUT | None | Function should return True |
| 3 | For each property | Supported values from profile | Set property, then get property | Retrieved value matches set value |
| 4 | Stop the HDMI OUTPUT | None | Function should return True |
| 5 | Close the HDMI OUTPUT | None | Function should return True |

### Test 4
| Title | Details |
| -- | -- |
| Function Name | SetGetMultiProperties |
| Description | This test validates setting multiple properties simultaneously using setPropertyMulti. |
| Test Group | 02 |
| Test Case ID | 004 |
| Priority | High |

**Pre-Conditions :**
- No external HDMI device required. Properties can be set/get without device connected.
- HDMI OUTPUT interface must be available in State::CLOSED initially

**Dependencies :**
- None

**User Interaction :**
None

**Test Procedure :**
| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Open the HDMI OUTPUT | None | Function should return valid controller handle |
| 2 | Start the HDMI OUTPUT | None | Function should return True |
| 3 | For each value index | Supported values from profile | Set multiple properties at once | All retrieved values match set values |
| 4 | Stop the HDMI OUTPUT | None | Function should return True |
| 5 | Close the HDMI OUTPUT | None | Function should return True |

### Test 5
| Title | Details |
| -- | -- |
| Function Name | SPDInfoFrame |
| Description | This test validates setting and getting Source Product Description InfoFrame. |
| Test Group | 02 |
| Test Case ID | 005 |
| Priority | High |

**Pre-Conditions :**
- No external HDMI device required. SPD InfoFrame can be set/get without device connected.
- HDMI OUTPUT interface must be available in State::CLOSED initially

**Dependencies :**
- None

**User Interaction :**
None

**Test Procedure :**
| Variation / Steps | Description | Test Data | Expected Result | Notes |
|-------------------|-------------|-----------|----------------|-------|
| 1 | Open the HDMI OUTPUT | None | Function should return valid controller handle |
| 2 | For each supported SPD source | Supported values from profile | Set SPD InfoFrame, then get SPD InfoFrame | Retrieved InfoFrame matches set InfoFrame |
| 3 | Close the HDMI OUTPUT | None | Function should return True |
