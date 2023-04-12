#!/bin/sh
#This script aims to configure the network on the targeted machine.
#It is executed at the end of the boot, just before the client program.

ip addr add 192.168.196.103/24 dev eth0
ip link set dev eth0 up
ip route add default via 192.1.68.196.254