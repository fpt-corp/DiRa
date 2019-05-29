import torch as tr, cv2, numpy as np

device = tr.device("cuda" if tr.cuda.is_available() else "cpu")

import rospkg

path = rospkg.RosPack().get_path('team105')


class NET(tr.nn.Module):
    def __init__(self):
        super(NET, self).__init__()  # 3x80x35

        self.conv = tr.nn.Sequential(
            tr.nn.Conv2d(in_channels=3, out_channels=32, kernel_size=5, stride=1, padding=2),
            tr.nn.ReLU(),
            tr.nn.MaxPool2d(2, 2),

            tr.nn.Conv2d(32, 64, 3, 2, 1),
            tr.nn.ReLU(),
            tr.nn.MaxPool2d(2, 2),

            tr.nn.Conv2d(64, 128, 3, 2, 1),
            tr.nn.ReLU(),
            # tr.nn.MaxPool2d(2, 2),
        )

        self.fc1 = tr.nn.Sequential(
            tr.nn.Linear(2048, 20),
            tr.nn.ReLU(),
        )

        self.fc2 = tr.nn.Sequential(
            tr.nn.Linear(20, 2),
            tr.nn.Softmax(dim=1),
        )

    def forward(self, x):
        x = self.conv(x)
        x = x.view(x.size(0), -1)
        x = self.fc1(x)
        x = self.fc2(x)
        return x


net = NET().to(device)
net.load_state_dict(tr.load(path + '/param/angle_semi'))


def predict(img):
    # input size 320x240

    img = img[80:, :, :]
    img = cv2.resize(img, (64, 64))

    img = img.transpose((2, 0, 1))
    img = np.expand_dims(img, 0)

    # if img.max()>1:
    img = np.array(img, dtype=np.float32) / 255.

    with tr.no_grad():
        img = tr.from_numpy(img).to(device)
        output = net(img)
        output = tr.argmax(output)

    return output.item()  # 0= not turn; 1= turn


if __name__ == "__main__":
    # import os
    i = 0
    folder = '/media/nda97531/01D445B1611FA9E0/ML/cds/video/'
    file = 'output_around.avi'
    cap = cv2.VideoCapture(folder + file)
    while (cap.isOpened()):
        hasnext, frame = cap.read()
        if not hasnext: break
        frame = cv2.flip(frame, 1)
        img = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

        result = predict(img)

        if result == 1:
            cv2.imwrite('frametest/' + str(i) + '.jpg', frame)
            i += 1
            cv2.putText(frame, 'turn', (100, 100), cv2.FONT_HERSHEY_SIMPLEX, 2, (20, 0, 190), 5)
        cv2.imshow('video', frame)
        cv2.waitKey(20)
    # with tr.no_grad():
    #     for filename in os.listdir(folder):
    #         # print(filename)
    #         # input('..')
    #         img= cv2.imread(folder+filename)
    #         img= cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    #
    #         img= cv2.resize(img,(320,240))
    #         img = img[40:-5, :, :]
    #         # img= cv2.flip(img,1)
    #
    #         # if predict(img)==1:# and filename.split('_')[0]=='0':
    #         cv2.imshow(str(predict(img)), img)
    #         cv2.waitKey(0)
    #         i+=1
    # print (i)
