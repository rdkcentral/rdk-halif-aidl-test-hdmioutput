# HDMI Output High Level Test Specification Document

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Introduction](#introduction)
- [L2 Test Specification](#l2-test-specification)
- [L3 Test Specification](#l3-test-specification)

## Acronyms, Terms and Abbreviations

- `HAL`    - Hardware Abstraction Layer
- `API`    - Application Programming Interface
- `AIDL`   - Android Interface Definition Language
- `L2`     - Level 2 Testing
- `L3`     - Level 3 Testing
- `NA`     - Not Applicable
- `HPD`    - Hot Plug Detect
- `HDCP`   - High-Bandwidth Digital Content Protection
- `EDID`   - Extended Display Identification Data
- `SPD`    - Source Product Description InfoFrame

## Introduction

HDMI Output HAL provides control and monitoring of HDMI output ports on RDK platforms.
It abstracts HDMI output behavior including Hot Plug Detect handling, EDID delivery, state transitions, HDCP authentication, InfoFrame configuration, and dynamic runtime HDMI output configuration.

The HDMI Output HAL supports multiple output ports, each with:
* Independent capabilities
* A defined state machine
* Its own Controller interface
* Event callback interfaces for state changes, EDID reporting, HDCP status, frame-rate updates, and hot-plug events

- Hdmi Output Interface Definitions - [Hdmi output HAL](https://github.com/rdkcentral/rdk-halif-aidl/tree/0.13.1/hdmioutput/current/com/rdk/hal/hdmioutput)

- Hdmi Output HAL Feature Profile - [hfp-hdmioutput.yaml](https://github.com/rdkcentral/rdk-halif-aidl/tree/main/hdmioutput/current/hfp-hdmioutput.yaml)


## L2 Test Specification

| Test | Description | HAL APIs |
|------|-------------|----------|
| Get the State machine transitions for HDMI Output port | <ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li> Register event listeners using registerEventListener </li><li>Ensure state transitions (open -> start -> stop -> close) work as expected using getState. <ul><li>During open(): CLOSED->OPENING->READY</li><li>During start(): READY->STARTING->STARTED</li><li>During stop(): STARTED->STOPING->READY</li><li>During close(): READY->CLOSING->CLOSED</li></ul></li> <li> Validate event listener notifications using onStateChanged </li></ul>| <ul><li>IHDMIOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(), getState(), registerEventListener(), unregisterEventListener(), close()</li><li>IHDMIOutputController - start(), stop()</li></ul> |
|Verify HotPlugDetect state of each HDMI output |<ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Verify the current HPD state for a device attached to the HDMI output port</li><li> HotPlugState should be FALSE in getHotPlugDetectState and callback</li><li>Verify onEDID callback is not received</li></ul> |<ul><li>IHDMOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(),close()</li><li>IHDMIOutputController - getHotPlugDetectState()</li><li>IHDMIOutputControllerLisener - onHotPlugDetectStateChanged(), onEDID()</li></ul>|
| Validate HDMI output property using set/get | <ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Verifies individual HDMI output property set and get operation after opening or starting the HDMI output port instance</li><li>Property:VIC, CONTENT_TYPE, AFD, HDR_OUTPUT_MODE, SCAN_INFORMATION</li></ul> | <ul><li>IHDMOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(), getProperty(), close()</li><li>IHDMIOutputController - start(), setProperty(), stop()</li></ul> |
| Validate HDMI output multi-property set and get | <ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Verifies setting of multiple property-value pairs set and get operation after opening or starting the HDMI output port instance</li><li>Property:VIC, CONTENT_TYPE, AFD, HDR_OUTPUT_MODE, SCAN_INFORMATION</li></ul> | <ul><li>IHDMOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(), close()</li><li>IHDMIOutputController - start()  setPropertyMulti(), stop()</li></ul> |
| Verify SPD InfoFrame set/get| <ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Verify correct SPDInfoFrame set and get operation after opening the HDMI output port instance</li></ul> | <ul><li>IHDMOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(), close()</li><li>IHDMIOutputController - setSPDInfoFrame(), getSPDInfoFrame()</li></ul> |


## L3 Test Specification

| Test | Description | HAL APIs used |
|-----|------------|-------------|
|Verify HotPlugDetect state callback for each HDMI output |<ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Verify the current HPD state for a device attached to the HDMI output port</li><li>Verify callback is triggered once during state OPENING </li><li> Verify the HotPlugState in callback is FALSE when state is OPENING and HDMI Output device is not connected </li><li> Verify the HotPlugState in callback is TRUE when state is OPENING and HDMI Output device is already connected </li><li>Verify Callback is not fired on HPD events untill state is STARTED </li><li>Verify the HotPlugState is TRUE in callback on HPD plug event when the HDMI output port is STARTED. </li><li>Verify the HotPlugState is FALSE in callback on HPD unplug event when the HDMI output port is STARTED. </li><li>Verify the HotPlugState is TRUE in callback on powerON of sink device when the HDMI output port is STARTED. </li><li>Verify the HotPlugState is FALSE in callback on powerOFF if sink device when the HDMI output port is STARTED. </li><li> Verify onEDID() callback is received after onHotPlugDetectStateChanged is TRUE.</li></ul> |<ul><li>IHDMOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(),close()</li><li>IHDMIOutputController - getHotPlugDetectState()</li><li>IHDMIOutputControllerListener - start(),onHotPlugDetectStateChanged(), onEDID(), stop()</li></ul>|
|Verify HDCP Status change callback for each HDMI Output|<ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Verify the HDCP status change event for each port after open or started.</li><li>Verify callback for UNAUTHENTICATED when the active HDMI output device is connected after READY</li><li>Verify callbacks for AUTHENTICATION_IN_PROGRESS and AUTHENTICATED when HDMI output device is connected after STARTED</li><li>Verify callbacks for AUTHENTICATION_IN_PROGRESS and AUTHENTICATED when HDMI output device is connected before STARTED</li><li>Verify the HDCP status and the negotiated HDCP version.</li></ul> |<ul><li>IHDMIOutputManager - getHDMIOutputIds, getHDMIOutput</li><li> IHDMIOutput - open(), close()</li><li> IHDMIOutputController - start(), getHDCPStatus(), getHDCPCurrentVersion(), stop()</li><li> IHDMIOutputControllerListener - onHDCPStatusChanged()</li></ul>|
|Verify different HDCP protocol versions|<ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Loop through different external devices with various HDCP protocol versions</li><li>Verify callbacks for AUTHENTICATION_IN_PROGRESS and AUTHENTICATED when HDMI output device is connected after STARTED</li><li>Verify the HDCP status and the negotiated HDCP version.</li></ul> |<ul><li>IHDMIOutputManager - getHDMIOutputIds, getHDMIOutput</li><li> IHDMIOutput - open(), close()</li><li> IHDMIOutputController - start(), getHDCPStatus(), getHDCPCurrentVersion(), stop()</li><li> IHDMIOutputControllerListener - onHDCPStatusChanged()</li></ul>|
|Verify HDCP Status Failure callback for each HDMI Output|<ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Verify the HDCP status change event for each port after open or started.</li><li>Verify callback for UNKNOWN when connected HDMI output device is not powered after OPENING</li><li>Verify callback for UNAUTHENTICATED when connected HDMI output device HDCP is disabled</li></ul> |<ul><li>IHDMIOutputManager - getHDMIOutputIds, getHDMIOutput</li><li> IHDMIOutput - open(), close()</li><li> IHDMIOutputController - start(), getHDCPStatus(), stop()</li><li> IHDMIOutputControllerListener - onHDCPStatusChanged()</li></ul>|
|Stress test on HDCP Status change for each HDMI Output|<ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Verify the HDCP status change event for each port after open or started.</li><li>Perform continuous plug/unplug in a loop and ensure each transition through the various HDCP states</li></ul> |<ul><li>IHDMIOutputManager - getHDMIOutputIds, getHDMIOutput</li><li> IHDMIOutput - open(), close()</li><li> IHDMIOutputController - start(), getHDCPStatus(), stop()</li><li> IHDMIOutputControllerListener - onHDCPStatusChanged()</li></ul>|
|Verify on E-EDID successfully retrieved callback for each HDMI Output|<ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Verify the onEDID event for each port when HDMI Output device is connected after READY.</li><li>Verify the onEDID event for each port when HDMI Output device is connected before READY.</li><li>Reads the E-EDID after on successfull event is retrieved for each port </li></ul> |<ul><li>IHDMIOutputManager - getHDMIOutputIds, getHDMIOutput</li><li> IHDMIOutput - open(), close()</li><li> IHDMIOutputController - start() getHDCPStatus(), stop()</li><li> IHDMIOutputControllerListener - onEDID()</li></ul>|
| Verify SPD InfoFrame Transmission| <ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>Verify correct SPDInfoFrame set and get operation after opening the HDMI output port instance</li><li>Verify whether SPD information is being sent at regular intervals after the source device starts outputting video.</li></ul> | <ul><li>IHDMOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(), close()</li><li>IHDMIOutputController - start(), setSPDInfoFrame(), getSPDInfoFrame(), stop()</li></ul> |
|Verifies the set property applied on HDMI output device|<ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>set the property value after opening/starting the HDMI output port instance</li><li>Property:VIC, CONTENT_TYPE, AFD, HDR_OUTPUT_MODE, SCAN_INFORMATION</li><li>Verify the change is applied to HDMI output device</li></li>|<ul><li>IHDMIOutputManager - getHDMIOutputIds, getHDMIOutput</li><li> IHDMIOutput - open(), getProperty(), close()</li> <li> IHDMIOutputController - start(), setProperty(), setPropertyMulti(), stop()</li></ul>|
|Verifies the VIC(Video ID code) change event|<ul><li> Reads supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li>set the VIC property value after opening/starting the HDMI output port instance</li><li>Verify the VIC (Video ID Code) change event is notified after state STARTED and HDCP authentication.</li>|<ul><li>IHDMIOutputManager - getHDMIOutputIds, getHDMIOutput</li><li> IHDMIOutput - open(), getProperty(), getPropertyMulti(), close()</li> <li> IHDMIOutputController - start(), setProperty(), setPropertyMulti()  stop()</li><li> IHDMIOutputControllerListener - onFrameRateChanged()</li></ul>|

