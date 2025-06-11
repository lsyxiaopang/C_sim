import numpy as np
import serial
import time

G_len=1

START_FACT=b"\x01"
STOP_FACT=b"\xFF"

def init_sys(ser_name)->serial.Serial:
    #* This function initizalize the factoization machine
    #Note: We need to send two series of 0x00 to make sure the system is ready
    #
    ser=serial.Serial(ser_name,115200)
    ser.write(bytearray([0x00,0x00]))
    ser.read(2)
    return ser

def start_factorize(ser:serial.Serial):
    #*This function activates the factorization function
    #!Note: DON'T use this function directly, use repeat_factor instead
    #
    ser.write(START_FACT)
    back_message=ser.read(1)
    print("Back message {}".format(back_message))

def stop_factorize(ser:serial.Serial):
    #*This function stops the factorization process and turn the status into cmd process
    ser.write(STOP_FACT)
    
def repeat_factor(ser:serial.Serial,input_data,groups):
    #*This function repeat the factorization process(G_len samples as a group)
    #*The return value is a tuple made up with (back_val,back_count)
    #! Data length now:30-32-bit(30-factorization number,32-count)
    cut_data=[]
    back_val=[]
    back_count=[]
    info_dict={"back temp":[],"region increase":[],"region max":[],"pbit len":[]}
    for i in range(8):#8*8=64
        cut_data.append((input_data>>(i*8))&0xFF)
    for j in range(groups):
        for i in range(G_len):
            ser.write(bytearray(cut_data))
        # print("Waiting for the factorization process to finish")
        for i in range(G_len):
            bdata=ser.read(18)
            v=(((bdata[3]<<24)+(bdata[2]<<16)+(bdata[1]<<8)+bdata[0]))#*No need to *2+1 to get the final value
            # back_val.append(bdata[1]*256+bdata[0])#*No need to *2+1 to get the final value
            c=((bdata[9]<<40)+(bdata[8]<<32)+
                (bdata[7]<<24)+(bdata[6]<<16)+(bdata[5]<<8)+(bdata[4]))
            if(v!=0):
                back_val.append(v)
                back_count.append(c)
            else:
                back_val.append(1)
                back_count.append(np.nan)
            # back_count.append((bdata[5]<<24)+(bdata[4]<<16)+(bdata[3]<<8)+(bdata[2]))
        print(F"Group {j} finished! Count result:{back_count[-1]}")
    #*Only the last config in the group would be saved
    info_dict["pbit len"]=bdata[10]
    info_dict["region max"]=[bdata[12],bdata[11]]
    info_dict["region increase"]=[bdata[14],bdata[13]]
    info_dict["back temp"]=[bdata[17],bdata[16],bdata[15]]

    return (back_val,back_count,info_dict)

def set_pbit_num(ser:serial.Serial,input_num):
    #*This function sets the **number of the pbit value**
    cmd=[0x02,input_num]
    ser.write(bytearray(cmd))
    print(ser.read(2))
def set_value(ser:serial.Serial,back_ground_temp=[21,22,24],region_par=[7,8],region_coeff=[20,21],max_iter_pow=[31]):
    #*This function sets the configuration value of the system
    addrs=[0x10,0x11,0x12,0x20,0x21,0x30,0x31,0x03]
    datas=back_ground_temp+region_par+region_coeff+max_iter_pow
    comb=[]
    for i in range(len(datas)):
        comb.append(addrs[i])
        comb.append(datas[i])
    ser.write(bytearray(comb))
    print(ser.read(8*2))
    
    


if __name__=="__main__":    
#!有关参数的含义与计算
#! 第一组参数：系统温度参数(a1,a2,a3) 计算方式 1/T=2^(-a1)+2^(-a2)+2^(-a3)
#! 第二组参数：局域温度参数(b1,b2) 计算方式 sn=(1-b)*sn'+k*b*val b=2^(-b1)+2^(-b2) 
#! 第三组参数：局域温度限制参数(k1,k2) 计算方式 k=1-2^(-k1)-2^(-k2)
#! 第四个参数：待分解的质因数(如果待分解质因数中存在有0xFF会导致求解器出错，需要避免)
#!## 第五个参数：数据编号（便于文件命名）(目前不再需要这一功能，不再输出散文件)
    value_table=[[37,37,39],[7,8],[14,15],803964919,1]   
    # value_table=[[30,30,33],[4,5],[14,15],12337337,3] #! 保留这一行（注释）用于系统快速验证
    #*Start the system
    serial_number="/dev/tty_back_down"
    ser=init_sys(serial_number)

    #*Reconfigure the system
    set_value(ser,value_table[0],value_table[1],value_table[2],[16])
    #! 在这里标记用到了多少个pBit(例如分解30位数需要14个pBit)
    set_pbit_num(ser,14) 
    # set_pbit_num(ser,11) #!保留这一行（注释）用于系统快速验证

    #*Doing the factorization
    start_factorize(ser)
    #! 总分解次数：G_len*100
    val,count,conf=repeat_factor(ser,value_table[3],10) 
    count=np.array(count)
    stop_factorize(ser)
    stop_factorize(ser)

    # #*Post-process of the data `val,count 
    print(F"Count mean:{np.mean(count)}")
    print(F"Count std:{np.std(count)}")
    # print(count)
    # print(count%8)
    # import matplotlib.pyplot as plt
    # plt.hist(np.log10(count),bins=20)
    # plt.show()
    print(F"Value output{set(val)}")
    print(F"Config {conf}")
    # # print(count%2)
    # np.savetxt("H_{}_{}_{}_{}.txt".format(value_table[3],value_table[0],value_table[1],value_table[4]),count)