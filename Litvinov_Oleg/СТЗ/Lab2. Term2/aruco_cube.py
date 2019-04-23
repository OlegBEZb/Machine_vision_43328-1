import numpy as np
import cv2
import cv2.aruco as aruco
import os
import pickle

from calibration import camera_calibration_by_aruco_board
from generate import generate_board, generate_marker


def draw(img, imgpts, corners):
    """
    Draws a cube on the image. For more simple debugging process bottom edge of
    the cube coloured green and the top coloured red. 
    
    In addition marker corner are drawn
    
    Parameters
    ----------
    img : 
        original frame on which the cube will be projected
    imgpts : numpy.ndarray of shape(8, 1, 2)
        image points in camera coordinate system
    corners : list of numpy.array of shape(1, 4, 2)
        positions of marker corners on input image
    
    Returns
    -------
    None
    """
    
    # Given an array of detected marker corners and its corresponding 
    # ids, this functions draws the markers in the image.
    img = aruco.drawDetectedMarkers(img, corners, borderColor=(0, 0, 255))
    
    imgpts = np.int32(imgpts).reshape(-1,2)
    # draw ground floor in green
    img = cv2.drawContours(img, [imgpts[:4]], -1,(0,255,0),-3)
    # draw pillars in blue color
    for i,j in zip(range(4),range(4,8)):
        img = cv2.line(img, tuple(imgpts[i]), tuple(imgpts[j]),(255),3)
    # draw top layer in red color
    img = cv2.drawContours(img, [imgpts[4:]], -1,(0,0,255),-3)
    cv2.imshow('drawing_axis', img)


def detection_markers():
    """
    
    """
    # Constant parameters used in Aruco methods
    ARUCO_PARAMETERS = aruco.DetectorParameters_create()
    ARUCO_DICT = aruco.Dictionary_get(aruco.DICT_5X5_1000)

    
    pckl_file_exist = False
    if not os.path.exists('calibration.pckl'):
        print("You need to calibrate the camera you'll be using. See \
              calibration project directory for details.")
        exit()
    else:
        pckl_file_exist = True
        f = open('calibration.pckl', 'rb')
        (cameraMatrix, distCoeffs) = pickle.load(f)
        f.close()
        if cameraMatrix is None or distCoeffs is None:
            print("Calibration issue. Remove ./calibration.pckl and recalibrate\
                  your camera with CalibrateCamera.py.")
            exit()

    # Create grid board object we're using in our stream
    board = aruco.GridBoard_create(
        markersX=5,
        markersY=7,
        markerLength=0.04,
        markerSeparation=0.01,
        dictionary=ARUCO_DICT)

    # Create vectors we'll be using for rotations and translations for postures
    rvecs, tvecs = None, None

    cam = cv2.VideoCapture(0)

    while (cam.isOpened() and pckl_file_exist):
        
        # Exit at the end of the video on the 'q' keypress
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
        
        # Capturing each frame of our video stream
        ret, img = cam.read()
        if ret == True:
            # grayscale image
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

            # Detect Aruco markers
            corners, ids, rejectedImgPoints = aruco.detectMarkers(
                gray, ARUCO_DICT, parameters=ARUCO_PARAMETERS)


            # Refind not detected markers based on the already detected and 
            # the board layout.
            corners, ids, _, _ = aruco.refineDetectedMarkers(
                image=gray,
                board=board,
                detectedCorners=corners,
                detectedIds=ids,
                rejectedCorners=rejectedImgPoints,
                cameraMatrix=cameraMatrix,
                distCoeffs=distCoeffs)

            side_len = 0.5
            axis = np.float32([[-side_len, -side_len, 0], [-side_len, side_len, 0], 
                               [side_len, side_len, 0], [side_len, -side_len, 0],
                               [-side_len, -side_len, 1], [-side_len, side_len, 1], 
                               [side_len, side_len, 1], [side_len, -side_len, 1]])
        
            # Draw cube only when there is only one marker in a frame
            if ids is not None and len(ids) == 1:
                rvecs, tvecs, _ = aruco.estimatePoseSingleMarkers(
                    corners, ids, cameraMatrix, distCoeffs)
                
                # The function computes projections of 3D points to the image 
                # plane given intrinsic and extrinsic camera parameters. 
                # imgpts - output array of image points in camera coordinate
                # system (x and y)
                imgpts, _ = cv2.projectPoints(axis, rvecs, tvecs, 
                                              cameraMatrix, distCoeffs)
                
                # Display image with a cube
                draw(img, imgpts, corners)
                cv2.waitKey(2)    
            

    cv2.destroyAllWindows()


# To generate calibration board and markers, uncomment these lines
# after that print both files
#marker_name = "marker.jpg"
#board_name = "board.jpg"
#generate_marker(marker_name)
#generate_board(board_name)


# To calibrate the camera you need show it calibration board
# This step should be done for every environment change
#camera_calibration_by_aruco_board('calibration.pckl')


# If the camera is calibrated, single marker (marker.jpg) can be shown
# To test that camera detects all the markers, 
# show the board with all of them (board.jpg)
detection_markers()