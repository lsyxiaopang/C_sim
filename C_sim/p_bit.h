//
//  p_bit.h
//  C_sim
//
//  Created by songyu li on 2025/1/1.
//
#include <cstdlib>
#include <math.h>
#include <iostream>
#include "parameters.h"
using namespace std;

class p_bit
{
public:
    uint16_t bit_now;//标记当前的pBit为1/0
    uint16_t k;//标记这个pBit所在的位
    uint16_t n;//系统总共有的位数（用来带入计算公式）
    
    //A（指数）的表达式为A=(1-uAi)*Ene/tem
    
    //&&&&&&&&&&&&&&&&&&&&&&&&&以下参数记得修改
    float Ai=P_Ai;//局域温度参数，局域温度的更新满足规则：uAi=(1-Ai)*uAi+Ai*bit_now//&&&&&
    float tem_float=P_tem_float;//背景温度值
    int tem_m[3]={P_tem_int1,P_tem_int2,P_tem_int3};//基于移位给出的温度值（包含乘的16）
    //&&&&&&&&&&&&&&&&&&&&&&&&&一下参数记得修改
    
    int uAi=0;
    float fAi=0;
    bool process_yuan;//判断是否用局域温度的方法来计算Ik
    
    int refresh_bit(int64_t NXY_Y,int64_t Y2,bool inverse);//从已有的数据更新这个pbit的数据 并且返回更新信息
    
    uint32_t lfsr_generate();
    
    p_bit(int kv,int nv,bool pyuan=true):bit_now(0),k(kv),n(nv),process_yuan(pyuan){};//初始化pBit系统，可以初始化当前pbit所在的位置
    p_bit():bit_now(0),k(0),n(0),process_yuan(false){};
    
private:
    int16_t get_inverse_sigmoid(uint16_t rand);
    int64_t get_Ik1_int(int64_t NXY_Y,int64_t Y2);//从计算出来的两个部分得到Ik1
    float get_Ik1_float(int64_t NXY_Y,int64_t Y2);//从计算出来的两个部分得到Ik1
};

uint64_t get_X(p_bit* p_bitsx,uint16_t n);//将pBit数组整合为一个数字

