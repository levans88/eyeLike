##Updates
- This repo was forked from Tristan Hume's work as described in the original readme below.
- I have converted constants to globals so the values can be adjusted at runtime.
- Trackbars were added using OpenCV's included portions of qt.
- I intended to add checkboxes to toggle some settings also but that requires full qt
  (which is out of scope for this weekend project...)
- I tested min/max valid values for each variable independently but it is *possible* to crash on certain combos
- The trackbars don't display values on the correct scale for floats but they are divided in the methods.
- Some methods were modified to prevent "0" and use at least "1" as a minimum value instead.
- Blur only works with odd numbers so currently I'm just doing +1 if even.
- Comments are not cleaned up yet.
- See "CMakeInstructions.md" for loose instructions on how to generate a Visual Studio project from the original.

##eyeLike
An OpenCV based webcam eye tracker based on a simple image gradient-based eye center algorithm by Fabian Timm.

## DISCLAIMER
**This does not track gaze yet.** It is basically just a developer reference implementation of Fabian Timm's algorithm that shows some debugging windows with points on your pupils.

If you want cheap gaze tracking and don't mind hardware check out [The Eye Tribe](https://theeyetribe.com/).
If you want webcam-based eye tracking contact [Xlabs](http://xlabsgaze.com/) or use their chrome plugin and SDK.
If you're looking for open source your only real bet is [Pupil](http://pupil-labs.com/) but that requires an expensive hardware headset.

##Status
The eye center tracking works well but I don't have a reference point like eye corner yet so it can't actually track
where the user is looking.

If anyone with more experience than me has ideas on how to effectively track a reference point or head pose
so that the gaze point on the screen can be calculated contact me.

##Building

CMake is required to build eyeLike.

###OSX or Linux with Make
```bash
# do things in the build directory so that we don't clog up the main directory
mkdir build
cd build
cmake ../
make
./bin/eyeLike # the executable file
```

###On OSX with XCode
```bash
mkdir build
./cmakeBuild.sh
```
then open the XCode project in the build folder and run from there.

###On Windows
There is some way to use CMake on Windows but I am not familiar with it.

##Blog Article:
- [Using Fabian Timm's Algorithm](http://thume.ca/projects/2012/11/04/simple-accurate-eye-center-tracking-in-opencv/)

##Paper:
Timm and Barth. Accurate eye centre localisation by means of gradients.
In Proceedings of the Int. Conference on Computer Theory and
Applications (VISAPP), volume 1, pages 125-130, Algarve, Portugal,
2011. INSTICC.

(also see youtube video at http://www.youtube.com/watch?feature=player_embedded&v=aGmGyFLQAFM)
