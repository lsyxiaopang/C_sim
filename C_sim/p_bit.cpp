//
//  p_bit.cpp
//  C_sim
//
//  Created by songyu li on 2025/1/1.
//
#include "p_bit.h"



int8_t p_bit::get_Ik1(int64_t NXY_Y, int64_t Y2)
{

    //这里不再使用移位法，而是用更加直接的除法（除法使用常见的可以移位的东西计算，以保证可实现性）
    
    int64_t s1=(NXY_Y<<(this->k+1));
    int64_t s2=(Y2<<(2*(this->k)));
    s1=(s1>>tem_m[0])+(s1>>tem_m[1])+(s1>>tem_m[2]);//@@@@@注意：此处的数据被乘了16
    s2=(s2>>tem_m[0])+(s2>>tem_m[1])+(s2>>tem_m[2]);//@@@@@注意：此处的数据被乘了16
    int64_t Ik1;
    
    if(this->bit_now==1)
    {
        Ik1=s1+s2;
    }
    else
    {
        Ik1=s1-s2;
    }
//    uAi=uAi*(1.0-Ai)+Ai*(1-this->bit_now)*0.9999;//此为在float版本下的数据
    int addvalue=(1<<20)+(1<<19);
    uAi=uAi-(uAi>>4)-(uAi>>5)+(addvalue-(addvalue>>14)-(addvalue>>15))*(1-this->bit_now);
    if(this->process_yuan)
    {
        Ik1=Ik1*((1<<24)-uAi);
        Ik1=(Ik1>>24);
    }

    //以下部分是用于检查是否出现越界的情况
    //TODO 在FPGA中研究如何防止该种越界的发生
    int8_t bak;
    if(Ik1>126)
        bak=126;
    else if (Ik1<-126)
        bak=-126;
    else
        bak=(int8_t)Ik1;
    
    return bak;
}

int8_t p_bit::get_inverse_sigmoid(uint16_t rand)
{
    //在最新的实现方式中，我们是采用了反函数的形式
    float nrand=(float)(rand%32767);
    float inv=log(32767/nrand-1)*16;//@@@@@注意：这里针对的也是乘了16的情况
    return (int8_t)inv;
}

void p_bit::refresh_bit(int64_t NXY_Y, int64_t Y2,bool inverse=false)
{
    //完成对p-bit的一次更新
    uint16_t this_rand=rand()%65535;
    int8_t Ik1=this->get_Ik1(NXY_Y, Y2);//首先拿到A的值（已经乘了16）
    if(!inverse)
    {
        cout<<"ERROR!";
    }
    else
    {
        int8_t rand_sig_inv=this->get_inverse_sigmoid(this_rand);
        if(Ik1>rand_sig_inv)
            this->bit_now=1;
        else
            this->bit_now=0;
    }
}

uint64_t get_X(p_bit* ps,uint8_t n)
{
    uint64_t x_now=1;
    for(int i=0;i<n-1;i++)
    {
        uint64_t nval=((ps+i)->bit_now)<<(i+1);
        x_now+=nval;
    }
    return x_now;
}


