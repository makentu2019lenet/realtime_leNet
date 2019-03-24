# USAGE
# python real_time_object_detection.py --prototxt MobileNetSSD_deploy.prototxt.txt --model MobileNetSSD_deploy.caffemodel

# import the necessary packages
from imutils.video import VideoStream
from imutils.video import FPS
import numpy as np
import argparse
import imutils
import time
import cv2

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-p", "--prototxt", required=True,
	help="path to Caffe 'deploy' prototxt file")
ap.add_argument("-m", "--model", required=True,
	help="path to Caffe pre-trained model")
ap.add_argument("-c", "--confidence", type=float, default=0.2,
	help="minimum probability to filter weak detections")
args = vars(ap.parse_args())

# initialize the list of class labels MobileNet SSD was trained to
# detect, then generate a set of bounding box colors for each class
CLASSES = ["brick", "carpet", "ceramic", "fabric", "foliage",
	"food", "glass", "hair", "leather", "metal", "mirror", "other",
	"painted", "paper", "plastic", "polishedstone", "skin", "sky",
	"stone", "tile", "wallpaper", "water", "wood"]
COLORS = np.random.uniform(0, 255, size=(len(CLASSES), 3))

# load our serialized model from disk
print("[INFO] loading model...")
net = cv2.dnn.readNetFromCaffe(args["prototxt"], args["model"])

# initialize the video stream, allow the cammera sensor to warmup,
# and initialize the FPS counter
print("[INFO] starting video stream...")
vs = VideoStream(src=0).start()
time.sleep(2.0)
fps = FPS().start()

# loop over the frames from the video stream
while True:
	   # grab the frame from the threaded video stream and resize it
	   # to have a maximum width of 400 pixels
	   frame = vs.read()
	   frame = imutils.resize(frame, width=400)
	   # grab the frame dimensions and convert it to a blob
	   # google net just only work in 224*224
	   (h, w) = frame.shape[:2]
	   blob = cv2.dnn.blobFromImage(frame,1, (224, 224), (104, 117, 123))
	   # pass the blob through the network and obtain the detections and
	   # predictions
	   # the output predicted probabilities for each of the 1,000 ImageNet
       # classes
	   net.setInput(blob)
	   preds = net.forward()
           idxs = np.argsort(preds[0])[::-1][:5]
	   for (i, idx) in enumerate(idxs):
	   # draw the top prediction on the input image
	       if i == 0:
		    text = "Label: {}, {:.2f}%".format(CLASSES[idx],preds[0][idx] * 100)
		    cv2.putText(frame, text, (5, 25),cv2.FONT_HERSHEY_SIMPLEX,0.7,(0, 0, 255),2)
		    print("[INFO] {}. label: {}, probability: {:.5}".format(i + 1,CLASSES[idx], preds[0][idx]))



	# update the FPS counter
	       fps.update()

# stop the timer and display FPS information
fps.stop()
print("[INFO] elapsed time: {:.2f}".format(fps.elapsed()))
print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))

# do a bit of cleanup
cv2.destroyAllWindows()
vs.stop()
