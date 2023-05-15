#!/bin/sh

if [ -z "$2" ]
  then
    echo "Usage: ./win_launch.bat <PATH_TO_NETSETUP> <PATH_TO_CLIENT>"
    echo "    <PATH_TO_NETSETUP> The path to the network setup script"
    echo "    <PATH_TO_CLIENT> The path to the client builds"
else
  make os-image netSetup=$1 clientO=$2
fi