#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# GPIO控制LED亮度，制作呼吸灯效果
# http://blog.mangolovecarrot.net/2015/04/28/raspi-study02/
#
# 通过LED的电流越大，LED越亮，电流越小，LED越暗。如果可以控制输出电流
# 大小就可以控制LED的明暗了。
# 但是树莓派的各引脚并没有直接调整输出电流大小的功能。
# 我们先学习一个名词：脉宽调制(PWM)
# 简单的说，PWM技术就是不停的通断电路并控制通断持续的时间片段长度来控制
# 用电器在单位时间内实际得到的电能。
# 这么说好像还是复杂了，再简单点说，如果你的手足够快，打开电灯开关后
# 马上关闭，如果这个时间间隔足够短，灯丝还没有全部亮起来就暗下去了。
# 你再次打开电灯再关闭，再打开再关闭... 如果你一直保持相同的频率，
# 那么电灯应该会保持一个固定的亮度不变。理论上，你可以通过调整开灯持续
# 的时间长度和关灯持续的时间长度的比例就能得到不同亮度了。这个比例
# 被称为"占空比"。
# PWM就是差不多这个意思。
# 树莓派1代B型的26个针脚里，有一个特殊的GPIO口是支持硬件PWM的，不过
# 从B+开始不知道什么原因这个很实用的接口被去掉了。


import RPi.GPIO
import time

# 指定GPIO口的引脚编号: 主板编号模式或者BCM编号
RPi.GPIO.setmode(RPi.GPIO.BOARD)
#RPi.GPIO.setmode(RPi.GPIO.BCM)

# 不需要显示针脚警告消息
RPi.GPIO.setwarnings(False)

# 指定GPIO14(就是LED长针连接的GPIO针脚)的模式为输出模式
# 如果上面GPIO口的选定模式指定为主板模式的话，这里就应该指定8号而不是14号。
LED_GPIO = 8
RPi.GPIO.setup(LED_GPIO, RPi.GPIO.OUT)

# 创建一个 PWM 实例，需要两个参数，第一个是GPIO端口号，这里我们用14号
# 第二个是频率（Hz），频率越高LED看上去越不会闪烁，相应对CPU要求就越高，
# 设置合适的值就可以
pwm = RPi.GPIO.PWM(LED_GPIO, 80)

# 启用 PWM，参数是占空比，范围：0.0 <= 占空比 >= 100.0
pwm.start(0)

while True:
    try:
        # 电流从小到大，LED由暗到亮
        for i in range(0, 101):
            # 更改占空比
            pwm.ChangeDutyCycle(i)
            time.sleep(.02)
        # 再让电流从大到小，LED由亮变暗
        for i in range(100, -1, -1):
            pwm.ChangeDutyCycle(i)
            time.sleep(.02)
    except KeyboardInterrupt:
        print("\nexit")
        break
    except:
        print("error")
        break;

# 停用 PWM
pwm.stop()

# 最后清理GPIO口，建议每次程序结束时清理一下，好习惯
RPi.GPIO.cleanup()
