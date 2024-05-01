#!/bin/bash

mkdir build
cd ./build

#install deps
packages=(
    "cmake"
    "cmake-gui"
    "libboost-date-time-dev"
    "libboost-dev"
    "libboost-filesystem-dev"
    "libboost-graph-dev"
    "libboost-iostreams-dev"
    "libboost-program-options-dev"
    "libboost-python-dev"
    "libboost-regex-dev"
    "libboost-serialization-dev"
    "libboost-thread-dev"
    "libcoin-dev"
    "libeigen3-dev"
    "libgts-bin"
    "libgts-dev"
    "libkdtree++-dev"
    "libmedc-dev"
    "libocct-data-exchange-dev"
    "libocct-ocaf-dev"
    "libocct-visualization-dev"
    "libopencv-dev"
    "libproj-dev"
    "libpyside2-dev"
    "libqt5opengl5-dev"
    "libqt5svg5-dev"
    "qtwebengine5-dev"
    "libqt5x11extras5-dev"
    "libqt5xmlpatterns5-dev"
    "libshiboken2-dev"
    "libspnav-dev"
    "libvtk7-dev"
    "libx11-dev"
    "libxerces-c-dev"
    "libzipios++-dev"
    "occt-draw"
    "pyside2-tools"
    "python3-dev"
    "python3-matplotlib"
    "python3-packaging"
    "python3-pivy"
    "python3-ply"
    "python3-pyside2.qtcore"
    "python3-pyside2.qtgui"
    "python3-pyside2.qtsvg"
    "python3-pyside2.qtwidgets"
    "python3-pyside2.qtnetwork"
    "python3-pyside2.qtwebengine"
    "python3-pyside2.qtwebenginecore"
    "python3-pyside2.qtwebenginewidgets"
    "python3-pyside2.qtwebchannel"
    "python3-markdown"
    "python3-git"
    "python3-pyside2uic"
    "qtbase5-dev"
    "qttools5-dev"
    "swig"
    "libyaml-cpp-dev"
)

installable_deps=()

for dep in "${packages[@]}"
do
	if apt-cache showpkg "$dep" &> /dev/null; then
		installable_deps+=("$dep")
        	echo "$dep is installable"
        	
    	else
        	echo "$dep is not installable"
    	fi
done

sudo apt-get install $installable_deps

sudo cmake ../
sudo make -j$(nproc --ignore=4)

