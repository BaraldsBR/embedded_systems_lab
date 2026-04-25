# install guide for the raspberry pi! 
# (This script can also be executed as a shell if you know how [IMPORTANT] the TA's will NOT help with using this script.
# If you want to execute it as a shell please run it in the wanted directory
echo "[WARNING] the TA's have provided this and are NOT responsible for helping you"

## update your packages
sudo apt update
## make project folder (from git)
sudo apt install git
git clone https://git.ram.eemcs.utwente.nl/repository-esl/laboratory-files.git
cd laboratory-files

## install packages
sudo apt install yosys nextpnr-ice40-qt libboost-all-dev libeigen3-dev qtcreator qtbase5-dev qt5-qmake fpga-icestorm g++ raspi-config
sudo apt install git libgpiod-dev gpiod
# get the additional tooling
git clone https://git.ram.eemcs.utwente.nl/repository-esl/icoprog.git
cd icoprog
## Create the icoprog executable
g++ src/icoprog.cpp src/gpio_interface.cpp -o icoprog -lgpiodcxx

# BEFORE YOU DO ANYTHING THE .PCF file is still missing! This is located in the Assignments
