# PIE
Page Image Explorer (PIE) is a visualization tool for document collections. The READ project  has  received  funding  from  the European  Union’s  Horizon  2020 research  and innovation programme under grant agreement No 674943.

[![Build Status](https://travis-ci.org/TUWien/PIE.svg?branch=master)](https://travis-ci.org/TUWien/PIE)
<!-- [![codecov](https://codecov.io/gh/TUWien/PIE/branch/master/graph/badge.svg)](https://codecov.io/gh/TUWien/PIE) -->

## Build on Windows
### Compile dependencies
- `Qt` SDK or the compiled sources (>= 5.9.0)
- `OpenCV` (>= 3.3.0)

### Compile ReadFramework
1. Clone the repository from `git@github.com:TUWien/PIE.git`
2. Open CMake GUI
3. set your ReadFramework folder to `where is the source code`
4. choose a build folder
5. Hit `Configure`
6. Set `QT_QMAKE_EXECUTABLE` by locating the qmake.exe
7. Set `OpenCV_DIR` to your OpenCV build folder
8. Hit `Configure` then `Generate`
9. Open the `PIE.sln` which is in your new build directory
10. Right-click the PIE project and choose `Set as StartUp Project`
11. Compile the Solution
12. enjoy

### If anything did not work
- check if you have setup OpenCV
- check if your Qt is set correctly (otherwise set the path to `qt_install_dir/qtbase/bin/qmake.exe`)
- check if your builds proceeded correctly

## Build on Ubuntu
Note that Qt 5.5 is needed, thus Ubuntu version must be >= 16.04 or backports of Qt 5.5 have to be used (see .travis.yml for an ppa repository and names packages which need to be installed).

Get required packages:

``` console
sudo apt-get install qt5-qmake qttools5-dev-tools qt5-default libqt5svg5 qt5-image-formats-plugins libopencv-dev cmake git
```

You also need OpenCV > 3.0. Either you can compile it yourself or perhaps you find a repository (you can also use the one from the .travis file, but be carefull, these packages are not tested, then you need following packages:
``` console
sudo apt-get install libopencv-dev libopencv-stitching-dev libopencv-imgcodecs-dev libopencv-flann-dev   libopencv-features2d-dev libopencv-calib3d-dev libopencv-hdf-dev libopencv-reg-dev libopencv-shape-dev libopencv-xobjdetect-dev libopencv-xfeatures2d-dev libopencv-ximgproc-dev libopencv-highgui-dev
```

Get the READ Framework sources from github:
``` console
git clone https://github.com/TUWien/PIE
```
This will by default place the source into ./PIE

Go to the PIE directory and run `cmake` to get the Makefiles:
``` console
cd PIE
cmake .
```

Compile READ Framework:
``` console
make
```

You will now have a binary (PIE), which you can test (or use directly). Also the build libraries are in this directory. To install it to /usr/local/bin, use:
``` console
sudo make install
```

### Authors
- Markus Diem
- Stefan Fiel
- Florian Kleber

### Links:
- [1] http://www.caa.tuwien.ac.at/cvl/
- [2] https://read.transkribus.eu/
- [3] https://github.com/TUWien/
- [4] http://nomacs.org
