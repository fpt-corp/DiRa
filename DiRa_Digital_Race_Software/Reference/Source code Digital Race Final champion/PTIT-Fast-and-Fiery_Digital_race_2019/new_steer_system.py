import numpy as np


def get_point_3(img, flag, square):
    IMG_H, IMG_W = img.shape
    border = border = int((square - 1) / 2)
    turn_left = False
    turn_right = False
    left_count = 0
    right_count = 0
    leftmost = 0
    rightmost = 0
    leftmost_count = 0.0
    rightmost_count = 0.0
    x_left, y_left, x_right, y_right = 0, 0, 0, 0
    for i in range(int(IMG_H * 0.4), int(IMG_H * 0.75), border + 1):
        left = border
        while np.sum(img[i - border: i + border + 1,
                     left - border: left + border + 1]) / 255 != square ** 2 and left < IMG_W / 2 - 1 - border:
            left += 1
        if left < IMG_W / 2 - border:
            if left <= 10 and i < int(0.7 * IMG_H):
                leftmost += i
                leftmost_count += 1
            x_left += left
            y_left += i
            left_count += 1
        right = IMG_W - 1 - border
        while np.sum(img[i - border: i + border + 1,
                     right - border: right + border + 1]) / 255 != square ** 2 and right >= IMG_W / 2 + border:
            right -= 1
        if right >= IMG_W / 2 + border:
            if right >= 309 and i < int(0.7 * IMG_H):
                rightmost += i
                rightmost_count += 1
            x_right += right
            y_right += i
            right_count += 1
    if left_count == 0 and right_count == 0:
        return 160, 160
    if right_count == 0:
        x_right = x_left
        y_right = y_left
        right_count = left_count
    elif left_count == 0:
        x_left = x_right
        y_left = y_right
        left_count = right_count
    x_left, y_left = int(x_left / left_count), int(y_left / left_count)
    x_right, y_right = int(x_right / right_count), int(y_right / right_count)
    if rightmost_count / right_count >= 0.3:
        if (leftmost_count == 0) or (
                leftmost_count / left_count < rightmost_count / right_count and rightmost / rightmost_count < leftmost / leftmost_count) or (
                flag == 1):
            return int((3 * y_right + y_left) / 4), int((3 * x_right + x_left) / 4)
    elif leftmost_count / left_count >= 0.3:
        if (rightmost_count == 0) or (
                rightmost_count / right_count < leftmost_count / left_count and rightmost / rightmost_count > leftmost / leftmost_count) or (
                flag == -1):
            return int((y_right + 3 * y_left) / 4), int((x_right + 3 * x_left) / 4)
    return int((y_left + y_right) / 2), int((x_left + x_right) / 2)


def get_line(x1, y1, x2, y2):
    points = []
    issteep = abs(y2 - y1) > abs(x2 - x1)
    if issteep:
        x1, y1 = y1, x1
        x2, y2 = y2, x2
    rev = False
    if x1 > x2:
        x1, x2 = x2, x1
        y1, y2 = y2, y1
        rev = True
    deltax = x2 - x1
    deltay = abs(y2 - y1)
    error = int(deltax / 2)
    y = y1
    ystep = None
    if y1 < y2:
        ystep = 1
    else:
        ystep = -1
    for x in range(x1, x2 + 1):
        if issteep:
            if y < 0 or y > 319:
                break
            points.append((y, x))
        else:
            if x < 0 or x > 319:
                break
            points.append((x, y))
        error -= deltay
        if error < 0:
            y += ystep
            error += deltax
    # Reverse the list if the coordinates were reversed
    if rev:
        points.reverse()
    return points


def check_obstacle(label, x, y, square, mul):
    ratio = 0.8 * y / 160.0
    car_size = 90
    line_left = get_line(160 - car_size, 150, int(x - car_size * ratio), y + 5)
    line_right = get_line(160 + car_size, 150, int(x + car_size * ratio), y + 5)
    border = int((square - 1) / 2)
    correction = 0
    skip = 0
    for point in line_left:
        if skip != border + 1:
            skip += 1
            continue
        check = label[point[1] - border: point[1] + border + 1, point[0] - border: point[0] + border + 1]
        if np.sum(check) / 255 == 0:
            x_l = point[0]
            y_l = point[1]
            multiplier = mul
            length = 0
            if x < x_l:
                multiplier *= -1
            while label[y_l, x_l] == 0 and x_l < 319:
                x_l += 1
                length += 1
            if length >= 15:
                correction += multiplier * int(length * y_l / 159.0 * y / 159.0)
                break
        skip = 0
    for point in line_right:
        y_most = 0
        if skip != border + 1:
            skip += 1
            continue
        check = label[point[1] - border: point[1] + border + 1, point[0] - border: point[0] + border + 1]
        if np.sum(check) / 255 == 0:
            x_r = point[0]
            y_r = point[1]
            multiplier = mul
            length = 0
            if x > x_r:
                multiplier *= -1
            while label[y_r, x_r] == 0 and x_r > 0:
                x_r -= 1
                length += 1
            if length >= 15:
                correction += multiplier * int(-length * y_r / 159.0 * y / 159.0)
                break

        skip = 0
    print correction
    return correction


def get_cte(img, flag, square, mul):
    y, x = get_point_3(img, flag, square)
    x = max(0, min(x + check_obstacle(img, x, y, square, mul), 319))
    return x

