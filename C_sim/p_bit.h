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
    uint8_t bit_now;//实际上这个是一位就足够了
    uint8_t k;//标记这个pbit实际在多少位
    uint8_t n;//系统总共有多少位
    uint8_t Bi=0;//袁的修正中的Ai乘以256的结果
    double Ai=0;//袁的修正，在确认量化前可以的时候先使用这个
    uint32_t uAi=0;
    float tem=6e11;
    bool process_yuan;//采用袁的方法来处理Ik
    bool process_jung;//采用jung的方法处理Ek//TODO 还未实现
    
    void refresh_bit(int64_t NXY_Y,int64_t Y2,bool inverse);//从已有的数据更新这个pbit的数据
    
    p_bit(int kv,int nv,bool pjung=false,bool pyuan=true):bit_now(0),k(kv),n(nv),process_jung(pjung),process_yuan(pyuan){};

    p_bit():bit_now(0),k(0),n(0),process_jung(false),process_yuan(false){};
    
private:
    uint16_t get_sigmoid(int8_t Ik);//从已经归一化的Ik中得到对应的sigmoid值
    int8_t get_inverse_sigmoid(uint16_t rand);//验证---当前所使用的随机数处理方法（将11位随机数变化为8位值）是否可行
    int8_t get_Ik1(int64_t NXY_Y,int64_t Y2);//从计算出来的两个部分得到Ik1
    int8_t get_Ik2(int8_t get_Ik1);//基于袁的方法，对Ik进行再次处理
};

uint64_t get_X(p_bit* p_bitsx,uint8_t n);

