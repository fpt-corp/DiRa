import matplotlib.patches as patches
import matplotlib.pyplot as plt


def cal_IOU(bb1, bb2):
    i_x1 = max(bb1[0], bb2[0])
    i_y1 = max(bb1[1], bb2[1])
    i_x2 = min(bb1[2], bb2[2])
    i_y2 = min(bb1[3], bb2[3])

    i_h = i_y2 - i_y1
    i_w = i_x2 - i_x1

    if i_h > 0 and i_w > 0:
        i = i_h * i_w
        u = (bb1[2] - bb1[0]) * (bb1[3] - bb1[1]) + (bb2[2] - bb2[0]) * (bb2[3] - bb2[1]) - i
        return i / u
    else:
        return 0


def take_probability(elem):
    return elem[4]


# list_bbox_p = [[x1, y1, x2, y2, p],[...]]
def nonmax_supression(list_bbox_p, threshold=0.1):
    list_bbox_p.sort(key=take_probability, reverse=True)
    # print(list_bbox_p)
    result = []
    while len(list_bbox_p) > 0:
        temp = list_bbox_p[0]
        list_bbox_p.remove(list_bbox_p[0])
        # print('pop', temp)
        for bb in list_bbox_p:
            iou = cal_IOU(temp[:-1], bb[:-1])
            # print(temp, bb, iou)
            if iou > threshold:
                list_bbox_p.remove(bb)
                # print('removed', bb)
        result.append(temp)
    return result


if __name__ == '__main__':
    img = plt.imread('image.png')
    h, w = img.shape[:-1]
    bboxs = [[10, 10, 500, 500, 0.4], [100, 100, 400, 400, 0.7], [200, 200, 600, 600, 0.6]
        , [30, 30, 550, 550, 0.61], [90, 90, 390, 390, 0.62]]

    f, ax = plt.subplots(1)
    plt.imshow(img)
    for i in range(len(bboxs)):
        rect = patches.Rectangle((bboxs[i][0], bboxs[i][1]), bboxs[i][2] - bboxs[i][0], bboxs[i][3] - bboxs[i][1],
                                 linewidth=2, edgecolor='r', facecolor='none')
        ax.add_patch(rect)
        plt.text(bboxs[i][0], bboxs[i][1], bboxs[i][4])

    supressed_bboxs = nonmax_supression(bboxs)
    print(supressed_bboxs)
    for i in range(len(supressed_bboxs)):
        rect = patches.Rectangle((supressed_bboxs[i][0], supressed_bboxs[i][1]),
                                 supressed_bboxs[i][2] - supressed_bboxs[i][0],
                                 supressed_bboxs[i][3] - supressed_bboxs[i][1],
                                 linewidth=4, edgecolor='g', facecolor='none')
        ax.add_patch(rect)
    plt.show()
