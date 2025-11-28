# HDMI Output High Level Test Specification Document

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Introduction](#introduction)
- [Module Description](#module-description)
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

- Hdmi Output Interface Definitions - [Hdmi output HAL](https://github.com/rdkcentral/rdk-halif-aidl/tree/0.12.0/hdmiputput/current/com/rdk/hal/hdmioutput)

- Hdmi Output HAL Feature Profile - [hfp-hdmioutput.yaml](https://github.com/rdkcentral/rdk-halif-aidl/tree/main/hdmiputput/current/hfp-hdmioutputtodo.yaml)


## L2 Test Specification

| Test | Description | HAL APIs |
|------|-------------|----------|
| Get the State machine transitions for HDMI Output port | <ul><li>Verifies the current HDMI output state when IHDMOutputEventListener.onStateChanged() changes</li><li> It Read supported output ports using IHDMIOutputManager class</li><li> Loop through supported output port </li><li> Register event listeners using registerEventListener </li><li>Ensure state transitions (open -> start -> stop -> close) work as expected using getState</li> <li> Validate event listener notifications using onStateChanged </li></ul>| <ul><li>IHDMIOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(), getState(), registerEventListener(), unregisterEventListener(), close()</li><li>IHDMIOutputController - start(), stop()</li></ul> |
|Verify HotPlugDetect state of each HDMI input |<ul><li>Verifies current HPD state for a device attached to the HDMI output port.</li><li> HotPlugState will be FALSE if port is not opened </li></ul> |<ul><li>IHDMOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(),close()</li><li>IHDMIOutputController - start(),getHotPlugDetectState() stop()</li> <li> IHDMIOutputControllerListener - onHotPlugDetectStateChanged()</li></ul>|
| Validate HDMI output property using set/get | Verifies individual HDMI output property set and get operation | <ul><li>IHDMOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(), getProperty(), close()</li><li>IHDMIOutputController - start(),setProperty(), stop()</li></ul> |
| Validate HDMI output multi-property set/get | Verifies setting of multiple property-value pairs set and get operation | <ul><li>IHDMOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(), getPropertyMulti(), close()</li><li>IHDMIOutputController - start(), setPropertyMulti(), stop()</li></ul> |
| Verify SPD InfoFrame set/get| Verify correct SPDInfoFrame set and get operation | <ul><li>IHDMOutputManager - getHDMIOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(), getSPDInfoFrame(), close()</li><li>IHDMIOutputController - start(), getSPDInfoFrame(), stop()</li></ul> |
|Verify HDCP version |<ul><li>Reads the HDCP version for each port </li><li>If HDCP has not yet been authenticated then `HDCPProtocolVersion.UNDEFINED` is returned</li></ul>  |<ul><li>IHDMOutputManager - getHDMOutputIds, getHDMIOutput</li><li>IHDMIOutput - open(), close()<li>IHDMIOutputController - start(), getHDCPCurrentVersion(), stop()</li></ul> |


## L3 Test Specification

| Test | Description | HAL APIs used |
|-----|------------|-------------|
|Verify state transition callback of each HDMI output |<ul><li> Register event listeners using registerEventListener </li><li>Ensure state transitions (open -> start -> stop -> close) work as expected using getState()</li> <li> Validate event listener callbacks using onStateChanged() </li></ul> |<ul><li>IHDMIOutputManager - getHDMIInputIds, getHDMIInput</li><li>IHDMIOutput - open(),getState(), registerEventListener(), unregisterEventListener(), close()</li><li>IHDMIOutputController - start(), stop()</li><li> IHDMIOutputEventListener - onStateChanged()</li></ul>|
|Verify HotplugDetect state callback of each HDMI output |<ul><li>Verifies HotplugDetect state for a device attached to the HDMI output port.</li><li> HPD state  will TRUE if port is  connected to sink </li></ul> |<ul><li>IHDMIOutputManager - getHDMIInputIds, getHDMIInput</li><li>IHDMIOutput - open(),close()</li><li>IHDMIOutputController - start(),getHotPlugDetectState() stop()</li><li> IHDMIOutputControllerListener - onHotPlugDetectStateChanged()</li></ul>|
|Verifies the VIC(Video ID code) change event|<ul><li> Reads the video framerate change event from the source to HDMI input </li><li>The VIC (Video ID Code) change event is notified when the framerate is changed HDMI output.</li></ul>|<ul><li>IHDMIOutputManager - getHDMIInputIds, getHDMIInput</li><li> IHDMIOutput - open(), getProperty(), getPropertyMulti(), close()</li> <li> IHDMIOutputController - start(), setProperty(), setPropertyMulti(), stop()</li><li> IHDMIOutputControllerListener - onFrameRateChanged()</li></ul>|
|Verify HDCP Status change callback for each HDMI Output|<ul><li>Reads the HDCP status change event for each port </li><li>For connected output port provides the E-EDID</li></ul> |<ul><li>IHDMIOutputManager - getHDMIInputIds, getHDMIInput</li><li> IHDMIOutput - open(), close()</li><li> IHDMIOutputController - start(), stop()</li><li> IHDMIOutputControllerListener - onHDCPStatusChanged()</li></ul>|
|Verify on E-EDID successfully retrieved callback for each HDMI Output|<ul><li>Reads the E-EDID after on successfull even is retrieved event for each port </li><li>For connected output port returns the HDCP status</li> <li>if HDCP state is changed can be notified through IHDMIOutputControllerListener.onHDCPStatusChanged()</li></ul> |<ul><li>IHDMIOutputManager - getHDMIInputIds, getHDMIInput</li><li> IHDMIOutput - open(), close()</li><li> IHDMIOutputController - start() getHDCPStatus(), stop()</li><li> IHDMIOutputControllerListener - onEDID()</li></ul>|
