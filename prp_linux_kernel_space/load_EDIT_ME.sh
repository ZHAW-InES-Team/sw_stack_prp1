#!/bin/sh
# Interface names
INTERFACE_NAME_A=eth1
INTERFACE_NAME_B=eth2

# If the set mac function does not work set it to 1
PROMISCOUS_MODE=0

# Debug level from 0 to 5 (5: very verbose)
DEBUG_LEVEL=0


# Do not edit from here!
echo shutting down interface prp0
ifconfig prp0 down

echo unloading driver prp_dev
rmmod prp_dev

echo compile driver
make clean
make

echo loading driver for interfaces A:$INTERFACE_NAME_A and B:$INTERFACE_NAME_B and creating virtual device prp0
insmod prp_dev.ko dev_name_A="$INTERFACE_NAME_A" dev_name_B="$INTERFACE_NAME_B" dev_promisc=$PROMISCOUS_MODE prp_debug_level=$DEBUG_LEVEL

echo starting up interface prp0
ifconfig prp0 up

