# Vision-Tracking-Format
A simple vision tracking format to use and create other programs with, by deafult it should be somewhat setup for the 2019 FRC game Deep Space

Vision Tracking Documentation.
===

## All Vision Tracking code is located in the folder [Vision](vision)

# Getting started
### 1. Grab the image:
  - [Tinkerboard Image](https://dlcdnets.asus.com/pub/ASUS/mb/Linux/Tinker_Board_S/20181023-tinker-board-linaro-stretch-alip-v2.0.8.img.zip)
    - Make sure you use version 2.0.8! v1.8 doesn't work with some USB cameras (nor the Kinect) due to a kernel problem.
  - [Raspberry Pi](https://downloads.raspberrypi.org/raspbian_lite_latest)
### 2. Flash the image onto a microsd card using etcher: https://etcher.io/
  - You will need to grab the .img file from the zip. The .img file is what you give to etcher.
  - After complete, insert the microSD card and startup the coprocessor. It may take a minute or two.
### 3. SSH into the Coprocessor
  - On Mac/Linux, use `ssh username@hostname`
  - On Windows, download [puTTY](https://the.earth.li/~sgtatham/putty/latest/w64/putty-64bit-0.70-installer.msi) and use that.
  - Tinkerboard:
    - Hostname: `tinkerboard`, Username: `linaro`, Password: `linaro`
  - Raspberry Pi:
    - Hostname: `raspberrypi`, Username: `pi`, Password: `raspberry`
### 4. Run the following command:
  - `wget --no-check-certificate -qO - https://github.com/CJBuchel/Vision-Tracking-Format/blob/master/vision/bootstrap.sh?raw=1 | bash`
### 5. Your Coprocessor will now restart
### 6. Run your code locally on your computer using (`.\gradlew :vision:runvision` or simply type `.\gradlew runvision`)
### 7. Deploy `./gradlew :vision:deploy` (`./gradlew :vision:deploy -Praspberry` for the Raspberry Pi) to deploy your code!



