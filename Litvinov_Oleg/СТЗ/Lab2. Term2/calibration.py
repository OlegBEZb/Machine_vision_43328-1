<<<<<<< HEAD
import cv2.aruco as aruco
import cv2
import pickle

def camera_calibration_by_aruco_board(
            calibration_params_filename='calibration.pckl'):
    """
    Function to calibrate camera via 5*7 marker board. Should be run before
    detecting markers.
    
    Saves internal calibration parameters and distortion coefficients into 
    specified file.
    
    Parameters
    ----------
    calibration_params_filename : str
        name of the file where to store internal calibration parameters and 
        distortion coefficients 

    Returns
    -------
    None
    """
    aruco_dict = aruco.Dictionary_get(aruco.DICT_5X5_1000)

    # Creating a theoretical board we'll use to calculate marker positions
    board = aruco.GridBoard_create(
        markersX=5,
        markersY=7,
        markerLength=0.04,
        markerSeparation=0.01,
        dictionary=aruco_dict)

    # Read an image or a video to calibrate your camera
    # I'm using a video and waiting until my entire gridboard is seen before
    # calibrating. The following code assumes you have a 5X7 Aruco gridboard
    # to calibrate with
    cam = cv2.VideoCapture(0)

    while (cam.isOpened()):
        # Capturing each frame of our video stream
        ret, img = cam.read()
        cv2.imshow('camera calibration', img)
        # retval is non-zero in the case of successful frame reading
        if ret == True:
            # grayscale image
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            parameters = aruco.DetectorParameters_create()

            # Detect Aruco markers
            corners, ids, rejectedImgPoints = aruco.detectMarkers(
                    gray, aruco_dict, parameters=parameters)

            # Make sure markers were detected before continuing
            if (ids is not None and 
                corners is not None and 
                len(ids) > 0 and 
                len(corners) > 0 and 
                len(corners) == len(ids)):
                # The next if makes sure we see all matrixes in our gridboard
                if len(ids) == len(board.ids):
                    # Calibrate the camera now using cv2 method
                    ret, cameraMatrix, distCoeffs, _, _ = cv2.calibrateCamera(
                        objectPoints=board.objPoints,
                        imagePoints=corners,
                        imageSize=gray.shape,
                        cameraMatrix=None,
                        distCoeffs=None)

                    # Print matrix and distortion coefficient to the console
                    print('Intrinsic  calibration coefficients\n', cameraMatrix)
                    print('Distortion (extrinsic) coefficients\n', distCoeffs)

                    # Output values to be used where matrix+dist is required
                    f = open(calibration_params_filename, 'wb')
                    pickle.dump((cameraMatrix, distCoeffs), f)
                    f.close()

                    # PRint to console our success
                    print('\nCalibration successful!')

                    break

        # Exit at the end of the video on the EOF key
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

=======
import cv2.aruco as aruco
import cv2
import pickle

def camera_calibration_by_aruco_board(
            calibration_params_filename='calibration.pckl'):
    """
    Function to calibrate camera via 5*7 marker board. Should be run before
    detecting markers.
    
    Saves internal calibration parameters and distortion coefficients into 
    specified file.
    
    Parameters
    ----------
    calibration_params_filename : str
        name of the file where to store internal calibration parameters and 
        distortion coefficients 

    Returns
    -------
    None
    """
    aruco_dict = aruco.Dictionary_get(aruco.DICT_5X5_1000)

    # Creating a theoretical board we'll use to calculate marker positions
    board = aruco.GridBoard_create(
        markersX=5,
        markersY=7,
        markerLength=0.04,
        markerSeparation=0.01,
        dictionary=aruco_dict)

    # Read an image or a video to calibrate your camera
    # I'm using a video and waiting until my entire gridboard is seen before
    # calibrating. The following code assumes you have a 5X7 Aruco gridboard
    # to calibrate with
    cam = cv2.VideoCapture(0)

    while (cam.isOpened()):
        # Capturing each frame of our video stream
        ret, img = cam.read()
        cv2.imshow('camera calibration', img)
        # retval is non-zero in the case of successful frame reading
        if ret == True:
            # grayscale image
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            parameters = aruco.DetectorParameters_create()

            # Detect Aruco markers
            corners, ids, rejectedImgPoints = aruco.detectMarkers(
                    gray, aruco_dict, parameters=parameters)

            # Make sure markers were detected before continuing
            if (ids is not None and 
                corners is not None and 
                len(ids) > 0 and 
                len(corners) > 0 and 
                len(corners) == len(ids)):
                # The next if makes sure we see all matrixes in our gridboard
                if len(ids) == len(board.ids):
                    # Calibrate the camera now using cv2 method
                    ret, cameraMatrix, distCoeffs, _, _ = cv2.calibrateCamera(
                        objectPoints=board.objPoints,
                        imagePoints=corners,
                        imageSize=gray.shape,
                        cameraMatrix=None,
                        distCoeffs=None)

                    # Print matrix and distortion coefficient to the console
                    print('Intrinsic  calibration coefficients\n', cameraMatrix)
                    print('Distortion (extrinsic) coefficients\n', distCoeffs)

                    # Output values to be used where matrix+dist is required
                    f = open(calibration_params_filename, 'wb')
                    pickle.dump((cameraMatrix, distCoeffs), f)
                    f.close()

                    # PRint to console our success
                    print('\nCalibration successful!')

                    break

        # Exit at the end of the video on the EOF key
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

>>>>>>> 42a659055a6d57bad3c490d3d672ed9f2327a43b
    cv2.destroyAllWindows()