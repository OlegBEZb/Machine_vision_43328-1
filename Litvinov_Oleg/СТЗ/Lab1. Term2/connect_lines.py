import cv2
import numpy as np

def nothing(x):
    pass

def minDist(lines, line):
    dst1 = (lines[-1][0, 0] - line[0, 0]) ** 2 + (
            lines[-1][0, 1] - line[0, 1]) ** 2  # p11 p21
    dst2 = (lines[-1][0, 0] - line[0, 2]) ** 2 + (
            lines[-1][0, 1] - line[0, 3]) ** 2  # p11 p22
    dst3 = (lines[-1][0, 2] - line[0, 0]) ** 2 + (
            lines[-1][0, 3] - line[0, 1]) ** 2  # p12 p21
    dst4 = (lines[-1][0, 2] - line[0, 2]) ** 2 + (
            lines[-1][0, 3] - line[0, 3]) ** 2  # p12 p22

    if dst1 < dst2 and dst1 < dst3 and dst1 < dst4:
        lines.append([[lines[-1][0, 0], lines[-1][0, 1], line[0, 0], line[0, 1]]])
        lines.append(line)
        return

    if dst2 < dst3 and dst2 < dst4:
        lines.append([[lines[-1][0, 0], lines[-1][0, 1], line[0, 2], line[0, 3]]])
        lines.append(line)
        return

    if dst3 < dst4:
        lines.append([[lines[-1][0, 2], lines[-1][0, 3], line[0, 0], line[0, 1]]])
        lines.append(line)
        return

    lines.append([[lines[-1][0, 2], lines[-1][0, 3], line[0, 2], line[0, 3]]])
    lines.append(line)
    return


def connectLines(img_skeletized, original):
    # TODO: сделать вторую точку всегда ниже первой, чтобы цифры расставлялись норм
    
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
    skel = img_skeletized.copy()
    lines = cv2.HoughLinesP(image=img_skeletized, threshold=th, rho=rho, 
                                  theta=theta/360, minLineLength=minlen, 
                                  maxLineGap=maxgap)

    # convert lines into python list format
    lines = lines.tolist()
    # get rid of [[]]
    for i, double_brackets_list in enumerate(lines):
        lines[i] = [item for sublist in double_brackets_list for item in sublist]
        
    print('fresh handled lines', lines)
    
    lines.sort(key = lambda lines: max(lines[1], lines[3]), reverse=True) 
    
    print('\n sorted external', lines)


    for i, line in enumerate(lines):
        if ( (i == 0) and (line[1] < line[3]) ) or \
           ((line[0] - lines[i-1][2]) ** 2 + (line[1] - lines[i-1][3]) ** 2 > \
            (line[2] - lines[i-1][2]) ** 2 + (line[3] - lines[i-1][3]) ** 2):
            # swap points
            line[0], line[2] = line[2], line[0]
            line[1], line[3] = line[3], line[1]
        else:
            continue

    print('\n sorted internal', lines)

    
    for i, line in enumerate(lines):
            cv2.line(original_with_path, (line[0], line[1]), (line[2], line[3]), (0, 255, 0), 1)
            cv2.putText(original_with_path, str(i), (line[0], line[1] - 20), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
            if (i != 0):
                cv2.line(original_with_path, (line[0], line[1]), (lines[i-1][2], lines[i-1][3]), (0, 255, 0), 1)    
            
    return original_with_path