# USBINT1-C8051F380
USB device firmware for [Silab C8051F380](http://www.silabs.com/products/mcu/8-bit/c8051f38x/pages/c8051f38x.aspx)

## Source file
* `SPI0_TI` SPI function and TI Analog frontend protocol set
* `UART1_ATCommand` AT Command via UART1
* `USB0_Descriptor` USB Descriptions
* `USB0_Interrupt` USB and IO low-level interrupt routine
* `USB0_InterruptServerRoutine` USB high-level service routine
* `USB0_Standard_Request` Standard request protocol
* `USBINT1_Main` Main application

## Branches
* `afe4490` integration TI AFE4490 Pulse oximeter frontend and C8051F380
* `ads1298` integration TI ADS1298 ECG Analog frontend and C8051F380

## Tags
* `oxi-x.y.z` for Pulse oximeter firmware release
   example: oxi-0.1.1
* `ecg-x.y.z` for ECG firmware release
   example: ecg-0.1.0

## Related projects
 for [USBINT1](https://github.com/nodtem66/USBINT1) firmware
