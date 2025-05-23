//
//  p_bit.cpp
//  C_sim
//
//  Created by songyu li on 2025/1/1.
//
#include "p_bit.h"
//#include "parameters.h"


int64_t p_bit::get_Ik1_int(int64_t NXY_Y, int64_t Y2)
{

    //这里不再使用移位法，而是用更加直接的除法（除法使用常见的可以移位的东西计算，以保证可实现性）
    
    int64_t s1=(NXY_Y<<(this->k+1));
    int64_t s2=(Y2<<(2*(this->k)));
    int64_t Ik1;
    
    if(this->bit_now==1)
    {
        Ik1=s1+s2;
    }
    else
    {
        Ik1=s1-s2;
    }
    Ik1=(Ik1>>(this_info.iback_temp[0]-4))+(Ik1>>(this_info.iback_temp[1]-4))+(Ik1>>(this_info.iback_temp[2]-4));
    if(this_info.sigmoid_approx==false)
        Ik1=Ik1>>4;
    uint32_t addvalue=(1<<(24-this_info.iAi[0]))+(1<<(24-this_info.iAi[1]));
    int p_supress;
    if(this_info.supress_type==0)
        p_supress=1-this->bit_now;
    else
        p_supress=this->bit_now;
    if(this_info.sfa)
        uAi=uAi-(uAi>>this_info.iAi[0])-(uAi>>this_info.iAi[1])+(addvalue-(addvalue>>this_info.iregion_top[0])-(addvalue>>this_info.iregion_top[0]))*p_supress;
    else
        uAi=0;//不开启SFA优化
    if(this_info.power_approx)
    {
        //使用指数近似
        Ik1=Ik1<<((int)log2((1<<24)-uAi));
        Ik1=(Ik1>>24);
    }
    else
    {
        //不使用指数近似（方便起见，使用作弊的double做法）
        double fuAi=(double)((1<<24)-uAi)/(1<<24);
        Ik1=(int64_t)((double)Ik1*fuAi);
    }
    int16_t bak;
    if(this_info.sigmoid_approx)
    {
        if(Ik1>this_info.approx_max)
            bak=(int16_t)this_info.approx_max;
        else if (Ik1<-this_info.approx_max)
            bak=(int16_t)(-this_info.approx_max);
        else
            bak=(int16_t)Ik1;
        return bak;
    }
    else
    {
        return Ik1;
    }
}

double p_bit::get_Ik1_double(int64_t NXY_Y,int64_t Y2)
{
    double s1=(NXY_Y<<(this->k+1));
    double s2=(Y2<<(2*(this->k)));
    double Ik1;
    int p_supress;
    if(this_info.supress_type==0)
        p_supress=1-this->bit_now;
    else
        p_supress=this->bit_now;
    if(this->bit_now==1)
    {
        Ik1=s1+s2;
    }
    else
    {
        Ik1=s1-s2;
    }
    Ik1=Ik1/this_info.fback_temp;
    if(this_info.sfa)
        dAi=dAi*(1-this_info.fAi)+this_info.fregion_top*p_supress*this_info.fAi;
    else
        dAi=0;
    return Ik1*(1-dAi);
}

int16_t p_bit::get_inverse_sigmoid(uint16_t rand)
{
    //在最新的实现方式中，我们是采用了反函数的形式
    double nrand=(double)(rand%2049)
    ;//此处处理了一下边界防止溢出
    //此处不要修改！（与是否为松弛截断无关）
    double inv=log(2048/nrand-1)*16;//@@@@@注意：这里针对的也是乘了16的情况
    if(inv>127)
        inv=127;
    else if(inv<-128)
        inv=-128;
    return (int16_t)inv;
}

int p_bit::refresh_bit(int64_t NXY_Y, int64_t Y2,bool inverse=false)
{
    int bak_s=1;
    //完成对p-bit的一次更新
    uint16_t this_rand=rand()%65535;
    double sigmoid_input=1e10;
    if(this_info.quitfy)
    {
        int64_t Ik1=this->get_Ik1_int(NXY_Y, Y2);
        if(this_info.sigmoid_approx)
        {
            int16_t rand_sig_inv=this->get_inverse_sigmoid(this_rand);
            if(Ik1>rand_sig_inv)
            {
                if(this->bit_now==0)
                    bak_s=0;
                this->bit_now=1;
            }
            else
            {
                if(this->bit_now==1)
                    bak_s=2;
                this->bit_now=0;
            }
        }
        else{
            //直接把结果塞到sigmoid输入中
            sigmoid_input=Ik1;
        }
    }
    else
    {
        sigmoid_input=this->get_Ik1_double(NXY_Y, Y2);
    }
    if((this_info.quitfy==false)||(this_info.sigmoid_approx==false))
    {
        //使用传统的sigmoid（浮点法）进行计算
        if(abs(sigmoid_input-1e10)<0.001)
            cout<<"WARNING p_bit ERROR!!!"<<endl;
        double sigmoid_out=1/(1+exp(-sigmoid_input));//需要检查是否正确
        double rand_01=(double)this_rand/65535;
        if(sigmoid_out>rand_01)
        {
            if(this->bit_now==0)
                bak_s=0;
            this->bit_now=1;
        }
        else
        {
            if(this->bit_now==1)
                bak_s=2;
            this->bit_now=0;
        }
    }
    return bak_s;
}


uint64_t get_X(p_bit* ps,uint16_t n)
{
    uint64_t x_now=1;
    for(uint32_t i=0;i<n-1;i++)
    {
        uint64_t nval=((ps+i)->bit_now)<<(i+1);
        x_now+=nval;
    }
    return x_now;
}


