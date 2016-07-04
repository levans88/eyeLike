CMake Instructions
------------------

NOTE: Adapt these settings/paths as needed for your system.

After OpenCV extract, add enviornment variable:
	
	setx -m OPENCV_DIR H:\OpenCV\Build\x64\vc14

Add to system path:

	%OPENCV_DIR%\bin

Copy include folder from:
	
	H:\OpenCV\build\include

To:
	
	H:\OpenCV\build\x64\vc14\lib\include

Run cmake-gui

Add root project source directory (not src if src isn't the root project folder).

Hit "Configure" button.

Choose appropriate Visual Studio version, for me it's Visual Studio 14 2015 Win64.

All other options are default (use default native compilers etc.)

Will fail, but if fails specifically saying missing compiler, try to make blank C++ project in Visual Studio first. It will update/download items, then compiler will be available.

After failure, change OpenCV_DIR to:
	
	H:/OpenCV/build/x64/vc14/lib

Configure again, then generate (and a Visual Studio project will be created).