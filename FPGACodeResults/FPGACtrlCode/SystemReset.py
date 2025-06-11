import sys
import serial
import time
#* 这个程序是用在系统被搞坏需要重置的情形，先把之前的程序退出掉
#* 此处端口的选取是通过命令行参数决定例如：python SystemReset.py /dev/tty_back_up

ser_name=sys.argv[1]
ser=serial.Serial(ser_name,115200)
ser.write(bytearray([0x01]))
ser.write(bytearray([0xFF,0xFF]))
time.sleep(1)
ser.reset_output_buffer()
ser.reset_input_buffer()
ser.close()
