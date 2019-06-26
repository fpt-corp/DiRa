import numpy as np
import cv2

IMG_W = 320
IMG_H = 160

def preprocess_img(img, ratio=2):
    h, w = img.shape
    img = img[2*h//5:, :]
    h = 3*h//5
    img = cv2.resize(img, (w//ratio, h//ratio), cv2.INTER_NEAREST)
    return img

def longest_array(a, padding=1):
    start, end, length = 0, 0, 0

    i = 0
    j = 0
    window = 0

    while (j < len(a) - 1):
        if(a[i] == 0):
            i += 1
            j += 1
            continue

        j += 1
        if a[j] >= a[i] - padding:
            if j - i > length:
                length = j - i
                start = i
                end = j
        else:
            i += 1
            j = i

    return start, end, length

def find_object(img, padding = 1):
    h, w = img.shape
    res = []
    for i in range(w):
        res.append(longest_array(img[:, i], padding))

    filtered = np.zeros((h, w), dtype=np.uint8)
    for r in range(len(res)):
        for j in range(res[r][0], res[r][1]+1):
            filtered[j, r] = 1

    return filtered


def remove_road(img, road_distance=17, padding=3, far_distance=60):
    h, w = img.shape
    i = 1
    while i < h:
        max_pix = -1
        for j in range(w):
            if img[-i, j] > far_distance or img[-i, j] == 0:
                img[-i, j] = road_distance
                continue
            if img[-i, j] <= road_distance + padding and img[-i,j] >= road_distance - padding:
                if max_pix < img[-i, j]:
                    max_pix = img[-i, j]
                img[-i, j] = road_distance
        if max_pix > road_distance:
            road_distance = max_pix
        i += 1

    return img

def remove_noise(img, k):
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT,(k,k))
    # img = cv2.morphologyEx(img, cv2.MORPH_CLOSE, kernel)
    img = cv2.morphologyEx(img, cv2.MORPH_OPEN, kernel)
    img = cv2.morphologyEx(img, cv2.MORPH_CLOSE, kernel)
    return img

def find_obstacles(img):
    rects = []
    # _,img = cv2.threshold(img,1,255,0)
    _, contours, _ = cv2.findContours(img, cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
    for cnt in contours:
        if cv2.contourArea(cnt) > 80:
            rects.append(cv2.boundingRect(cnt))
        # print(x, y, w, h)
        # print(x, y, w, h)
    return rects

def get_restriction(rects):
    x_left = IMG_W // 2
    x_right = IMG_W // 2
    extra = 20
    for rect in rects:
        x, y, w, h = rect
        # print(rect)
        x = x * 4
        w = w * 4
        # cv2.rectangle(image, (x, 0), (x+w, IMG_H), (0, 255, 0), 1)

        if x > IMG_W // 2:
            if x > x_right:
                x_right = x

        if (x + w) < IMG_W // 2:
            if (x + w) < x_left:
                x_left = x + w

        if x < IMG_W // 2 and x + w > IMG_W / 2:
            if IMG_W / 2 - x > x + w - IMG_W / 2:
                x_left = x + w
                x_right = IMG_W - 1
            else:
                x_left = 0
                x_right = x

    if x_right != IMG_W // 2 and x_right <= 0.8 * IMG_W:
        i_right = x_right
    else:
        i_right = IMG_W - 1

    if x_left != IMG_W // 2 and x_right >= 0.2 * IMG_W:
        i_left = x_left
    else:
        i_left = 0
    return i_left, i_right
