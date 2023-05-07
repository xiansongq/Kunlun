//
// Created by 17579 on 2023/5/7.
//


#include "../../include/global.hpp"

int main(){
#pragma omp parallel for num_threads(thread_count)
    for(int i=0;i<10;i++){
        std::cout<<i<<" "<<"Hello "<<std::endl;
    }
}