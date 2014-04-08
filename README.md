Kinect-2-Face-Tracking-v1-port-and-OpenCV
===========================================

“This is based on preliminary software and/or hardware, subject to change.”

OVERVIEW:
FT2OpenCVHelpers folder contains the classes for managing Kinect 2, a port of Face Tracker 1 to work with Kinect 2, and an interface to OpenCV by converting the image frames into Mat.
Since we are using a hack that uses the old Face Tracker library made for Kinect 1, it require us to also connect (and open) a Kinect 1 camera during runtime.
Thus a class for managing Kinect 1 is also included.
The MFC part of the project is emtpy, and can be ignored.

METHOD:
For Face Tracker 1 to work with Kinect 2, it involved:
- changing resolution of Kinect 2 frames to 1280x960 for color and 640x480 for depth
(from 1920x1080 color and 512x424 depth)
(note: no transformation / scaling was made, simply just cropping the image)
- supplying the mapping function DepthToColorMapper to Face Tracker during Initialize
- porting the GetClosestHint function for the Kinect 2's Body frames
- the new focal lengths used are 827.8388f for color and 413.9194f for depth

ENVIRONMENT SETUP:
I'm using locally compiled x64 OpenCV binaries C:\SDKs\opencv\build3\lib\Debug[Release].  You should change this to your OpenCV lib binaries folder
