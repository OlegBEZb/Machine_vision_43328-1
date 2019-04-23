import os
import cv2
import numpy as np
from skeletonization import skeletonization
from connect_lines import nothing, connectLines


video_num = 1
video = cv2.VideoCapture('Video/' + str(video_num) + '.avi')
frame = video.read()[1]

num_of_frames = 5
shift_btw_frames = 100
# therefore in this work will be processed frames 101, 201, ..., 501

for i in range(num_of_frames):
    print('\nprocessing frame number ', i)

    # skip frames to avoid multicollinearity
    for j in range(shift_btw_frames):
        frame = video.read()[1]
    cv2.imshow('original fr' + str(i) + '_video' + str(video_num), frame)
    

    # creating window with trackbars
    cv2.namedWindow('main', flags=cv2.WINDOW_AUTOSIZE)
    cv2.createTrackbar('bl', 'main', 79, 255, nothing)
    cv2.createTrackbar('bu', 'main', 166, 255, nothing)
    cv2.createTrackbar('gl', 'main', 84, 255, nothing)
    cv2.createTrackbar('gu', 'main', 145, 255, nothing)
    cv2.createTrackbar('rl', 'main', 47, 255, nothing)
    cv2.createTrackbar('ru', 'main', 119, 255, nothing)
    
    # make a equals anything except 'z' which will means ending of the configuring
    #a = 0
    #while(a != 'z'):
    bl = cv2.getTrackbarPos('bl', 'main')
    bu = cv2.getTrackbarPos('bu', 'main')
    gl = cv2.getTrackbarPos('gl', 'main')
    gu = cv2.getTrackbarPos('gu', 'main')
    rl = cv2.getTrackbarPos('rl', 'main')
    ru = cv2.getTrackbarPos('ru', 'main')

    # blur a little to prevent mistakes with binarization
    img_blurred = cv2.GaussianBlur(frame, (5, 5), 0)
    img_bin = np.zeros((img_blurred.shape[0], img_blurred.shape[1]), dtype=np.uint8)
    # binarize the blurred image
    cv2.inRange(src=img_blurred, lowerb=(bl, gl, rl), upperb=(bu, gu, ru), 
                dst=img_bin)
    #cv2.imshow('binarization fr' + str(i) + '_video' + str(video_num), img_bin)

    cv2.dilate(img_bin, (5, 5), img_bin)
    cv2.erode(img_bin, (5, 5), img_bin)
    cv2.erode(img_bin, (5, 5), img_bin)
    cv2.dilate(img_bin, (5, 5), img_bin)
    cv2.dilate(img_bin, (5, 5), img_bin)
    cv2.dilate(img_bin, (5, 5), img_bin)
    #cv2.imshow('erode and dilate fr' + str(i) + '_video' + str(video_num), img_bin)

    # prepare image format to skeletonization: 255 -> 1, another -> 0
    img_bin = img_bin // 255

    # Zhang-Suen algorithm
    # load if file exists, else process
    if os.path.isfile('Frames/skelet_'+ 'video' + str(video_num) + \
                      '_fr' + str(i) + '.png'):
        print('video' + str(video_num) + '_fr' + str(i) + '_exists')
        img_skeletized = cv2.imread('Frames/skelet_' + 'video' + \
                                    str(video_num) + '_fr' + str(i) + '.png', 
                                    cv2.IMREAD_GRAYSCALE)
    else:
        print('video' + str(video_num) + '_fr' + str(i) + '_not_exists')
        img_skeletized = skeletonization(img_bin)
        cv2.imwrite('Frames/skelet_'+ 'video' + str(video_num) + \
                    '_fr' + str(i) + '.png',
                    img_skeletized)
        

    # back to visible format: 1 -> 255, 0 -> 0
    img_skeletized *= 255
    cv2.imshow('skeletonization fr' + str(i) + '_video' + str(video_num), 
               img_skeletized)
    
    frame_with_path = connectLines(img_skeletized, frame)
    cv2.imshow('with_path fr' + str(i) + '_video' + str(video_num), 
               frame_with_path)
    
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    

cv2.waitKey(0)