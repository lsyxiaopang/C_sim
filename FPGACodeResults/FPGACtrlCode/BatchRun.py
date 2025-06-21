VERSION="0.2"
import tomllib
import time
from threading import Timer
from loguru import logger

toml_name="FPGACodeResults/Running/Test/612/testinfo.toml"
toml_file=open(toml_name,"rb")
config_data=tomllib.load(toml_file)
#*读取基本数据
version_toml=config_data["version"]
if(VERSION!=version_toml):
    raise

input_base=config_data["input_folder"]
output_base=config_data["output_folder"]
fpga_serial=config_data["fpga_control"]

#TODO 在目标文件夹创建对应日志文件
logger.add(output_base+"info_log.log")
logger.add(output_base+"debug.log",level="DEBUG")
logger.info("Read config finished!")

#*系统连接FPGA
import FactorizationMachineConnector as fmc 
import numpy as np
ser=fmc.init_sys(fpga_serial)
logger.info("FPGA connected")

#! 每半个小时处理一次，保证程序在工作
def schedule_debug_log():
    """定时触发 debug 日志并循环调度"""
    try:
        # 输出调试信息（可自定义内容）
        logger.debug("Program checking: {}", time.time())
    except Exception as e:
        logger.error("Program error: {}", e)
    finally:
        # 重新调度下一次任务（关键：形成循环）
        timer = Timer(interval=1800, function=schedule_debug_log)  # 1800秒=30分钟
        timer.daemon = True  # 设为守护线程，主程序退出时自动终止
        timer.start()

schedule_debug_log()

try:
    #*开始每一组数据的处理与运行
    sections=[(k,v) for k,v in config_data.items() if isinstance(v,dict)]
    for sec_name,sec_data in sections:
        logger.info(F"Start section {sec_name}")
        back_temp=sec_data["back_temp"]
        region_coeff=sec_data["region_temp_coeff"]
        region_top=sec_data["region_temp_top"]
        pbit_c=sec_data["pbit_count"]
        iter_max=sec_data["max_iter_pow"]
        #*配置质因数求解器状态
        fmc.set_value(ser,back_temp,region_coeff,region_top,[iter_max])
        fmc.set_pbit_num(ser,pbit_c)
        fmc.start_factorize(ser)
        #*开始读取数据
        datas=np.loadtxt(input_base+sec_data["input_csv"],dtype=np.uint64).reshape(-1)
        outputfile=open(output_base+sec_data["output_csv"],"a")
        logger.info(F"Read {datas.shape[0]} data")
        repeat_times=sec_data["repeat"]
        # results=[]
        for data in datas:
            val,count,conf=fmc.repeat_factor(ser,data,repeat_times)
            logger.info(F"{data} factorization finished with settings {conf}")
            if not all(data%v==0 for v in val):
                logger.error(F"Output value {val} have problem!!!")
            outputfile.write(F"{data},{np.mean(count)},{np.std(count)},{','.join(map(str,count))}\n")
        
        # outputfile.write("\n".join(results))
        outputfile.close()
        logger.info(F"Section {sec_name} finished")

        fmc.stop_factorize(ser)
        fmc.stop_factorize(ser)
        time.sleep(0.1)
        ser.flushInput()
        ser.flushOutput()
        
except Exception as e:
    logger.error(F"Program error occurred at {time.time()}, with the error message {str(e)}")    