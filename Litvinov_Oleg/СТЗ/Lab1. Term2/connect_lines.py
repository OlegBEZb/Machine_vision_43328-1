import cv2
import numpy as np


def nothing(x):
    pass


def connectLines(img_skeletized, original):
   
    # 	Ищем линии
    cv2.createTrackbar('th', 'main', 55, 255, nothing)
    cv2.createTrackbar('rho', 'main', 1, 255, nothing)
    cv2.createTrackbar('theta', 'main', 8, 255, nothing)
    cv2.createTrackbar('minlen', 'main', 29, 255, nothing)
    cv2.createTrackbar('maxgap', 'main', 80, 255, nothing)

    #while True:
    original_with_path = np.copy(original)

    th = cv2.getTrackbarPos('th', 'main')
    rho = cv2.getTrackbarPos('rho', 'main')
    theta = cv2.getTrackbarPos('theta', 'main')
    minlen = cv2.getTrackbarPos('minlen', 'main')
    maxgap = cv2.getTrackbarPos('maxgap', 'main')
    
    # Find line segments in a binary image using the probabilistic Hough transform.
    # Each line is represented by a 4-element vector  (x_1, y_1, x_2, y_2) , 
    # where  (x_1,y_1) and  (x_2, y_2) are the ending points of each detected 
    # line segment.
    lines = cv2.HoughLinesP(image=img_skeletized, threshold=th, rho=rho, 
                                  theta=theta/360, minLineLength=minlen, 
                                  maxLineGap=maxgap)

    if lines is None:
        print('no lines found')
        return original

    # convert lines into python list format
    lines = lines.tolist()
    # get rid of [[]]
    for i, double_brackets_list in enumerate(lines):
        lines[i] = [item for sublist in double_brackets_list for item in sublist]
        
    #print('fresh handled lines', lines)
    
    # the main logic of providing the way: lines are sorted by their
    # lowest point. After that first line should be oriented from bottom to top
    # all the others should be oriented from the closest end to the furthest 
    # in relation to the top of the previous line
    lines.sort(key = lambda lines: max(lines[1], lines[3]), reverse=True) 
    #print('\n sorted external', lines)
    for i, line in enumerate(lines):
        if ( (i == 0) and (line[1] < line[3]) ) or \
           ( (line[0] - lines[i-1][2]) ** 2 + (line[1] - lines[i-1][3]) ** 2 > \
             (line[2] - lines[i-1][2]) ** 2 + (line[3] - lines[i-1][3]) ** 2 ):
            # swap points
            line[0], line[2] = line[2], line[0]
            line[1], line[3] = line[3], line[1]
        else:
            continue

    #print('\n sorted internal', lines)

    # showing sorted lines, connecting them with each other and printing line 
    # number near the beginning of each line
    for i, line in enumerate(lines):
            cv2.line(original_with_path, (line[0], line[1]),
                     (line[2], line[3]), (0, 255, 0), 1)
            cv2.putText(original_with_path, str(i), (line[0], line[1] - 20), 
                        cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
            if (i != 0):
                cv2.line(original_with_path, (line[0], line[1]), 
                         (lines[i-1][2], lines[i-1][3]), (0, 255, 0), 1)    
            
    return original_with_path