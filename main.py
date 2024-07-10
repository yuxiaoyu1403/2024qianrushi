import sensor, time, lcd
import Message,detection

sensor.reset()                         # Reset and initialize the sensor.
sensor.set_hmirror(True)
sensor.set_vflip(True)
sensor.set_pixformat(sensor.RGB565)    # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)      # Set frame size to QVGA (320x240)
sensor.set_windowing((240, 240))       # Set 240x240 window.
sensor.skip_frames(time=2000)          # Let the camera adjust.
sensor.set_auto_gain(False)     # 追踪颜色关闭自动增益
sensor.set_auto_whitebal(False) # 追踪颜色关闭白平衡

clock = time.clock()

# *********************************初始化参数**********************************#
Message.Ctr.Workdata = 1    # 0:red  1:yellow  2:green
Message.Ctr.WorkMode = 1    # 1:色块      10:空闲模式
Message.Ctr.IsDebug = 0     # 1:调试模式    0:禁止LOG
Message.Ctr.Use_lcd = 1     # 1:使能LCD显示 0:关闭LCD显示
# *****************************************************************************#

if Message.Ctr.Use_lcd == 1:
    lcd.init()

while True:
    clock.tick()

    # 接收串口数据
#    Message.UartReadBuffer()

    # 工作模式
    if Message.Ctr.WorkMode == 1:
        Img = sensor.snapshot()
        if detection.BanCheck(Img) == 1:
            detection.Number(Img)
            Message.UartSendData(
                Message.DotDataPack(
                    detection.Dot.flag, detection.Dot.num,
                    detection.Dot.x, detection.Dot.y, Message.Ctr.T_ms
                )
            )
    lcd.display(Img)
#    # 计算程序运行频率
#    if Message.Ctr.IsDebug == 1:
#        fps = int(clock.fps())
#        Message.Ctr.T_ms = (int)(1000 / fps)
#        print("fps", fps, "T_ms", Message.Ctr.T_ms)
# ************************************ end ***********************************#
