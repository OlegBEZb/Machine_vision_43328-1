import numpy as np

def check_pixel(area, iter=0):
    # Массив координат точек вокруг центральной
    pixel = [(0, 1), (0, 2), (1, 2), (2, 2),
             (2, 1), (2, 0), (1, 0), (0, 0)]

    # Если центральный пиксель черный
    if area[1, 1] == 0:
        return 0

    # cond1 = P2 + P3 + ... + P9
    cond1 = 0
    for i, j in pixel:
        cond1 += area[i, j]

    # cond2 = S(1)
    cond2 = 0
    if int(area[pixel[7]]) - int(area[pixel[0]]) > 0:
        cond2 += 1

    for i in range(1, len(pixel)):
        if int(area[pixel[i-1]]) - int(area[pixel[i]]) > 0:
            cond2 += 1
    if iter % 2 == 0:    # cond3 и 4 для четных итераций
        # cond3 = P2 * P4 * P6
        cond3 = int(area[pixel[0]]) * int(area[pixel[2]]) * int(area[pixel[4]])
        # cond3 = P4 * P6 * P8
        cond4 = int(area[pixel[2]]) * int(area[pixel[4]]) * int(area[pixel[6]])
    else:    # cond3 и 4 для нечетных итераций
        # alt_cond3 = P2 * P4 * P8
        cond3 = int(area[pixel[0]]) * int(area[pixel[2]]) * int(area[pixel[6]])
        # alt_cond4 = P2 * P6 * P8
        cond4 = int(area[pixel[0]]) * int(area[pixel[4]]) * int(area[pixel[6]])
    if 2 <= cond1 <= 6 and cond2 == 1 and cond3 == 0 and cond4 == 0:
        return 1
    else:
        return 0


def skeletonization(img):
    del_pixels = np.array(1)
    height, width = img.shape[0:2]
    # Скелетезация проходит в два шага слабо отличающихся друг от друга. Чтобы поменять
    # условие для удаления, мы считаем итерации, и для четных и нечетных выполняем слегка разные действия.
    iter = 0
    while np.sum(del_pixels) > 0:
        print('here')
        del_pixels = np.zeros(img.shape)
        for line in range(1, height - 1):
            for column in range(1, width - 1):
                area = img[line - 1: line + 2, column - 1: column + 2]
                del_pixels[line, column] = check_pixel(area, iter)

        # Все помеченные пиксели удаляются(приравниваются к 0/становятся черными
        for line in range(1, height - 1):
            for column in range(1, width - 1):
                if del_pixels[line][column] == 1:
                    img[line][column] = 0
        iter += 1

    return img