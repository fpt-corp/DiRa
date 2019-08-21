import rospy
from std_msgs.msg import String, Bool
from param import Param


class LCDControl:
    def __init__(self, param):
        self.param = param

        self.b1 = False
        self.b2 = False
        self.b3 = False
        self.b4 = False

        self.option = 0
        self.choose_symbol = ['>', ' ', ' ', ' ']

        self.line1 = '0:0:{}speed={:2}-{:2}'
        self.line2 = '0:1:{}steer_decay={:.2f}'
        self.line3 = '0:2:{}sign_size={:2}-{:2}'
        self.line4 = '0:3:{}speed_slow={:.1f}'

        self.lcd_pub = rospy.Publisher("/lcd_print", String, queue_size=4)

        self.sub_b1 = rospy.Subscriber("/bt1_status", Bool, callback=self.callback1)
        self.sub_b2 = rospy.Subscriber("/bt2_status", Bool, callback=self.callback2)
        self.sub_b3 = rospy.Subscriber("/bt3_status", Bool, callback=self.callback3)
        self.sub_b4 = rospy.Subscriber("/bt4_status", Bool, callback=self.callback4)

    def callback1(self, data):
        self.b1 = data.data

    def callback2(self, data):
        self.b2 = data.data

    def callback3(self, data):
        self.b3 = data.data

    def callback4(self, data):
        self.b4 = data.data

    def check_button(self):
        if self.b1:
            if self.option == 0:
                if not self.b4:
                    self.param.base_speed = min(25, self.param.base_speed + 1)
                else:
                    self.param.min_speed = min(20, self.param.min_speed + 1)
            if self.option == 1:
                self.param.speed_decay = min(5, self.param.speed_decay + 0.1)
            if self.option == 2:
                if not self.b4:
                    self.param.sign_size_2 = min(50, self.param.sign_size_2 + 1)
                else:
                    self.param.sign_size_1 = min(40, self.param.sign_size_1 + 1)
            if self.option == 3:
                self.param.speed_slow_down = max(3, self.param.speed_slow_down + 0.5)

            self.publish_lcd()
            print('bt1')

        elif self.b2:
            if self.option == 0:
                if not self.b4:
                    self.param.base_speed = max(10, self.param.base_speed - 1)
                else:
                    self.param.min_speed = max(10, self.param.min_speed - 1)
            if self.option == 1:
                self.param.speed_decay = max(1, self.param.speed_decay - 0.1)
            if self.option == 2:
                if not self.b4:
                    self.param.sign_size_2 = max(20, self.param.sign_size_2 - 1)
                else:
                    self.param.sign_size_1 = max(10, self.param.sign_size_1 - 1)
            if self.option == 3:
                self.param.speed_slow_down = max(0, self.param.speed_slow_down - 0.5)

            self.publish_lcd()
            print('bt2')

        elif self.b3:
            self.option = (self.option + 1) % 4

            self.publish_lcd()
            print('bt3')

        # elif self.b4:
        #     self.option = (self.option - 1) % 4
        #
        #     self.publish_lcd()
        #     print('bt4')

    def publish_lcd(self):
        self.lcd_pub.publish(self.line1.format(self.choose_symbol[self.option], self.param.min_speed, self.param.base_speed))
        self.lcd_pub.publish(self.line2.format(self.choose_symbol[self.option - 1], self.param.speed_decay))
        self.lcd_pub.publish(self.line3.format(self.choose_symbol[self.option - 2], self.param.sign_size_1, self.param.sign_size_2))
        self.lcd_pub.publish(self.line4.format(self.choose_symbol[self.option - 3], self.param.speed_slow_down))
