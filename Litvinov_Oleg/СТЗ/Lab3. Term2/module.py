import cv2
import numpy as np

def plot_grid(cmap_depth, grid_height, grid_width, step=20):
    """
    This finction plots a grid on a cmap_depth 
    """
    for x in range(0, grid_width, 20):
        cv2.line(cmap_depth, (0, x), (grid_height, x), [255, 255, 255], 1)

    for y in range(0, grid_height, 20):
        cv2.line(cmap_depth, (y, 0), (y, grid_width), [255, 255, 255], 1)
        

def proj(pix, cx, cy, focal_x, focal_y, from_cam_to_laser_dist, lp_normal):
    """
    This function calcules positions in camera coordinate system of highlighted
    points from projection the image plane in camera coordinate system
    """
    # highlighted pixel in camera coordinate system
    pm = np.zeros((3, 1))
    pm[0, 0] = float((pix[0] - cx) / focal_x)
    pm[1, 0] = float((pix[1] - cy) / focal_y)
    pm[2, 0] = float(0.4)

    denominator = float((pm[0, 0]**2 + pm[1, 0]**2 + pm[2, 0]**2)**0.5)

    # unit vector of each axis
    unit_vectors = np.zeros((3,1))
    unit_vectors[0, 0] = float(pm[0, 0] / denominator)
    unit_vectors[1, 0] = float(pm[1, 0] / denominator)
    unit_vectors[2, 0] = float(pm[2, 0] / denominator)

    # k in the manual
    scale_coef = float(-from_cam_to_laser_dist / (lp_normal[1, 0] * unit_vectors[1, 0]))

    p = np.zeros((3, 1))
    p[0, 0] = float(scale_coef * unit_vectors[0, 0])
    p[1, 0] = float(scale_coef * unit_vectors[1, 0])
    p[2, 0] = float(scale_coef * unit_vectors[2, 0])

    return p
