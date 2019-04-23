import cv2
import numpy as np
from scipy.spatial import distance as dist
from sklearn.metrics.pairwise import cosine_similarity

def mean_circle(img, center, radius, i):
    # create mask to deal only with this circle
    mask = np.zeros((img.shape[0], img.shape[1], img.shape[2]), dtype=np.uint8)
    cv2.circle(mask, center, radius, color=(1, 1, 1), thickness=-1)

    # convert image to hsv and multiply by mask
    hsv_img = np.zeros((img.shape[0], img.shape[1], img.shape[2]), dtype=np.uint8)
    cv2.cvtColor(img, code=cv2.COLOR_BGR2HSV, dst=hsv_img)
    hsv_img = hsv_img * mask
    cv2.imshow('hsv mask_' + str(i), hsv_img)
    
    fig_area = len(mask[:, :, 0].nonzero()[0])
    #print('area ' + str(i) + ' in pixels: ', fig_area)
    hue_pixels_sum = np.sum(hsv_img[:, :, 1])
    #sat_pixels_sum = np.sum(hsv_img[:, :, 1])
    
    mean_hue = hue_pixels_sum / fig_area
    #mean_sat = sat_pixels_sum / fig_area
    
    return mean_hue#, mean_sat

# load samples for comparison
cuzn = cv2.imread('CuZn.png')
ni = cv2.imread('Ni.jpg')
cv2.imshow('CuZn', cuzn)
cv2.imshow('Ni', ni)

# read original image with money
money = cv2.imread('money.jpg')
#cv2.imshow('money_BGR', money)
gray_money = cv2.cvtColor(money, cv2.COLOR_BGR2GRAY)
#cv2.namedWindow('main')

gray_money = cv2.GaussianBlur(gray_money, (5, 5), 0)
gray_money = cv2.GaussianBlur(gray_money, (5, 5), 0)
#cv2.imshow('money_gray', gray_money)

circle_image = money.copy()

dp = 1
minDist = 17
param1 = 255
param2 = 61
minR = 3
maxR = 0

# find money borders
can = cv2.Canny(gray_money, param1, param2)
cv2.imshow('Canny', can)

cv2.waitKey(0)

#  Each vector is encoded as a 3-element floating-point vector (x,y,radius)
circles = cv2.HoughCircles(gray_money, cv2.HOUGH_GRADIENT,
                           dp=dp/10, minDist=minDist,
                           param1=param1, param2=param2,
                           minRadius=minR,
                           maxRadius=maxR)
print('circles shape: ', circles.shape, '\n')
circles = np.uint16(np.around(circles))

mean_cuzn = mean_circle(cuzn, (cuzn.shape[0]//2, cuzn.shape[1]//2), cuzn.shape[0]//2, 'CuZn') + 100
mean_ni = mean_circle(ni, (ni.shape[0]//2, ni.shape[1]//2), ni.shape[0]//2, 'Ni')
print('mean_ni', mean_ni)
print('mean_cuzn', mean_cuzn)

for i, circle in enumerate(circles[0, :]):
    mean = mean_circle(money, (circle[0], circle[1]), circle[2], i)
    print('mean_' + str(i), mean)

    if abs(mean_cuzn - mean) < abs(mean_ni - mean):
        cv2.circle(money, (circle[0], circle[1]), circle[2], (0, 255, 0), 2)
    else:
        cv2.circle(money, (circle[0], circle[1]), circle[2], (0, 0, 255), 2)


cv2.imshow('money_with_circles', money)

cv2.waitKey(0)
cv2.destroyAllWindows()
