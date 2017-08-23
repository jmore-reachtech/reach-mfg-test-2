## Manufacturing Test
```bash
# mfg--test --help
	Perform Manufacturing Test
    
    Usage: mfg-test [options]
    
    Options:
    	-h, --help          Display this help and exit
        -v, --verbose       Enable verbose output
        -t, --tests         Tests to run
            --list-tests    List available tests and exit
            --half-duplex   Set AUART3 (J25) to half duplex
            --mac-address   Burn in MAC address
            --splash        Splash image to panel
            --version       Display Program version
            
    Environment:
    	TEST_WEB_SERVER_ADDR	Web server address to test ETHERNET
        							Defaults to: 10.10.10.2
        TEST_RTC_SERVER_ADDR	Time server address to test RTC
        							Defaults to: 10.10.10.2
        TEST_MODULE_ADDR		Module local IP address
        							Defaults to: 10.10.10.3
        TEST_MODULE_GATEWAY		Module default gateway address
        							Defaults to: not set
```
## List Tests
```bash
# mfg-test --list-tests

Available Manufacturing Tests

TOUCH     (J0)
AUART1    (J2)
AUART3    (J25)
AUART4    (J21)
I2C       (J21)
CAN       (J20)
ETHERNET  (J3)
USB1      (J4)
USB2      (J5)
GPIO      (J22)
FLASH     (J0)
LCD       (J0)
BEEPER    (L52)
SPEAKER   (L52)
RTC       (J0)
```
## TOUCH Test
```bash
# mfg-test --tests TOUCH
```
### Expected Results:
This **TOUCH** test will launch ts_calibrate. The ability to touch calibrate the panel is a successful test.

## AUART1 (J2) Test
```bash
# mfg-test --tests AUART1
```
### Expected Results:
The **AUART** test will send the character **'a'** and expects to receive the letter **'A'**.

## AUART3 (J25) Test
- For full duplex boards add the **--half-duplex** option
```bash
# mfg-test --tests AUART3
```
### Expected Results:
The **AUART** test will send the character **'a'** and expects to receive the letter **'A'**.

## AUART4 (J21) Test
```bash
# mfg-test --tests AUART4
```
### Expected Results:
The **AUART** test will send the character **'a'** and expects to receive the letter **'A'**.

## I2C (J21) Test
```bash
# mfg-test --tests I2C
```
### Expected Results:
The **I2C** test expects to see a slave address of **0x05** with a control register of **0xC**. A successful test will receive one byte of **0x3**.

## CAN (J20) Test
```bash
# mfg-test --tests CAN
```
### Expected Results:
The **CAN** test sets the CAN frame ID to **0x3E** sets 2 bytes for the payload. The payload bytes are **0xDE** and **0xAD**. The CAN test expects to read one CAN frame with a 2 byte payload of **0xBE** and **0xEF**.

## ETHERNET (J3) Test
- The default server address for the **ETHERNET** test is **10.10.10.2**. The sever address can be overwritten by setting the environment variable **TEST_WEB_SERVER_ADDR**.

```bash
# mfg-test --tests ETHERNET
```
```bash
# TEST_WEB_SERVER_ADDR=192.168.0.100 mfg-test --tests ETHERNET
```
### Expected Results:
The **ETHERNET** is successful if the follopwing URL can be accessed, http://TEST_WEB_SERVER_ADDR/file-32M.

## USB1 (J4) Test
```bash
# mfg-test --tests USB1
```
### Expected Results:
The **USB** test expects an SD card with one FAT32 partition to be available at **/dev/sda1**. A successful test is when the test can write random data to the device.

**Note:** Make sure the SD cards are attached before powering the module. Dynamically plugging in an SD card can swap the order of **USB1** and **USB2**.

## USB2 (J5) Test
```bash
# mfg-test --tests USB2
```
### Expected Results:
The **USB** test expects an SD card with one **FAT32** partition to be available at **/dev/sdb1**. A successful test is when the test can write random data to the device.

**Note:** Make sure the SD cards are attached before powering the module. Dynamically plugging in an SD card can swap the order of **USB1** and **USB2**.

## GPIO (J22) Test
```bash
# mfg-test --tests GPIO
```
### Expected Results:
The **GPIO** test will walk a 1 across the lower four pins and then the upper four pins. A successful test is a write/read of each pin.

## FLASH (J0) Test
```bash
# mfg-test --tests FLASH
```
### Expected Results:
The **FLASH** test will run nandtest over each MTD, /dev/mtd[0-6]. A successful test will be the test cumulative restutls of all the nandtest invocations.

**Note:** This test will read/write each page and can take up to **8** minutes.

## LCD (J0) Test
```bash
# mfg-test --tests LCD
```
### Expected Results:
The **LCD** test will write an image to the framebuffer. Manual inspection is required to validate the image rendering is correct.

## BEEPER (L52) Test
- Partial stuff boards only

```bash
# mfg-test --tests BEEPER
```
### Expected Results:
The **BEEPER** test send a tone to the speaker 5 times. Manual inspection is required to validate the tone is correct.

## SPEAKER (L52) Test
- Full stuff boards only

```bash
# mfg-test --tests SPEAKER
```
### Expected Results:
The **SPEAKER** test plays an audio beep. Manual inspection is required to validate the audio is played correctly.

## RTC (J0) Test
- The default server address for the **RTC** test is **10.10.10.2**. The sever address can be overwritten by setting the environment variable **TEST_RTC_SERVER_ADDR**.

```bash
# mfg-test --tests RTC
```
```bash
# TEST_RTC_SERVER_ADDR=192.168.0.100 mfg-test --tests RTC
```
### Expected Results:
The **RTC** test will use the **TEST_RTC_SERVER_ADDR** to run the **rdate** command and set the system time. If the system time is set successfully the **RTC** test will set the hwclock based on the system time. A successful test is when both commands complete sucessfully.