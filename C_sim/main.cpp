//
//  main.cpp
//  C_sim
//
//  Created by songyu li on 2025/1/1.
//

#define AB_len 12//两个分解出来的位数

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "p_bit.h"
#include <chrono>//这个代码用于计时

using namespace std;

uint64_t one_batch(int64_t N,bool process_yuan)
{
    p_bit A[AB_len-1];
    for(int i=0;i<AB_len-1;i++)
        A[i]=p_bit(i+1, AB_len*2,false,process_yuan);//注意修改温度
    p_bit B[AB_len-1];
    for(int i=0;i<AB_len-1;i++)
        B[i]=p_bit(i+1, AB_len*2,false,process_yuan);
//    int64_t N=35317*37493;
//    int64_t N=4057*3041;
    uint64_t ans;
    uint64_t step=0;
    while (true) {
//        分为三步：1.检查 2.更新A 3.更新B
        int64_t x=get_X(A, AB_len);
        int64_t y=get_X(B,AB_len);

        if(x*y==N)
        {
            ans=x;
            break;
        }
        for(int i=0;i<AB_len-1;i++)
        {
            A[AB_len-2-i].refresh_bit((N-x*y)*y, y*y,true);
            x=get_X(A,AB_len);
            y=get_X(B,AB_len);
        }
        
//        x=get_X(B,AB_len);//这里应该权重会更新两次，需要指明
//        y=get_X(A,AB_len);

        for(int i=0;i<AB_len-1;i++)
        {
            B[AB_len-2-i].refresh_bit((N-x*y)*x, x*x,true);
            x=get_X(B,AB_len);//这里应该权重会更新两次，需要指明
            y=get_X(A,AB_len);
        }
        step++;
    }
    cout<<ans<<endl;
    return step;
}

int main(int argc, const char * argv[]) {
    cout<<RAND_MAX<<endl;
    cout<<"TEST MESSAGE!!"<<endl;
//    float a=0;
//    for(int i=0;i<100;i++)
//    {
//        a=a*exp(-0.1)+(1-exp(-0.1));
//        cout<<1-a<<endl;
//    }
    
    srand(static_cast<unsigned int>(time(0)));
    uint64_t steps=0;
    ofstream file_out("lab_rough.txt");
    if (!file_out)
        return 1;
    for(int i=0;i<5000;i++)
    {
        steps=one_batch(4057*3041, false);
//        steps=one_batch(136061, false);
//        steps=one_batch(5*7, false);
        file_out<<steps<<endl;
        cout<<i<<endl;
    }
    file_out.close();
//        all_steps+=one_batch(4057*3041,true);
//        all_steps+=one_batch(9792421,true);
//        all_steps+=one_batch(35);
    return 0;
}
