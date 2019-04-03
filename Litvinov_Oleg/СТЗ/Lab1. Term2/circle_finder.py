import cv2
import numpy as np
import sys


def mean_circle(img, center, radius, i):
    mask = np.zeros((img.shape[0], img.shape[1], img.shape[2]), dtype=np.uint8)
    cv2.circle(mask, center, radius, color=(1, 1, 1), thickness=-1)

    hsv_img = np.zeros((img.shape[0], img.shape[1], img.shape[2]), dtype=np.uint8)

    cv2.cvtColor(img, code=cv2.COLOR_BGR2HSV, dst=hsv_img)
    hsv_img = hsv_img*mask
    cv2.imshow(str(i), hsv_img)
    print('shape', str(len(mask[:, :, 0].nonzero()[0])))

    return np.sum(hsv_img[:, :, 0]) / len(mask[:, :, 0].nonzero()[0])


cuzn = cv2.imread('CuZn.png')
ni = cv2.imread('Ni.jpg')
cv2.imshow('CuZn', cuzn)
cv2.imshow('Ni', ni)

money = cv2.imread('money.jpg')
cv2.imshow('money_BGR', money)
gray_money = cv2.cvtColor(money, cv2.COLOR_BGR2GRAY)
cv2.namedWindow('main')

gray_money = cv2.GaussianBlur(gray_money, (5, 5), 0)
gray_money = cv2.GaussianBlur(gray_money, (5, 5), 0)
cv2.imshow('money_gray', gray_money)

circle_image = money.copy()

dp = 1
minDist = 17
param1 = 255
param2 = 61
minR = 3
maxR = 0

can = cv2.Canny(gray_money, param1, param2)
cv2.imshow('Canny', can)

cv2.waitKey(3)
circles = cv2.HoughCircles(gray_money, cv2.HOUGH_GRADIENT,
                           dp=dp/10, minDist=minDist,
                           param1=param1, param2=param2,
                           minRadius=minR,
                           maxRadius=maxR)
circles = np.uint16(np.around(circles))

mean_cuzn = mean_circle(cuzn, (cuzn.shape[0]//2, cuzn.shape[1]//2), 20, 12)
mean_ni = mean_circle(ni, (ni.shape[0]//2, ni.shape[1]//2), 20, 21) - 10
print('cuzn: ', mean_cuzn, 'ni: ', mean_ni)

num = 0
for i in circles[0, :]:
    mean = mean_circle(money, (i[0], i[1]), i[2] - 5, num)
    print(num, mean)
    if abs(mean_ni - mean) < abs(mean_cuzn - mean):
        cv2.circle(money, (i[0], i[1]), i[2], (0, 255, 0), 2)
    else:
        cv2.circle(money, (i[0], i[1]), i[2], (0, 0, 255), 2)

    num += 1


cv2.imshow('money_with_circles', money)

cv2.waitKey(0)
cv2.destroyAllWindows()
