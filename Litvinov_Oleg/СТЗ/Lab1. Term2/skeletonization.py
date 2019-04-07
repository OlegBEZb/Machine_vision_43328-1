import numpy as np

def check_pixel(area, iter=0):
    """
    Returns
    ----------
    int
        1 if the pixel must be removed, 0 - otherwise
    """
    # coordinates of points around the central point
    pixel = [(0, 1), (0, 2), (1, 2), (2, 2), (2, 1), (2, 0), (1, 0), (0, 0)]

    # If central pixel is black, we don't need to reset or skip it
    if area[1, 1] == 0:
        return 0

    # cond1 = P2 + P3 + ... + P9
    # The number of white (1) pixels among the eight neighbors around central pixel
    cond1 = 0
    for i, j in pixel:
        cond1 += area[i, j]

    # cond2 = S(1)
    # The number of one-to-zero (1 -> 0 or black-to-white) transitions 
    # in the sequence P2 -> P3 -> ... -> P9 -> P2
    cond2 = 0
    for i in range(len(pixel)):
        if int(area[pixel[i]]) - int(area[pixel[(i+1) % len(pixel)]]) > 0:
            cond2 += 1
            
    if iter % 2 == 0:    # cond3 и 4 for the first step of algorithm
        # cond3 = P2 * P4 * P6
        cond3 = int(area[pixel[0]]) * int(area[pixel[2]]) * int(area[pixel[4]])
        # cond3 = P4 * P6 * P8
        cond4 = int(area[pixel[2]]) * int(area[pixel[4]]) * int(area[pixel[6]])
    else:    # cond3 и 4 for the second step of algorithm
        # alt_cond3 = P2 * P4 * P8
        cond3 = int(area[pixel[0]]) * int(area[pixel[2]]) * int(area[pixel[6]])
        # alt_cond4 = P2 * P6 * P8
        cond4 = int(area[pixel[0]]) * int(area[pixel[4]]) * int(area[pixel[6]])
    
    if 2 <= cond1 <= 6 and cond2 == 1 and cond3 == 0 and cond4 == 0:
        return 1
    else:
        return 0


def skeletonization(img):
    """
    Parameters
    ----------
    img : numpy.ndarray
        binary image where 1 stands for white and 0 stands for black pixels    
    """
    
    height, width = img.shape[0:2]
    # Скелетезация проходит в два шага слабо отличающихся друг от друга. 
    # Чтобы поменять условие для удаления, мы считаем итерации, 
    # и для четных и нечетных выполняем слегка разные действия.
    iter = 0
    del_pixels = np.array(1)
    while np.sum(del_pixels) > 0:
        if(iter % 10 == 0):
            print('skeletization iteration: ', iter)
        del_pixels = np.zeros(img.shape)
        for line in range(1, height - 1):
            for column in range(1, width - 1):
                area = img[line-1:line+2, column-1:column+2]
                del_pixels[line, column] = check_pixel(area, iter)

        # Все помеченные пиксели удаляются(приравниваются к 0/становятся черными
        img[del_pixels == 1] = 0
        iter += 1

    return img