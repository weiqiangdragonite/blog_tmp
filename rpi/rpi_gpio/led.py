#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# 使用GPIO接口控制发光二极管闪烁
# http://blog.mangolovecarrot.net/2015/04/20/raspi-study01/
#
# LED灯有一长一短两根针脚，如果将较长的一根连上电源正极，较短的一根
# 连上电源负极造成电位差就可以点亮LED灯。
# 将较短的一根连上树莓派的GND(也就是负极)端，较长的一根不要直接连上
# 树莓派的5V或者3.3V(两者都可理解为正极或高电平，以后统称高低电平，
# 不再另行解释)，而是连接到一个GPIO针脚上。
# 然后我们可以通过程序控制GPIO口的电位高低状态即可控制LED的亮(GPIO口
# 设置为高电平)或灭(GPIO口设置为低电平)。


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

# 循环10次
for i in range(0, 10):
	# 让GPIO14输出高电平(LED灯亮)
	RPi.GPIO.output(LED_GPIO, True)
	# 持续一段时间
	time.sleep(0.5)
	# 让GPIO14输出低电平(LED灯灭)
	RPi.GPIO.output(LED_GPIO, False)
	# 持续一段时间
	time.sleep(0.5)

# 永远循环
while True:
    try:
        RPi.GPIO.output(LED_GPIO, True)
        time.sleep(0.5)
        RPi.GPIO.output(LED_GPIO, False)
        time.sleep(0.5)
    except:
        break

# 最后清理GPIO口，建议每次程序结束时清理一下，好习惯
RPi.GPIO.cleanup()
