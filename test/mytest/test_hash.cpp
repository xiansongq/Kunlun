//
// Created by 17579 on 2023/5/7.
//

#include "../../crypto/hash.hpp"
#include "../../crypto/block.hpp"
#include "../../include/std.inc"

int main(){


/*    std::string str1="990235009163719316";
    auto a=Hash::StringToBlock(str1);
    Block::PrintBlock(a);
    block c=Block::MakeBlock('0LL', Block::BlockToInt64(a));
    Block::PrintBlock(c);
    std::string str2="990235009163719316";
    auto b=Hash::StringToBlock(str2);
    block h=Block::MakeBlock('0LL', Block::BlockToInt64(b));
    Block::PrintBlock(b);
    Block::PrintBlock(h);*/
    std::ifstream fin;
    std::vector<std::vector<std::string>> file_data;
    fin.open("B_PIR_DATA.txt", std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open file" << std::endl;
        std::exit(-1);
    }
    std::string line;
    int i = 0;
    while (std::getline(fin, line)) {
        std::vector<std::string> row;
        row.push_back(std::to_string(i++));   /*add index colunms*/
        size_t pos = 0;
        std::string token;
        /* Split each row of data into multiple columns.*/
        while ((pos = line.find(",")) != std::string::npos) {
            token = line.substr(0, pos);
            row.push_back(token);
            line.erase(0, pos + 1);
        }
        row.push_back(line);
        file_data.push_back(row); /*save data*/

    }
    fin.close();
    std::vector<std::string> keyword;
    std::ifstream fin1;
    fin1.open("A_PIR_ID.txt", std::ios::binary);
    if (!fin1) {
        std::cout << "Failed to open file" << std::endl;
        std::exit(-1);
    }
    std::string line1;

    while (std::getline(fin1, line1, '\n')) {
        if (!line1.empty() && line1[line1.size() - 1] == '\r')
            line1.erase(line1.size() - 1);
        keyword.push_back(line1);
        //index_data.push_back(Block::MakeBlock(0LL, std::stoull(line)));
    }
    fin.close();
    std::cout<<keyword[0]<<std::endl;
    std::cout<<file_data[8][1]<<std::endl;
    (file_data[8][1]==keyword[0])? std::cout<<"1"<<std::endl:std::cout<<"0"<<std::endl;
    Block::PrintBlock(Hash::StringToBlock(keyword[0]));
    Block::PrintBlock(Hash::StringToBlock(file_data[8][1]));





    return 0;
}
