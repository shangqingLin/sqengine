#include "number_to_string.h"
#include <stdio.h>

void floatToString(float n,std::string& result){
    char data[100];
    sprintf(data, "%f",n);
    result.append(data);
}

void charBufferToFloatString(char* buffer,int num,std::string& result){
    char data[100];
    int size = sizeof(float);
    for(int i = 0; i < num ; ++i){
        sprintf(data, "%f",*(float*)(buffer + i * size));
        result.append(data);
        if(i < num - 1){
            result.append(",");
        }
    }
}