if [%2] == [] goto usage
make os-image netSetup=%1 clientO=%2
goto :eof

:usage
@echo off
@echo "Usage: ./win_launch.bat <PATH_TO_NETSETUP> <PATH_TO_CLIENT>"
@echo "    <PATH_TO_NETSETUP> The path to the network setup script"
@echo "    <PATH_TO_CLIENT> The path to the client builds"
exit /B 1