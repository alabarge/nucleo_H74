..\utils\fw_ver.exe ..\H74_fw\build.inc ..\H74_fw\build.h ..\.git
echo f|xcopy ..\H74_fw\build.h ..\H74_fw\share\build.h /F /Y /R
echo f|xcopy ..\H74_fw\cp_srv\cp_msg.h ..\H74_fw\share\cp_msg.h /F /Y /R
echo f|xcopy ..\H74_fw\daq_srv\daq_msg.h ..\H74_fw\share\daq_msg.h /F /Y /R
