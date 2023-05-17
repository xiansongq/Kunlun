//
// Created by 17579 on 2023/5/11.
//

#include "../../utility/polynomial.h"
#include "../../crypto/block.hpp"

int main(){
    std::ifstream fin;
    std::vector<uint64_t> vec_y;
    fin.open("B_PIR_DATA.txt", std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open file" << std::endl;
        std::exit(-1);
    }
    std::string line1;

    while (std::getline(fin, line1)) {
        std::vector<std::string> row;
        size_t pos = 0;
        std::string token;
        /* Split each row of data into multiple columns.*/
        while ((pos = line1.find(",")) != std::string::npos) {
            token = line1.substr(0, pos);
            row.push_back(token);
            line1.erase(0, pos + 1);
        }
        row.push_back(line1);
        vec_y.push_back(std::stoul(row[1]));

    }
    fin.close();
    std::vector<uint64_t > vec_x;
    for(auto i=0;i<vec_y.size(); ++i) vec_x.push_back(i);
    std::cout<<vec_x.size()<<"----"<<vec_y.size()<<std::endl;
    std::vector<uint64_t > conf;
    polynomial_from_points(vec_y,vec_x,conf,1024);
    std::cout<<conf.size()<<std::endl;
    return 0;
}
