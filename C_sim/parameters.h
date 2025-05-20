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
#define VERSION 0.2
//该文件中定义各类运行中会用到的参数，以方便修改
#define P_test_num 12337337
#define P_output_file_name "S_12337337_no_qutify_SFA.txt"
#define P_repeat_times 5000
#define AB_len 12//两个分解出来的位数

#define P_tem_int1 26//如果sigmoid_approx被使能了，那么这里的值需要被减4
#define P_tem_int2 26
#define P_tem_int3 29
#define P_tem_float (5e8)//需要研究这两个参数之间是如何转化的

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
#define P_qutify_approx false
#define P_SFA true//控制是否开启SFA优化
#define P_sigmoid_approx true
#define P_approx_max 128//127:松弛截断 128:端到端截断（也可以用于实验更为松弛的截断方案）
#define P_power_approx false

#endif


