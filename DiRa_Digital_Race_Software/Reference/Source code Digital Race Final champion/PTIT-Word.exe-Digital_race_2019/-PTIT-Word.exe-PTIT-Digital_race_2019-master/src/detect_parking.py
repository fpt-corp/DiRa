import cv2, numpy as np


class DetectParking:
    def __init__(self):
        self.isFirstDetection = True
        self.isSecondLine = False
        self.lastRho = -1

    def reset(self):
        self.isFirstDetection = True
        self.isSecondLine = False
        self.lastRho = -1

    def isHorizontal(self, theta, maxDegreeDifference=6):
        return (np.abs(theta - np.pi / 2) < (np.deg2rad(maxDegreeDifference)))

    def findLine(self, thres, minLineLength):
        lines = cv2.HoughLines(thres, rho=1, theta=np.deg2rad(1), threshold=minLineLength)

        if self.isHorizontal(lines[0, 0, 1]):
            return lines[0, 0, 0], lines[0, 0, 1]

        line = lines[np.argmin(np.abs(lines[:, 0, 1] - np.pi / 2.))]

        rho, theta = line[0, 0], line[0, 1]

        if not self.isHorizontal(theta):
            return None

        return rho, theta

    def detect_parking(self, img, distanceBetween2Lines=45, distanceFromTop=175, minLineLength=140,
                       binaryThreshold=230):

        height = img.shape[0]
        width = img.shape[1]

        # img = bird_view(img, False)

        # left cover
        cv2.drawContours(img, [np.array([(width // 2, 0), (0, 0), (0, height * 2 // 3)])]
                         , 0, (0, 0, 0), -1)
        # right cover
        cv2.drawContours(img, [np.array([(width // 2, 0), (width, 0), (width, height * 2 // 3)])]
                         , 0, (0, 0, 0), -1)
        # bottom cover
        cv2.drawContours(img,
                         [np.array([(0, height * 10 // 11), (width, height * 10 // 11), (width, height), (0, height)])]
                         , 0, (0, 0, 0), -1)
        # top cover
        cv2.drawContours(img, [np.array([(0, 0), (width, 0), (width, int(height*0.6)), (0, int(height*0.6))])]
                         , 0, (0, 0, 0), -1)

        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        _, thres = cv2.threshold(gray, binaryThreshold, 255, cv2.THRESH_BINARY)

        try:
            rho, theta = self.findLine(thres, minLineLength)
            if rho < 0:
                raise Exception
        except:
            return False, (0, 0), (0, 0)

        a = np.cos(theta)
        b = np.sin(theta)
        x0 = a * rho
        y0 = b * rho
        x1 = int(x0 + 1000 * (-b))
        y1 = int(y0 + 1000 * (a))
        x2 = int(x0 - 1000 * (-b))
        y2 = int(y0 - 1000 * (a))

        pt1 = (x1, y1)
        pt2 = (x2, y2)

        if (not self.isFirstDetection) and (np.abs(rho - self.lastRho) > distanceBetween2Lines):
            self.isSecondLine = True
        else:
            self.isFirstDetection = False

        self.lastRho = rho

        if self.isSecondLine and rho > distanceFromTop:
            return True, pt1, pt2
        else:
            return False, pt1, pt2
