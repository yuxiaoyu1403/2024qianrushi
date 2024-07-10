# Edge Impulse - OpenMV Object Detection Example

import sensor, image, time, os, tf, math, uos, gc, struct, lcd
import Message
import math

net = None
labels = None
min_confidence = 0.5

class Dot(object):
    flag = 0
    x = 0
    y = 0
    num = 0
    hight = 0

Dot = Dot()

thresholds = [(54, 70, -22, 5, 15, 38)]

try:
    # load the model, alloc the model file on the heap if we have at least 64K free after loading
    net = tf.load("trained.tflite", load_to_fb=uos.stat('trained.tflite')[6] > (gc.mem_free() - (64*1024)))
except Exception as e:
    raise Exception('Failed to load "trained.tflite", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

try:
    labels = [line.rstrip('\n') for line in open("labels.txt")]
except Exception as e:
    raise Exception('Failed to load "labels.txt", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

colors = [ # Add more colors if you are detecting more than 7 types of classes at once.
    (255,   0,   0),
    (  0, 255,   0),
    (255, 255,   0),
    (  0,   0, 255),
    (255,   0, 255),
    (  0, 255, 255),
    (255, 255, 255),
]

def BanCheck(Img):
    img = Img

    # detect() returns all objects found in the image (splitted out per class already)
    # we skip class index 0, as that is the background, and then draw circles of the center
    # of our objects

    for i, detection_list in enumerate(net.detect(img, thresholds=[(math.ceil(min_confidence * 255), 255)])):
        if (i == 0): continue # background class
        if (len(detection_list) == 0): continue # no detections for this class?

        print("********** %s **********" % labels[i])
        for d in detection_list:
            [x, y, w, h] = d.rect()

            center_x = math.floor(x + (w / 2))
            center_y = math.floor(y + (h / 2))
            img.draw_circle((center_x, center_y, 12), color=colors[i], thickness=2)

            Dot.x = center_x
            Dot.y = center_y
            print('x %d\ty %d' % ( Dot.x, Dot.y))
            Dot.flag = 2

            if 110 <= Dot.x <= 130 and 110 <= Dot.y <= 130:
                ban_detected = True
                break

        if 120 <= Dot.x <=130 and 110 <= Dot.y <= 130:
            return 1
        else:
            return 0


def Number(Img):
    img = Img
    a=0
    for blob in img.find_blobs(
        [thresholds[0]],
        pixels_threshold=200,
        area_threshold=200,
        merge=True,
    ):
        # These values depend on the blob not being circular - otherwise they will be shaky.
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        a+=1
    print(a)
    Dot.num = a
    return a
