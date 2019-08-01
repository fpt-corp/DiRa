import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import matplotlib.patches as patches
import os

from torch.jit import save


def anno_label(img, label, isProcessedLabel=False):
    shape = img.shape
    h_img = shape[0]
    w_img = shape[1]

    h_cell = h_img // 13
    w_cell = w_img // 13

    if not isProcessedLabel:
        bboxs = label.split('\n')

        # f, ax1 = plt.subplots(1)
        # ax1.imshow(img)
        # ax1.set_title('Anno -> Label')
        # annotation -> label
        y_np = np.zeros((13, 13, 5))
        for bbox in bboxs:
            if len(bbox) > 1:
                bbox = bbox.split(',')
                bbox = list(int(x) for x in bbox)

                h = bbox[5] - bbox[1]
                w = bbox[4] - bbox[0]
                x = w // 2 + bbox[0]
                y = h // 2 + bbox[1]

                # rect = patches.Rectangle((bbox[0], bbox[1]), w, h, linewidth=2, edgecolor='r', facecolor='none')
                # ax1.add_patch(rect)

                # normalize
                w_c = x // w_cell
                h_c = y // h_cell
                x = (x % w_cell) / w_cell
                y = (y % h_cell) / h_cell
                w = w / w_img
                h = h / h_img

                y_np[h_c, w_c] = [x, y, w, h, 1]
        # plt.show()
        return y_np

    else:
        y_np = np.zeros((13, 13, 5))
        for bbox in label:
            bbox = list(int(x) for x in bbox)

            h = bbox[3] - bbox[1]
            w = bbox[2] - bbox[0]
            x = w // 2 + bbox[0]
            y = h // 2 + bbox[1]

            # rect = patches.Rectangle((bbox[0], bbox[1]), w, h, linewidth=2, edgecolor='r', facecolor='none')
            # ax1.add_patch(rect)

            # normalize
            w_c = x // w_cell
            h_c = y // h_cell
            x = (x % w_cell) / w_cell
            y = (y % h_cell) / h_cell
            w = w / w_img
            h = h / h_img

            y_np[h_c, w_c] = [x, y, w, h, 1]
        # plt.show()
        return y_np


def label_anno(img, y_np):
    shape = img.shape
    h_img = shape[0]
    w_img = shape[1]

    h_cell = h_img // 13
    w_cell = w_img // 13

    # label -> annotation
    # f, ax2 = plt.subplots(1)
    # ax2.imshow(img)
    # ax2.set_title('Label -> Anno')

    y_np_nonzero = y_np[:, :, 4].nonzero()
    # print(y_np_nonzero)
    result = []
    for i in range(len(y_np_nonzero[0])):
        h_c, w_c = y_np_nonzero[0][i], y_np_nonzero[1][i]
        x, y, w, h, p = y_np[h_c][w_c]
        p = 2 - p if p >= 1 else p
        w *= w_img
        h *= h_img
        x = x * w_cell + w_cell * w_c
        y = y * h_cell + h_cell * h_c

        # print(x, y, w, h, h_c, w_c)

        if p > 0.7:
            # print(p)
            # rect = patches.Rectangle((x - w // 2, y - h // 2), w, h, linewidth=2, edgecolor='g', facecolor='none')
            # ax2.add_patch(rect)
            # plt.text(x - w // 2, y - h // 2, '%.2f' % p, color='r')
            result.append([x - w // 2, y - h // 2, x + w // 2, y + h // 2, p])

    # plt.show()
    return result


if __name__ == '__main__':
    imgPath = './train_image/'
    labelPath = './label_train/'

    savePath = './saved_label_train/'

    # imgIter = iter(os.listdir(imgPath))
    # labelIter = iter(os.listdir(labelPath))
    #
    # err=0
    #
    # for _ in range(len(os.listdir(labelPath))):
    #     try:
    #         labelFile = labelIter.__next__()
    #         # while True:
    #         imgFile = imgIter.__next__()
    #
    #         # if labelFile.split('.')[0] == imgFile.split('.')[0]:
    #
    #         img = mpimg.imread(imgPath + imgFile)
    #         label = open(labelPath + labelFile).read()
    #
    #         y_np = anno_label(img, label)
    #
    #         np.save(savePath + labelFile.split('.')[0], y_np)
    #
    #     except:
    #         print('ERR ', labelFile)
    #         # input(...)
    #         err+=1
    #
    # print("DONE!!!... Number of error ", err)

    # imgPath = './train_image/'
    # labelPath = './label_train/'
    #
    # img = mpimg.imread(imgPath + 'test (10463).JPG')
    # label = open(labelPath + 'test (10463).txt').read()
    # anno_label(img, label)
