# Porting QtCore to Arm Mbed OS

The Arm Mbed OS is an open-source operating system designed specifically
for Internet of Things (IoT) devices using Arm microcontrollers. The
Mbed Partner Ecosystem brings together the embedded and semiconductor
companies, component manufacturers, and OEMs. The requirement is that
the board partner needs to contribute their board drivers to get the
Mbed-Enabled certificate.

By utilizing their Ecosystem, Qt can provide the consistent experience
on the Mbed-Enabled devices. The Qt developers can focus on the
application code of their embedded device, regardless their choice of
hardware.

This project, I want to cross-compile and port QtCore to MbedOS as my
first step because some of the boards only include an internal flash
from 1 to 2 MB and even less. Without GUI and debug info, QtCore can
still fit into such resource constrained devices (headless devices).

The future work is to enable Qt GUI (Qt Quick WebGL) with Ethernet/WiFi
for the headless devices.

The benefits of this project:
1. It helps developer to create applications easily by using the tools
   of QtCore (e.g. signal slot, property system).
2. The developer can utilize the serializing data types to communicate
   with nodes in low power and lossy networks.

* https://os.mbed.com/docs/v5.10/introduction/
* https://os.mbed.com/platforms/ (Mbed-Enabled boards)

Released Qt5 libraries ported to Arm Mbed-OS
https://github.com/ryanjh/Qt5

Examples:
https://os.mbed.com/users/ryanjh31011/code/qt5-mbed-example-blinky/
https://os.mbed.com/users/ryanjh31011/code/qt5-mbed-example-sys-info/
