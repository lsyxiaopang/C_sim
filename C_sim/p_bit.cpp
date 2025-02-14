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
    
    float s1=(float)(NXY_Y<<(this->k+1));
    float s2=(float)(Y2<<(2*(this->k)));
    s1=16*s1/this->tem;//@@@@@注意：此处的数据被乘了16
    s2=16*s2/this->tem;//@@@@@注意：此处的数据被乘了16
    float Ik1;
    
    if(this->bit_now==1)
    {
        Ik1=s1+s2;
    }
    else
    {
        Ik1=s1-s2;
    }
    
    uAi=uAi*(1-Ai)+Ai*this->bit_now;
    if(this->process_yuan)
    {
        Ik1=Ik1*(1-uAi);
    }

    //以下部分是用于检查是否出现越界的情况
    //TODO 在FPGA中研究如何防止该种越界的发生
    int8_t bak;
    if(Ik1>127)
        bak=127;
    else if (Ik1<-127)
        bak=-127;
    else
        bak=(int8_t)Ik1;
    
    return bak;
}

uint16_t p_bit::get_sigmoid(int8_t Ik)
{
    //在FPGA中，这一部分应当是由查找表来实现，所以我在这里就简单地使用float求再量化的方案
    float fIk=Ik;
    float sigmoid_rough=1/(1+exp(-fIk/16));
    //现在这个Sigmoid是0-1的，下一步应该把sigmoid投影到16为无符号上面
    uint16_t out=(uint16_t)(sigmoid_rough*65536);
    return out;
}

int8_t p_bit::get_inverse_sigmoid(uint16_t rand)
{
    //在最新的实现方式中，我们是采用了反函数的形式
    float nrand=(float)(rand%2048);
    float inv=log(2048/nrand-1)*16;
    return (int8_t)inv;
}

void p_bit::refresh_bit(int64_t NXY_Y, int64_t Y2,bool inverse=false)
{
    //完成对p-bit的一次更新
    uint16_t this_rand=rand()%65535;
    int8_t Ik1=this->get_Ik1(NXY_Y, Y2);//首先拿到A的值（已经乘了16）//@@@@@如果不能正常工作，之后改成float计算
    if(!inverse)
    {
        uint16_t sig_out=this->get_sigmoid(Ik1);
        if(this_rand<sig_out)//注意是小于号
            this->bit_now=1;
        else
            this->bit_now=0;
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


