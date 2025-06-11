VERSION="0.0"
import tomllib
from loguru import logger

toml_name="FPGACodeResults/FPGACtrlCode/testinfo.toml"
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
logger.info("Read config finished!")

#*系统连接FPGA
import FactorizationMachineConnector as fmc 
import numpy as np
ser=fmc.init_sys(fpga_serial)
logger.info("FPGA connected")

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
    logger.info(F"Read {datas.shape[0]} data")
    repeat_times=sec_data["repeat"]
    results=[]
    for data in datas:
        val,count,conf=fmc.repeat_factor(ser,data,repeat_times)
        logger.info(F"{data} factorization finished with settings {conf}")
        if not all(data%v==0 for v in val):
            logger.error(F"Output value {val} have problem!!!")
        results.append(F"{data},{np.mean(count)},{np.std(count)},{",".join(map(str,count))}")
    
    outputfile=open(output_base+sec_data["output_csv"],"w")
    outputfile.write("\n".join(results))
    outputfile.close()
    logger.info(F"Section {sec_name} finished")

    fmc.stop_factorize(ser)
    fmc.stop_factorize(ser)
    
    