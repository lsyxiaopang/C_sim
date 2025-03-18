//
//  main.cpp
//  C_sim
//
//  Created by songyu li on 2025/1/1.
//

#define AB_len 15//两个分解出来的位数

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "p_bit.h"
#include <chrono>//这个代码用于计时

using namespace std;

uint64_t seed_values[28]={
    20,21,22,23,24,25,26,27,28,29,30,31,32,33,
    40,41,42,43,44,45,46,47,48,49,50,51,52,53
    
};

uint64_t one_batch(int64_t N,bool process_yuan,int c)
{
    p_bit A[AB_len-1];
    for(int i=0;i<AB_len-1;i++)
        A[i]=p_bit(i+1, AB_len*2,seed_values[i]+c,process_yuan);//注意修改温度
    p_bit B[AB_len-1];
    for(int i=0;i<AB_len-1;i++)
        B[i]=p_bit(i+1, AB_len*2,seed_values[i+14]+c,process_yuan);

    uint64_t ans=0;
    uint64_t step=0;
    while (true) {
//        分为三步：1.检查 2.更新A 3.更新B
        int64_t x=get_X(A, AB_len);
        int64_t y=get_X(B,AB_len);

        
        for(int i=0;i<AB_len-1;i++)
        {
//            if((N%x)==0||(N%y)==0)
            if(x*y==N)
            {
                ans=x;
                goto ready;
            }
            A[i].refresh_bit((N-x*y)*y, y*y,true);
            x=get_X(A,AB_len);
            y=get_X(B,AB_len);
       }
        for(int i=0;i<AB_len-1;i++)
        {
//            if((N%x)==0||(N%y)==0)
            if(x*y==N)
            {
                ans=x;
                goto ready;
            }
            B[i].refresh_bit((N-x*y)*x, x*x,true);
            x=get_X(A,AB_len);
            y=get_X(B,AB_len);
        }
        step++;
//        if((N%x)==0||(N%y)==0)
    }
ready:  cout<<"Get answer:"<<ans<<endl;
    return step;
}

int main(int argc, const char * argv[]) {
    cout<<"RAND max:"<<RAND_MAX<<endl;
    cout<<"TEST MESSAGE!!"<<endl;
    srand(static_cast<unsigned int>(time(0)));
    uint64_t steps=0;
    ofstream file_out("S_584460367_1e10_0.1_4.txt");
    
    if (!file_out)
        return 1;
    for(int i=0;i<500;i++)
    {
        steps=one_batch(584460367, true,i);
//        steps=one_batch(12337337, true);
//        steps=one_batch(5*7, false);
        file_out<<steps<<endl;
        cout<<"Factor count:"<<i<<endl;
    }
    file_out.close();
//        all_steps+=one_batch(4057*3041,true);
//        all_steps+=one_batch(9792421,true);
//        all_steps+=one_batch(35);
    return 0;
}
