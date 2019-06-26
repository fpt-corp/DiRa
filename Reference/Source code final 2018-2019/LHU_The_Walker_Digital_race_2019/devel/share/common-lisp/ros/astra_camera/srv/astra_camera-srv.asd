
(cl:in-package :asdf)

(defsystem "astra_camera-srv"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "GetSerial" :depends-on ("_package_GetSerial"))
    (:file "_package_GetSerial" :depends-on ("_package"))
  ))