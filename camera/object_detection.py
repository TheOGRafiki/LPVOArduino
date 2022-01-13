# TensorFlow Lite Object Detection Example
#
# This example shows off object detection. Object detect is much more powerful than
# object classification. It can locate multiple objects in the image.

import sensor
import image
import time
import os
import tf

sensor.reset()                         # Reset and initialize the sensor.
# Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)      # Set frame size to QVGA (320x240)
sensor.set_windowing((240, 240))       # Set 240x240 window.
sensor.skip_frames(time=2000)          # Let the camera adjust.

net = tf.load('<object_detection_network>', load_to_fb=True)
labels = []

try:  # Load labels if they exist
    labels = [line.rstrip('\n') for line in open("labels.txt")]
except:
    pass

colors = [  # Add more colors if you are detecting more than 7 types of classes at once.
    (255,   0,   0),
    (0, 255,   0),
    (255, 255,   0),
    (0,   0, 255),
    (255,   0, 255),
    (0, 255, 255),
    (255, 255, 255),
]

clock = time.clock()
while(True):
    clock.tick()

    img = sensor.snapshot()

    # detect() segments an object using the provided segmentation model. This produces mutliple
    # grayscale images per object class that we are trying to detect. detect() then runs
    # find_blobs() internally on the segmented images to find all blob locations and then returns
    # the bound boxes of all blobs found per object class. So, detect() returns a list of lists of
    # classification objects and the respective confidence level.

    for i, detection_list in enumerate(net.detect(img, thresholds=[(128, 255)])):
        if (i < len(labels)):
            print("********** %s **********" % labels[i])
        for d in detection_list:
            print(d)
            img = img.draw_rectangle(d.rect(), color=colors[i])

    print(clock.fps(), "fps", end="\n\n")
