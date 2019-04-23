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


def connectLines(skelet, original, binar):
    # TODO: сделать вторую точку всегда ниже первой, чтобы цифры расставлялись норм
    # 	Ищем линии
    cv2.createTrackbar('th', 'main', 55, 255, nothing)
    cv2.createTrackbar('rho', 'main', 1, 255, nothing)
    cv2.createTrackbar('theta', 'main', 8, 255, nothing)
    cv2.createTrackbar('minlen', 'main', 29, 255, nothing)
    cv2.createTrackbar('maxgap', 'main', 80, 255, nothing)

    #while True:
    new_img = np.copy(original)

    th = cv2.getTrackbarPos('th', 'main')
    rho = cv2.getTrackbarPos('rho', 'main')
    theta = cv2.getTrackbarPos('theta', 'main')
    minlen = cv2.getTrackbarPos('minlen', 'main')
    maxgap = cv2.getTrackbarPos('maxgap', 'main')
    
    skeletLines = cv2.HoughLinesP(skelet,
                            threshold=th,
                            rho=rho,
                            theta=theta/360,
                            minLineLength=minlen,
                            maxLineGap=maxgap)

    skel = skelet
    cv2.subtract(binar, skelet, skel)

    cv2.imshow('skel', skel)
    #  КОСТЫЛЬ
    # отсортированный массив линий
    points = []
    min_index_y = 0
    using_index = set()
    for i in range(len(skeletLines)):
        if skeletLines[min_index_y][0][3] < skeletLines[i][0][3]:
            min_index_y = i
    using_index.add(min_index_y)
    points.append(skeletLines[min_index_y])

    min_index_y = 0
    max_y = 0
    for i in range(len(skeletLines)):
        if max_y < skeletLines[i][0][3] and i not in using_index:
            min_index_y = i
            max_y = skeletLines[i][0][3]
    using_index.add(min_index_y)
    minDist(points, skeletLines[min_index_y])

    # sorted(lines, key=[i][0][3])
    min_index_y = 3
    max_y = 0
    for i in range(len(skeletLines)):
        if max_y < skeletLines[i][0][3] and i not in using_index:
            min_index_y = i
            max_y = skeletLines[i][0][3]
    using_index.add(min_index_y)
    minDist(points, skeletLines[min_index_y])

    min_index_y = 2
    max_y = 0
    for i in range(len(skeletLines)):
        if max_y < skeletLines[i][0][3] and i not in using_index:
            min_index_y = i
            max_y = skeletLines[i][0][3]
    using_index.add(min_index_y)
    minDist(points, skeletLines[min_index_y])

    i = 0
    for line in points:
        for x1, y1, x2, y2 in line:
            cv2.putText(new_img, str(i), (x2, y2), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
            cv2.line(new_img, (x1, y1), (x2, y2), (0, 255, 0), 1)
            cv2.line(binar, (x1, y1), (x2, y2), 100, 1)
            i += 1

    cv2.imshow('binar', binar)
    cv2.imshow('new', new_img)
    cv2.waitKey(2)