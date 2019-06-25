import torch as tr
import cv2
import numpy as np
import rospkg

class CNN32(tr.nn.Module):
    def __init__(self):
        super(CNN32, self).__init__()  # 3x32x32

        self.conv = tr.nn.Sequential(
            tr.nn.Conv2d(in_channels=3, out_channels=32, kernel_size=5, stride=1, padding=2),
            tr.nn.ReLU(),
            tr.nn.MaxPool2d(2, 2),

            tr.nn.Conv2d(32, 64, 3, 2, 1),
            tr.nn.ReLU(),
            tr.nn.MaxPool2d(2, 2),

            tr.nn.Conv2d(64, 64, 3, 1, 1),
            tr.nn.ReLU(),
            # tr.nn.MaxPool2d(2, 2),
        )

        self.fc1 = tr.nn.Sequential(
            tr.nn.Linear(64 * 4 * 4, 30),
            tr.nn.ReLU(),
        )

        self.fc2 = tr.nn.Sequential(
            tr.nn.Linear(30, 3),
            tr.nn.Softmax(dim=1),
        )

    def forward(self, x):
        # drop = tr.nn.Dropout(droprate)

        x = self.conv(x)
        x = x.view(x.size(0), -1)
        x = self.fc1(x)
        # x = drop(x)
        x = self.fc2(x)
        return x

class Net:
    def __init__(self):
        path = rospkg.RosPack().get_path('team105')

        self.device = tr.device("cuda:0" if tr.cuda.is_available() else "cpu")
        self.net = CNN32().to(self.device)
        self.net.load_state_dict(tr.load(path + '/param/param_sign.9977641105651855', map_location=self.device))

    def predict(self, img, new_size=32):
	try:
		img = np.array(img, dtype= np.float32) / 255.
		img = cv2.resize(img, (new_size, new_size))

		img= img.reshape(1,new_size,new_size,3).transpose((0,3,1,2))

		with tr.no_grad():
		    img = tr.from_numpy(img).to(self.device)
		    output = self.net(img)
		    output = tr.argmax(output)

		return int(output) # 0 = not turn, 1 = turn right, 2 = turn left
	except:
		return 0
        print('LOI')

    # with tr.no_grad():
    #     # while True:
    #     #     dir= raw_input("file directory: ")
    #     #     if dir == '': break
    #     for i in range(24,28):
    #         dir= 'other imgs/o27.png'#'other imgs/o' + str(i) + '.png'
    #
    #         img= cv2.imread(dir)
    #         img= np.flip(img,1)
    #         cv2.imshow(str(predict(img)), cv2.resize(img, (150,150)))
    #         cv2.waitKey(0)
