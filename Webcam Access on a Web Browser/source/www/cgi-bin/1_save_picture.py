#!/usr/bin/env python
import cv2, sys

# Constants
DEVICE_NUMBER = 0
IMAGE_FILE = "output.jpg"

# Init webcam
# http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#videocapture-videocapture
vc = cv2.VideoCapture(DEVICE_NUMBER)

# Check if the webcam init was successful
# http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#videocapture-isopened
if vc.isOpened(): # try to get the first frame
    # http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#videocapture-read
    retval, frame = vc.read()
else:
    sys.exit(1)

retval, frame = vc.read()

# Save the frame as an image file
# http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#imwrite
cv2.imwrite(IMAGE_FILE, frame)

# Read the output file
# http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#imread
img = cv2.imread(IMAGE_FILE)

# Show the image on the screen
# http://docs.opencv.org/2.4/modules/highgui/doc/user_interface.html#imshow
cv2.imshow("DB410c Workshop #1: Save Picture", img)

# Exit program after waiting indefinitely for a pressed key
# http://docs.opencv.org/2.4/modules/highgui/doc/user_interface.html#waitkey
cv2.waitKey(0)
