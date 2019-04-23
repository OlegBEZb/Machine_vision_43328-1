<<<<<<< HEAD
import numpy as np
import cv2

from module import plot_grid, proj

focal_x = 600
focal_y = 600
from_cam_to_laser_dist = -0.25

grid_height = 600
grid_width = 600

# normal to the light plane in camera coordinate system
lp_normal = np.zeros((3,1))
lp_normal[0, 0] = 0
lp_normal[1, 0] = 1
lp_normal[2, 0] = 0

# Создание ползунков
def nothing(x):
    pass

cv2.namedWindow('track')
cv2.createTrackbar('hmin', 'track', 66, 255, nothing)
cv2.createTrackbar('hmax', 'track', 91, 255, nothing)
cv2.createTrackbar('smin', 'track', 38, 255, nothing)
cv2.createTrackbar('smax', 'track', 82, 255, nothing)
cv2.createTrackbar('vmin', 'track', 95, 255, nothing)
cv2.createTrackbar('vmax', 'track', 255, 255, nothing)


vcap = cv2.VideoCapture('1.avi')

while(vcap.isOpened()):

    # capturing frame from video
    ret, frame = vcap.read()
    cv2.imshow('original videoframe', frame)
    
    frame_width = int(vcap.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(vcap.get(cv2.CAP_PROP_FRAME_HEIGHT))

    hmin = cv2.getTrackbarPos('hmin', 'track')
    hmax = cv2.getTrackbarPos('hmax', 'track')
    smin = cv2.getTrackbarPos('smin', 'track')
    smax = cv2.getTrackbarPos('smax', 'track')
    vmin = cv2.getTrackbarPos('vmin', 'track')
    vmax = cv2.getTrackbarPos('vmax', 'track')
    
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    #hsv = cv2.blur(hsv,(5,5))
    binary_frame = cv2.inRange(hsv, np.array([hmin, smin, vmin]), 
                               np.array([hmax, smax, vmax]))
    cv2.imshow('frame after hsv inRange', binary_frame)

    # 2-D depth map
    cmap_depth = np.zeros((grid_height, grid_width, 3), np.uint8)
    
    # projection of highlighted pixel on the image plane in camera coordinate system
    pix_camera_proj_cs = np.zeros((2, 1))
    
    for x in range(0, frame_width):
        for y in range(0, frame_height):
            if binary_frame[y, x] == 255:
                pix_camera_proj_cs[0, 0] = x
                pix_camera_proj_cs[1, 0] = y
                pix_camera_cs = proj(pix_camera_proj_cs, frame_width/2, 
                                     frame_height/2, focal_x, focal_y, 
                                     from_cam_to_laser_dist, lp_normal)
                pix_camera_cs *= 200 # *= 100 из метров в сантиметры
                pix_camera_cs[0, 0] += frame_width/2
                cv2.circle(cmap_depth, 
                           (int(pix_camera_cs[0, 0]), int(pix_camera_cs[2, 0])), 
                           1, [0, 255, 0], 1)
    
    plot_grid(cmap_depth, grid_height, grid_width, step=20)
    cv2.imshow('map_depth', cmap_depth)


    cv2.waitKey(1)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
=======
import numpy as np
import cv2

from module import plot_grid, proj

focal_x = 600
focal_y = 600
from_cam_to_laser_dist = -0.25

grid_height = 600
grid_width = 600

# normal to the light plane in camera coordinate system
lp_normal = np.zeros((3,1))
lp_normal[0, 0] = 0
lp_normal[1, 0] = 1
lp_normal[2, 0] = 0

# Создание ползунков
def nothing(x):
    pass

cv2.namedWindow('track')
cv2.createTrackbar('hmin', 'track', 66, 255, nothing)
cv2.createTrackbar('hmax', 'track', 91, 255, nothing)
cv2.createTrackbar('smin', 'track', 38, 255, nothing)
cv2.createTrackbar('smax', 'track', 82, 255, nothing)
cv2.createTrackbar('vmin', 'track', 95, 255, nothing)
cv2.createTrackbar('vmax', 'track', 255, 255, nothing)


vcap = cv2.VideoCapture('1.avi')

while(vcap.isOpened()):

    # capturing frame from video
    ret, frame = vcap.read()
    cv2.imshow('original videoframe', frame)
    
    frame_width = int(vcap.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(vcap.get(cv2.CAP_PROP_FRAME_HEIGHT))

    hmin = cv2.getTrackbarPos('hmin', 'track')
    hmax = cv2.getTrackbarPos('hmax', 'track')
    smin = cv2.getTrackbarPos('smin', 'track')
    smax = cv2.getTrackbarPos('smax', 'track')
    vmin = cv2.getTrackbarPos('vmin', 'track')
    vmax = cv2.getTrackbarPos('vmax', 'track')
    
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    #hsv = cv2.blur(hsv,(5,5))
    binary_frame = cv2.inRange(hsv, np.array([hmin, smin, vmin]), 
                               np.array([hmax, smax, vmax]))
    cv2.imshow('frame after hsv inRange', binary_frame)

    # 2-D depth map
    cmap_depth = np.zeros((grid_height, grid_width, 3), np.uint8)
    
    # projection of highlighted pixel on the image plane in camera coordinate system
    pix_camera_proj_cs = np.zeros((2, 1))
    
    for x in range(0, frame_width):
        for y in range(0, frame_height):
            if binary_frame[y, x] == 255:
                pix_camera_proj_cs[0, 0] = x
                pix_camera_proj_cs[1, 0] = y
                pix_camera_cs = proj(pix_camera_proj_cs, frame_width/2, 
                                     frame_height/2, focal_x, focal_y, 
                                     from_cam_to_laser_dist, lp_normal)
                pix_camera_cs *= 200 # *= 100 из метров в сантиметры
                pix_camera_cs[0, 0] += frame_width/2
                cv2.circle(cmap_depth, 
                           (int(pix_camera_cs[0, 0]), int(pix_camera_cs[2, 0])), 
                           1, [0, 255, 0], 1)
    
    plot_grid(cmap_depth, grid_height, grid_width, step=20)
    cv2.imshow('map_depth', cmap_depth)


    cv2.waitKey(1)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
>>>>>>> 42a659055a6d57bad3c490d3d672ed9f2327a43b
