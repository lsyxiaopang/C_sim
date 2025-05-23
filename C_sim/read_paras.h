//
//  read_paras.h
//  C_sim
//
//  Created by songyu li on 2025/5/22.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <algorithm>

//*Change this when changing version
#define info_len 100
using namespace std;

int read_config(string file_name,string* output_config_names,string* output_config_values);
void copy_config(string file_name,string dir_name);
