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
//#include "parameters.h"
#include "read_paras.h"
#include <time.h>
#include <sys/stat.h>
#define MAX_AB_LEN 16
#define MAX_REPEAT 10000

using namespace std;

p_bit_infos prepare_one_num(string* value,stringstream lss)
{
    p_bit_infos ret;
    ret.version=value[0];
    ret.supress_type=stoi(value[4]);
    ret.check_every_bit=(bool)stoi(value[5]);
    ret.quitfy=(bool)stoi(value[6]);
    ret.sfa=(bool)stoi(value[7]);
    ret.sigmoid_approx=(bool)stoi(value[8]);
    ret.power_approx=(bool)stoi(value[10]);
    if(ret.quitfy==false&&ret.sigmoid_approx==true)
    {
        cout<<"WARNING, quitfy is not open, sigmoid_cut won't work!"<<endl;
    }
    ret.approx_max=stoi(value[9]);
    
    string back_data;
    getline(lss,back_data,',');
    ret.test_num=stoi(back_data);
    ret.output_length=(int)(((int)log2(ret.test_num)+2)/2);
    getline(lss,back_data,',');
    ret.fback_temp=stod(back_data);
    for(int i=0;i<3;i++)
    {
        getline(lss,back_data,',');
        ret.iback_temp[i]=stoi(back_data);
    }
    getline(lss,back_data,',');
    ret.fAi=stod(back_data);
    for(int i=0;i<2;i++)
    {
        getline(lss,back_data,',');
        ret.iAi[i]=stoi(back_data);
    }
    getline(lss,back_data,',');
    ret.fregion_top=stod(back_data);
    for(int i=0;i<2;i++)
    {
        getline(lss,back_data,',');
        ret.iregion_top[i]=stoi(back_data);
    }
    return ret;
};


float one_num(int repeat,int64_t* output_data,p_bit_infos info)
{
    float back=0;
    for(int i=0;i<repeat;i++)
    {
        //First initial all the p-bits
        p_bit A[MAX_AB_LEN-1];
        p_bit B[MAX_AB_LEN-1];
        for (int i=0;i<MAX_AB_LEN-1;i++)
        {
            A[i]=p_bit(i+1,info.output_length*2,info);
            B[i]=p_bit(i+1,info.output_length*2,info);
        }
        uint64_t ans_now=0;
        uint64_t step=0;
        while(true){
            for(int i=0;i<info.output_length-1;i++)
            {
                int64_t x0=get_X(A,info.output_length);
                int64_t y0=get_X(B,info.output_length);
                
                int64_t N_XYY=(info.test_num-x0*y0)*y0;
                int64_t Y2=y0*y0;
                if(N_XYY==0&&info.check_every_bit)
                {
                    ans_now=x0;
                    goto ready;
                }
                A[i].refresh_bit(N_XYY, Y2, true);
            }
            for(int i=0;i<info.output_length-1;i++)
            {
                int64_t y0=get_X(A,info.output_length);
                int64_t x0=get_X(B,info.output_length);
                
                int64_t N_XYY=(info.test_num-x0*y0)*y0;
                int64_t Y2=y0*y0;
                if(N_XYY==0&&info.check_every_bit)
                {
                    ans_now=x0;
                    goto ready;
                }
                B[i].refresh_bit(N_XYY,Y2,true);
            }
            int64_t check_x=get_X(A,info.output_length);
            int64_t check_y=get_X(B,info.output_length);
            if((info.test_num-check_x*check_y==0))
            {
                ans_now=check_x;
                goto ready;
            }
            step++;
        }
        ready:cout<<"Get answer:"<<ans_now<<endl;
        back+=(float)step/(float)repeat;
        output_data[i]=step;
    }
    return back;
}


int main(int argc, const char * argv[]) {
    string profile_name[12];
    string profile_val[12];
    string input_profile_name="/Users/songyuli/Library/CloudStorage/OneDrive-Personal/OtherProj/pB/C_sim/C_sim/C_sim/paras.csv";
    read_config(input_profile_name,profile_name,profile_val);
    //prepare output folder
    time_t raw_time;
    struct tm *time_info;
    char time_str[40];
    time(&raw_time);
    time_info=localtime(&raw_time);
    strftime(time_str, sizeof(time_str), "%Y%m%d%H-%M-%S", time_info);
    string output_folder_name;
    output_folder_name=profile_val[2]+string(time_str);
    mkdir(output_folder_name.c_str(),0777);
    copy_config(input_profile_name, output_folder_name);
    string output_data_name=output_folder_name+"/data.csv";
    //input file
    ifstream data_input_file(profile_val[1]);
    ofstream data_output_file(output_data_name);
    string line;
    int repeat_times=stoi(profile_val[3]);
    while (getline(data_input_file,line)) {
        if(line.empty()||line[0]=='#')
            continue;
        else
        {
            p_bit_infos info;
            info=prepare_one_num(profile_val, stringstream(line));
            int64_t back_count[MAX_REPEAT];
            float mean=one_num(repeat_times, back_count, info);
            data_output_file<<info.test_num<<","<<repeat_times<<",-,";
            if(info.quitfy==true)
            {
                //Only output quitfy info
                data_output_file<<info.iback_temp[0]<<","<<
                info.iback_temp[1]<<","<<info.iback_temp[2]<<",-,";
                if(info.sfa)
                    data_output_file<<info.iAi[0]<<","<<info.iAi[1]<<","<<info.iregion_top[0]<<","<<info.iregion_top[1]<<",-,";
            }
            else
            {
                //Only output float info
                data_output_file<<info.fback_temp<<",-,";
                if(info.sfa)
                    data_output_file<<info.fAi<<","<<info.fregion_top<<",-,";
            }
            //Output mean and other stuff
            data_output_file<<mean;
            for(int i=0;i<repeat_times;i++)
                data_output_file<<","<<back_count[i];
            data_output_file<<endl;
            cout<<"One line complete!"<<mean<<endl;

        }
    }
    data_input_file.close();
    return 0;
}
