# konro-DROM-EUPilot
A modern resource manager integrated with DROM from DLB [https://pm.bsc.es/dlb]


# Compilation instructions
* `mkdir build && cd build`
* `cmake ..`
* `make`

# Dependencies
- cmake
- pkg-config
- liblog4cpp5-dev
- libhwloc-dev
- libsensors-dev
- libopencv-dev
- DLB

OpenCV is required only for the peopledetect demo.
If it is not installed, peopledetect is not compiled.


