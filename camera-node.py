# -*- coding: utf-8 -*-
import b0
import numpy as np
import cv2
import time

cam = cv2.VideoCapture(0)

node = b0.Node('python-camera-node')
camera_pub = b0.Publisher(node, 'camera')
node.init()

while not node.shutdown_requested():
    ret_val, img = cam.read()
    img = cv2.flip(img, 1)
    ok, arr = cv2.imencode('.jpg', img)
    if ok:
        msg = arr.tostring()
        print('Sending message %d' % len(msg))
        camera_pub.publish(msg)
    node.spin_once()
    time.sleep(0.1)

node.cleanup()

cv2.destroyAllWindows()

