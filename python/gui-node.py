# -*- coding: utf-8 -*-
import b0
import numpy as np
import cv2
import sys
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QLabel
from PyQt5.QtGui import QIcon, QPixmap, QImage, qRgb

gray_color_table = [qRgb(i, i, i) for i in range(256)]

def toQImage(im, copy=False):
    if im is None:
        return QImage()
    if im.dtype == np.uint8:
        if len(im.shape) == 2:
            qim = QImage(im.data, im.shape[1], im.shape[0], im.strides[0], QImage.Format_Indexed8)
            qim.setColorTable(gray_color_table)
            return qim.copy() if copy else qim
        elif len(im.shape) == 3:
            if im.shape[2] == 3:
                qim = QImage(im.data, im.shape[1], im.shape[0], im.strides[0], QImage.Format_RGB888);
                return qim.copy() if copy else qim
            elif im.shape[2] == 4:
                qim = QImage(im.data, im.shape[1], im.shape[0], im.strides[0], QImage.Format_ARGB32);
                return qim.copy() if copy else qim
    raise RuntimeError('bad image shape: %s' % im.shape)

def camera_callback(msg):
    print('Received message %d' % len(msg))
    arr = np.frombuffer(msg, np.uint8)
    img = cv2.imdecode(arr, cv2.IMREAD_COLOR)
    label.setPixmap(QPixmap.fromImage(toQImage(img).rgbSwapped()))

node = b0.Node('python-camera-gui')
camera_sub = b0.Subscriber(node, 'camera', camera_callback)
#camera_sub.set_option(6, 1) # enable conflate
node.init()

app = QApplication(sys.argv)

window = QMainWindow()
window.resize(320, 200)
window.setWindowTitle('Camera GUI')
label = QLabel()
window.setCentralWidget(label)
window.show()

timer = QTimer()
timer.timeout.connect(lambda: node.spin_once())
timer.start(1000/24)

exitcode = app.exec_()

node.cleanup()

cv2.destroyAllWindows()

sys.exit(exitcode)
