//
//  main.cpp
//  C_sim
//
//  Created by songyu li on 2025/1/1.
//


#include <iostream>
#include <fstream>
#include <cstdlib>
#include "p_bit.h"
#include <chrono>//这个代码用于计时
#include "parameters.h"

using namespace std;

//uint64_t seed_values[28]={
//    20,21,22,23,24,25,26,27,28,29,30,31,32,33,
//    40,41,42,43,44,45,46,47,48,49,50,51,52,53
//    
//};

int64_t update_NXYY(int64_t N_XYY_o,int64_t Y2,int k,int s)
{
    int64_t y2k=(Y2<<k);
    return N_XYY_o-(1-s)*y2k;
}

uint64_t one_batch(int64_t N,bool process_yuan,int c)
{
    p_bit A[AB_len-1];
    for(int i=0;i<AB_len-1;i++)
        A[i]=p_bit(i+1, AB_len*2,process_yuan);//注意修改温度
    p_bit B[AB_len-1];
    for(int i=0;i<AB_len-1;i++)
        B[i]=p_bit(i+1, AB_len*2,process_yuan);

    uint64_t ans=0;
    uint64_t step=0;
    while (true) {
//        分为三步：1.检查 2.更新A 3.更新B
        for(int i=0;i<AB_len-1;i++)
        {
            int64_t x0=get_X(A, AB_len);
            int64_t y0=get_X(B,AB_len);

            int64_t N_XYY=(N-x0*y0)*y0;
            int64_t Y2=y0*y0;
//            if((N%x)==0||(N%y)==0)
            if(N_XYY==0&&P_check_every_bit)
            {
                ans=x0;
                goto ready;
            }
            A[i].refresh_bit(N_XYY, Y2, true);
        }
        for(int i=0;i<AB_len-1;i++)
        {
            int64_t y0=get_X(A, AB_len);
            int64_t x0=get_X(B,AB_len);

            int64_t N_XYY=(N-x0*y0)*y0;
            int64_t Y2=y0*y0;
            if(N_XYY==0&&P_check_every_bit)
            {
                ans=x0;
                goto ready;
            }
            B[i].refresh_bit(N_XYY,Y2,true);
        }
        int64_t check_x=get_X(A,AB_len);
        int64_t check_y=get_X(B,AB_len);
        if((N-check_x*check_y==0))
        {
            ans=check_x;
            goto ready;
        }
        step++;
    }
ready:  cout<<"Get answer:"<<ans<<endl;
    return step;
}

void prepare_data_info(char* info_str)
{
    sprintf(info_str,"###VERSION%s INFO\n###%s %s %s ||%s %s %s %s|%s %s %s %s %s %s\n###P_qutify_approx  %s\n###P_SFA %s\n###sigmoid_approx %s\n###approx_max %s\n###power_approx %s",
            MVTS(VERSION),
            MVTS(P_test_num),MVTS(P_repeat_times),MVTS(AB_len),
            MVTS(P_tem_float),MVTS(P_tem_int1),MVTS(P_tem_int2),MVTS(P_tem_int3),
            MVTS(P_Ai),MVTS(P_top),MVTS(P_Ai_move1),MVTS(P_Ai_move2),MVTS(P_top_move1),MVTS(P_top_move2),
            MVTS(P_qutify_approx),MVTS(P_SFA),MVTS(P_sigmoid_approx),MVTS(P_approx_max),MVTS(P_power_approx));
    return;
}

int main(int argc, const char * argv[]) {
    cout<<"RAND max:"<<RAND_MAX<<endl;
    cout<<"TEST MESSAGE!!"<<endl;
    srand(static_cast<unsigned int>(time(0)));
    uint64_t steps=0;
    double step_mean=0;
    ofstream file_out(P_output_file_name);
    char info_data[300];
    prepare_data_info(info_data);
    file_out<<info_data<<endl;
    
    if (!file_out)
        return 1;
    for(int i=0;i<P_repeat_times;i++)
    {
        steps=one_batch(P_test_num, true,i);
        file_out<<steps<<endl;
        cout<<"Iteration:"<<steps<<"\tFactor count:"<<i<<endl;
        step_mean+=(double)steps/P_repeat_times;
    }
    file_out.close();
    cout<<"Mean:"<<step_mean<<endl;
    return 0;
}
