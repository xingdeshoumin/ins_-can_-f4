# INS_CAN_F4

## 通信协议

### 发送格式

帧格式：DATA
帧类型：标准帧
DLC:8字节

#### LEG_imu

|      名称      |  ID  |            标识符             |
| :------------: | :--: | :---------------------------: |
|  RIGHT_UP_LEG  |  0   |        0x01 0x02 0x03         |
|  LEFT_UP_LEG   |  1   |        0x04 0x05 0x06         |
| LEFT_DOWN_LEG  |  2   |        0x07 0x08 0x09         |
| RIGHT_DOWN_LEG |  3   |        0x0A 0x0B 0x0C         |
|      BODY      |  4   | 0x0D 0x0E 0x0F 0x10 0x11 0x12 |

- 标识符：0x01 + id*3

    | 数据域  |            内容            |
    | :-----: | :------------------------: |
    | DATA[0] |    (uint8_t)INS_quat[0]    |
    | DATA[1] | (uint8_t)(INS_quat[0]>>8)  |
    | DATA[2] | (uint8_t)(INS_quat[0]>>16) |
    | DATA[3] | (uint8_t)(INS_quat[0]>>24) |
    | DATA[4] |    (uint8_t)INS_quat[1]    |
    | DATA[5] | (uint8_t)(INS_quat[1]>>8)  |
    | DATA[6] | (uint8_t)(INS_quat[1]>>16) |
    | DATA[7] | (uint8_t)(INS_quat[1]>>24) |

- 标识符：0x02  + id*3

    | 数据域  |            内容            |
    | :-----: | :------------------------: |
    | DATA[0] |    (uint8_t)INS_quat[2]    |
    | DATA[1] | (uint8_t)(INS_quat[2]>>8)  |
    | DATA[2] | (uint8_t)(INS_quat[2]>>16) |
    | DATA[3] | (uint8_t)(INS_quat[2]>>24) |
    | DATA[4] |    (uint8_t)INS_quat[3]    |
    | DATA[5] | (uint8_t)(INS_quat[3]>>8)  |
    | DATA[6] | (uint8_t)(INS_quat[3]>>16) |
    | DATA[7] | (uint8_t)(INS_quat[3]>>24) |

- 标识符：0x03 + id*3

    | 数据域  |             内容             |
    | :-----: | :--------------------------: |
    | DATA[0] |    (uint8_t)encoder_angle    |
    | DATA[1] | (uint8_t)(encoder_angle>>8)  |
    | DATA[2] | (uint8_t)(encoder_angle>>16) |
    | DATA[3] | (uint8_t)(encoder_angle>>24) |
    | DATA[4] |              0               |
    | DATA[5] |              0               |
    | DATA[6] |              0               |
    | DATA[7] |              0               |

#### BODY _imu

在**LEG_imu**的基础上增加

- 标识符：0x04 + id*3

    | 数据域  |          内容           |
    | :-----: | :---------------------: |
    | DATA[0] |    (uint8_t)accel[0]    |
    | DATA[1] | (uint8_t)(accel[0]>>8)  |
    | DATA[2] | (uint8_t)(accel[0]>>16) |
    | DATA[3] | (uint8_t)(accel[0]>>24) |
    | DATA[4] |    (uint8_t)accel[1]    |
    | DATA[5] | (uint8_t)(accel[1]>>8)  |
    | DATA[6] | (uint8_t)(accel[1]>>16) |
    | DATA[7] | (uint8_t)(accel[1]>>24) |

- 标识符：0x05 + id*3

    | 数据域  |          内容           |
    | :-----: | :---------------------: |
    | DATA[0] |    (uint8_t)accel[2]    |
    | DATA[1] | (uint8_t)(accel[2]>>8)  |
    | DATA[2] | (uint8_t)(accel[2]>>16) |
    | DATA[3] | (uint8_t)(accel[2]>>24) |
    | DATA[4] |    (uint8_t)gyro[0]     |
    | DATA[5] |  (uint8_t)(gyro[0]>>8)  |
    | DATA[6] | (uint8_t)(gyro[0]>>16)  |
    | DATA[7] | (uint8_t)(gyro[0]>>24)  |

- 标识符：0x06 + id*3

    | 数据域  |          内容          |
    | :-----: | :--------------------: |
    | DATA[0] |    (uint8_t)gyro[1]    |
    | DATA[1] | (uint8_t)(gyro[1]>>8)  |
    | DATA[2] | (uint8_t)(gyro[1]>>16) |
    | DATA[3] | (uint8_t)(gyro[1]>>24) |
    | DATA[4] |    (uint8_t)gyro[2]    |
    | DATA[5] | (uint8_t)(gyro[2]>>8)  |
    | DATA[6] | (uint8_t)(gyro[2]>>16) |
    | DATA[7] | (uint8_t)(gyro[2]>>24) |

### 接收格式

帧格式：DATA
帧类型：标准帧
DLC:8字节
标识符：0x100

| 数据域  |       内容       |
| :-----: | :--------------: |
| DATA[0] |   mag_cal_flag   |
| DATA[1] | encoder_cal_flag |
| DATA[2] |   mpu_cal_flag   |
| DATA[3] |      mag_en      |
| DATA[4] |       NULL       |
| DATA[5] |       NULL       |
| DATA[6] |       NULL       |
| DATA[7] |       NULL       |
