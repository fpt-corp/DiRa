# GUIDELINE for Simulator

## Cài đặt

Download link: http://bit.ly/cds-simulator-2019

## Hướng dẫn sử dụng phần mềm

Chạy file simualator.x86-64

Tại giao diện mở chương trình

    Nhập tên đội: Tên đội thi đã đăng ký(sử dụng tên đội là team1 để test)
    Nhập IP của máy chủ chạy ROS_MASTER theo cấu trúc ws://xxx.xxx.xxx.xxx:
    trong đấy xxx.xxx.xxx.xxx: là ip của máy chủ ROS_MASTER
              pppp: là port sử dụng cho giao tiếp
    
![alt text](https://github.com/fpt-corp/DiRa/blob/master/Image_Resource/DiRa_Simulator_1.png)

    Sau khi kết nối, chương trình sẽ đếm ngược 2 phút và 
    truyền hình ảnh từ camera thu được cho ROS MASTER. Với hình ảnh thu được, 
    các đội thi phải truyền ngược lại tốc độ và góc lái với topic được định nghĩa như sau:
    
/tênđội/set_speed: Topic được publish từ ROS_node được định nghĩa dưới dạng số thực (Float32). Là tốc độ xe cần đạt. ( Mặc định đang để là 
/team1/set_speed, nếu nhập tên đội khác, cần sửa lại topic trong code /lane_detect/src/main.cpp theo tên đội đã nhập ở app)

/tênđội/set_angle: Topic được publish từ ROS_node định nghĩa dưới dạng số thực (Float32). Truyền góc lái của xe. ( Mặc định đang để là 
/team1/set_angle, nếu nhập tên đội khác, cần sửa lại topic trong code /lane_detect/src/carcontrol.cpp theo tên đội đã nhập ở app)

/tênđội/set_camera_angle: Topic được publish từ ROS_node định nghĩa dưới dạng số thực (Float32). Truyền quay của camera.

/tênđội/camera/rgb/compressed: Topic dùng để subcribe ảnh rgb thu được trên xe. Ảnh thu được là ảnh nén theo chuẩn “img”.( Mặc định đang để là 
/team1/camera/rgb/compressed, nếu nhập tên đội khác, cần sửa lại topic trong code /lane_detect/src/main.cpp theo tên đội đã nhập ở app)

/tênđội/camera/depth/compressed: Topic dùng để subcribe ảnh depth thu được trên xe. Ảnh thu được là ảnh nén theo chuẩn “img”.( Mặc định đang để là 
/team1/camera/depth/compressed, nếu nhập tên đội khác, cần sửa lại topic trong code /lane_detect/src/main.cpp theo tên đội đã nhập ở app)

## Kết nối máy chủ ROS Master đến phần mềm giả lập

B1: Tải các package cơ bản tại https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/ROS_Package_example

video_stream: tạo websocket để kết nối đến phần mềm mô phỏng

lane_detect: thuật toán xử lý ảnh mẫu của ban tổ chức ( các thông số chưa chuẩn, cần chỉnh lại)

B2: Chạy thử chương trình

Tạo websocket để kết nối đến phần mềm mô phỏng

`$ roslaunch video_stream video_stream.launch`

Kiểm tra thử hình ảnh nhận về từ phần mềm mô phỏng

`$ rosrun video_stream video_stream _image_transport:=compressed` 

Chạy thử code mẫu

`$ rosrun lane_detect lane_detect _image_transport:=compressed`
