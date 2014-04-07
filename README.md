Kinect-2-Face-Tracking-v1-port-and-OpenCV
===========================================

“This is based on preliminary software and/or hardware, subject to change.”

OVERVIEW:
The MFC part of the project is emtpy, and can be ignored.
FT2OpenCVHelpers folder contains the classes for managing Kinect 2, a port of Face Tracker 1 to work with Kinect 2, and an interface to OpenCV by converting the image frames into Mat.
Since we are using a hack that uses the old Face Tracker library made for Kinect 1, it require us to also connecdt (and open) a Kinect 1 camera during runtime.
Thus a class for managing Kinect 1 is also included.

METHOD:
For Face Tracker 1 to work with Kinect 2, it involved:
- changing resolution of Kinect 2 frames to 1280x960 for color and 640x480 for depth
(from 1920x1080 color and 512x424 depth)
(note: no transformation was made, just cropping the image from top left corner for the new size.  For the depth image where the new size is larger, I filled in the blanks with 0 values)
- supplying the mapping function DepthToColorMapper to Face Tracker during Initialize
- porting the GetClosestHint function for the Kinect 2's Body frames
- the new focal lengths used are 1064.36f for color and 364.38f for depth, as suggested by Stephen Schieberl
"I broke it up into horizontal FOV in radians (ax), vertical (ay), width (w), and height(h).

float fx = w / ( tan( ax * 0.5f ) * 2.0f );
float fy = h / ( tan( ay * 0.5f ) * 2.0f );
float f  = ( fx + fy ) * 0.5f;
ax = 84.0999985
ay = 53.7999992

Which results in a focal length of 1064.3584 for color. 
I get 364.378265 for depth using the same function. "


