import os

import PIL.Image as Image
import matplotlib.pyplot as plt
import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.utils.data

# from scipy import ndimage
from labeling import anno_label

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
print("----------", device, "---------------")


class TinyYoloNet(nn.Module):
    def __init__(self):
        super(TinyYoloNet, self).__init__()
        self.maxpool1 = nn.MaxPool2d(2, stride=2)
        self.maxpool2 = nn.MaxPool2d(2, stride=1)

        self.conv1 = nn.Conv2d(3, 16, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(16, 32, kernel_size=3, padding=1)
        self.conv3 = nn.Conv2d(32, 64, kernel_size=3, padding=1)
        self.conv4 = nn.Conv2d(64, 128, kernel_size=3, padding=1)
        self.conv5 = nn.Conv2d(128, 256, kernel_size=3, padding=1)
        self.conv6 = nn.Conv2d(256, 512, kernel_size=3, padding=1)
        self.conv7 = nn.Conv2d(512, 1024, kernel_size=3, padding=1)
        self.conv8 = nn.Conv2d(1024, 1024, kernel_size=3, padding=1)
        self.conv9 = nn.Conv2d(1024, 5, kernel_size=1)

    def forward(self, x):
        x = self.maxpool1(F.relu(self.conv1(x)))
        x = self.maxpool1(F.relu(self.conv2(x)))
        x = self.maxpool1(F.relu(self.conv3(x)))
        x = self.maxpool1(F.relu(self.conv4(x)))
        x = self.maxpool1(F.relu(self.conv5(x)))
        x = self.maxpool2(F.relu(self.conv6(x)))
        x = F.relu(self.conv7(x))
        x = F.relu(self.conv8(x))
        x = F.relu(self.conv9(x))
        return x


class YoloLoss():
    def __call__(self, out, label):
        s = 0.0
        for i in range(len(out)):
            x, y, w, h, p = out[i, 0, :, :], out[i, 1, :, :], out[i, 2, :, :], out[i, 3, :, :], out[i, 4, :, :]
            x_hat, y_hat, w_hat, h_hat, p_hat = label[i, 0, :, :], label[i, 1, :, :], label[i, 2, :, :], label[i, 3, :,
                                                                                                         :], label[i, 4,
                                                                                                             :, :]

            # print(x.shape, y.shape, x_hat.shape, y_hat.shape)
            f1 = 5 * torch.sum(p_hat * ((x - x_hat) ** 2 + (y - y_hat) ** 2))
            f2 = 5 * torch.sum(
                p_hat * ((torch.sqrt(w) - torch.sqrt(w_hat)) ** 2 + (torch.sqrt(h) - torch.sqrt(h_hat)) ** 2))
            f3 = 0.5 * torch.sum((1 - p_hat) * (p - p_hat) ** 2)
            f4 = torch.sum(p_hat * (p - p_hat) ** 2)

            s += f1 + f2 + f3 + f4

        return s / len(out)


class SignDataset(torch.utils.data.Dataset):
    def __init__(self, df, img_path, label_path, bbox_path, transform=None):
        self.df = df
        self.transform = transform
        self.imgPath = img_path
        self.labelPath = label_path
        self.bboxPath = bbox_path

    def __getitem__(self, index):
        row = self.df.iloc[index]

        imgFile = row['imgFile']
        labelFile = row['labelFile']
        bboxFile = row['bboxFile']

        # RAW data...
        # img = Image.open(self.imgPath + imgFile)
        # label = np.load(self.labelPath + labelFile)

        # AUGMENTING data...
        img = plt.imread(self.imgPath + imgFile)
        bbox_anno = open(self.bboxPath + bboxFile).read()
        list_bboxs = preprocess_bbox(bbox_anno)

        augmented_img, augmented_list_bbox = img_aug(img, list_bboxs)  # augmented_bbox = list bboxs
        img = Image.fromarray(augmented_img.astype('uint8'))
        label = anno_label(augmented_img, augmented_list_bbox, isProcessedLabel=True)
        #####
        # input(...)
        # (H-W-C) to (C-H-W)
        label = np.transpose(label, (2, 0, 1))

        if self.transform is not None:
            img = self.transform(img)

        return img, label

    def __len__(self):
        n, _ = self.df.shape
        return n


# change to list bbox
def preprocess_bbox(bboxAnno):
    listbb = []
    bboxs = bboxAnno.split('\n')
    for bbox in bboxs:
        bb = bbox.split(',')
        if len(bb) > 1:
            listbb.append([bb[0], bb[1], bb[4], bb[5]])
    return listbb


if __name__ == '__main__':
    train()

    os.system('shutdown -s')
