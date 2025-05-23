//
//  read_paras.cpp
//  C_sim
//
//  Created by songyu li on 2025/5/22.
//

#include"read_paras.h"

int read_config(string file_name,string* output_config_names,string* output_config_values)
{
    ifstream file_in(file_name);
    string line;
    int data_now=0;
    while(getline(file_in,line))
    {
//        cout<<line<<endl;
        if(line.empty()||line[0]=='#')
        {
            continue;
        }
        stringstream ss(line);
        getline(ss,output_config_names[data_now],',');
        getline(ss,output_config_values[data_now],',');
        data_now++;
    }
    file_in.close();
    return data_now;
}

void copy_config(string file_name,string dir_name)
{
    ifstream file_in(file_name);
    ofstream file_out(dir_name+"/info.csv");
    string line;
    while(getline(file_in,line))
    {
        file_out<<line<<endl;
    }
    file_in.close();
    file_out.close();
}
