## 11.26.20：39

init已经跑通， 但是BMI088的中断回调一直不触发， 现波形如下：

![IMG_1495](updatelogs.assets/IMG_1495.jpg)

CS1_Accel在init阶段有这个波形， 后续没有



![IMG_1496](updatelogs.assets/IMG_1496.jpg)

SCK正常



![IMG_1497](updatelogs.assets/IMG_1497.jpg)

MISO正常



![IMG_1498](updatelogs.assets/IMG_1498.jpg)

MOSI正常



![IMG_1499](updatelogs.assets/IMG_1499.jpg)

INT1_Accel正常



![IMG_1500](updatelogs.assets/IMG_1500.jpg)

INT1_Gyro正常



![IMG_1501](updatelogs.assets/IMG_1501.jpg)

CS1_Gyro一直为低



## 12.1.11：24

如果在HAL_CAN_RxFifo0MsgPendingCallback中没有switch那个id， 是不会进中断的。