//
//  parameters.h
//  C_sim
//
//  Created by songyu li on 2025/5/18.
//
#ifndef PARAMETERS_H
#define PARAMETERS_H

#define STRINGIFY(x) #x
#define MVTS(x) STRINGIFY(x)
#define VERSION 0.3
//该文件中定义各类运行中会用到的参数，以方便修改
#define P_test_num 12337337
#define P_output_file_name "S_12337337_no_qutify_SFA.txt"
#define P_repeat_times 100
#define AB_len 12//两个分解出来的位数

#define P_tem_int1 26//如果sigmoid_approx被使能了，那么这里的值需要被减4
#define P_tem_int2 26
#define P_tem_int3 29
#define P_tem_float (6e11)//需要研究这两个参数之间是如何转化的

#define P_Ai 0.1//局域温度参数(1-Ai)*u+top*Ai*bit_now
#define P_top 0.9999//这个结果似乎不能高于1-1e-7的结果

#define P_supress (1-this->bit_now) //this->bit_now or 1-this->bit_now

//这一部分数据需要保持与FPGA中一致
//此处温度的含义是否与FPGA中一致我认为需要校准
#define P_Ai_move1 4//局域温度的精度最高到24位，所以P_top_move不能改
#define P_Ai_move2 5
#define P_top_move1 14
#define P_top_move2 15

//控制系统的运行逻辑
//P_check_every_bit为真时，系统每更新一个bit就会检查一次
//P_qutify_approx控制能量计算器系统是否有量化近似（不影响SFA开关与工作，但是实现不同）
//P_SFA控制系统是否开启SFA优化（与是否有量化近似无关）
//P_sigmoid_approx控制**在能量计算器系统开启近似**的前提下，sigmoid函数是否有量化近似（截断）
//P_approx_max只有**在P_qutify_approx和P_sigmoid_approx**同时为true时才会生效
//P_power_approx只有在**能量计算器系统开启近似**时才能使用（与P_sigmoid_approx是否开启无关），用以验证指数量化

#define P_check_every_bit false
#define P_qutify_approx false
#define P_SFA false//控制是否开启SFA优化
#define P_sigmoid_approx true
#define P_approx_max 127//127:松弛截断 128:端到端截断（也可以用于实验更为松弛的截断方案）(不要改成128）
#define P_power_approx false

#endif


