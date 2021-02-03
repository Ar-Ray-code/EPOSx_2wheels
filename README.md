# EPOSx_2wheels

## Description

- This program is for communicating and controlling two Maxon Motors (Motor Driver : EPOS4).
- The message is communicated by a text file. So it doesn't depend on ROS1
- Don't load ROS1 environment.

## Installation (On "epos_2wheels_txt/" directory)

1. Run `$ sudo bash EPOS_INSTALL/install.sh` to install EPOSx library.
2. Run `$ mkdir build && cd build && cmake .. && make ` to compile programs.

## Usage

Example : `$ ./epos_2wheels --yaml_file ../yaml/cfg_EPOS.yaml --wheel_spd_path ../yaml/`

#### Options 

- yaml_file : EPOSx configure file.
- wheel_spd_path : Text folder path to communicate to ROS.

## How to control?

- Executing epos_2wheels, please run this command on other shell on `yaml/`directory.

  `$ >EPOS0.txt && echo "100" >> EPOS0.txt`

- After run this, `EPOS0.txt` is written "100" and MAXON Motor will spin 100 rpm.

- Please Edit `EPOS0.txt` and `EPOS1.txt` to control motors. I have [ROS1 package](https://github.com/Ar-Ray-code/twist_2wheel_pub) to edit these automatically.

## Caution (Bug)

- Don't load ROS1. If you want to use ROS1 environment, Please use other terminal and load this.

## Licence

- Apache2.0