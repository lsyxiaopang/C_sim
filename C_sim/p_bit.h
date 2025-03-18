//
//  p_bit.h
//  C_sim
//
//  Created by songyu li on 2025/1/1.
//
#include <cstdlib>
#include <math.h>
#include <iostream>
using namespace std;

class p_bit
{
public:
    uint8_t bit_now;//标记当前的pBit为1/0
    uint8_t k;//标记这个pBit所在的位
    uint8_t n;//系统总共有的位数（用来带入计算公式）
    
    //A（指数）的表达式为A=(1-uAi)*Ene/tem
    
    //&&&&&&&&&&&&&&&&&&&&&&&&&以下参数记得修改
    float Ai=0.1;//局域温度参数，局域温度的更新满足规则：uAi=(1-Ai)*uAi+Ai*bit_now//&&&&&
//    float tem=1e10;//背景温度值//&&&&&使用时记得修改//已经停用
    int tem_m[3]={30,31,32};//基于移位给出的温度值（包含乘的16）//&&&&使用时记得修改
    uint64_t random_val;
    //&&&&&&&&&&&&&&&&&&&&&&&&&一下参数记得修改
    
    int uAi=0;//局域温度值
    bool process_yuan;//判断是否用局域温度的方法来计算Ik
    
    void refresh_bit(int64_t NXY_Y,int64_t Y2,bool inverse);//从已有的数据更新这个pbit的数据
    
    uint32_t lfsr_generate();
    
    p_bit(int kv,int nv,uint64_t random_seed,bool pyuan=true):bit_now(0),k(kv),n(nv),random_val(random_seed),process_yuan(pyuan){};//初始化pBit系统，可以初始化当前pbit所在的位置
    //@@@@@注意：温度和局域温度参数需要在前文进行修改！！！

    p_bit():bit_now(0),k(0),n(0),process_yuan(false){};
    
private:
    int8_t get_inverse_sigmoid(uint16_t rand);//验证---当前所使用的随机数处理方法（将11位随机数变化为8位值）是否可行
    int8_t get_Ik1(int64_t NXY_Y,int64_t Y2);//从计算出来的两个部分得到Ik1
};

uint64_t get_X(p_bit* p_bitsx,uint8_t n);//将pBit数组整合为一个数字

