//
// Created by xiansongq on 2023/5/5.
//

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm> // std::find
#include <unordered_set>
using namespace std;
/*
 * local plaintext private set intersection
 *
 * */
int main(){
    unordered_set<string> str1;
    unordered_set<string>  str2;
    std::ifstream fin;
    fin.open("D:\\xiansong\\ClionProject\\test\\mytest\\A_PSI_DATA.txt", std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open file A_PSI_DATA" << std::endl;
        return -1;
    }
    std::string line;

    while (std::getline(fin, line) ) {
        std::stringstream stream(line);
        uint64_t a;
        stream >> a;
        str1.insert(to_string(a));

    }
    fin.close();
    std::ifstream fin1;
    fin1.open("D:\\xiansong\\ClionProject\\test\\mytest\\B_PSI_DATA.txt", std::ios::binary);
    if (!fin1) {
        std::cout << "Failed to open file B_PSI_DATA" << std::endl;
        return -1;
    }
    std::string line1;

    while (std::getline(fin1, line1)) {
        std::stringstream stream(line1);
        uint64_t a;
        stream >> a;
        str2.insert(to_string(a));
    }
    fin1.close();
    //输出交集到文件
    std::ofstream fout;
    fout.open("oprf_psi_out.txt", std::ios::out | std::ios::binary);
    if (!fout) {
        std::cerr << "oprf_psi_out.txt open error" << std::endl;
        exit(1);
    }
    cout<<str1.size()<<"  "<<str2.size()<<endl;
    int count=0;
    for(auto a:str1){
        if(str2.count(a)) {
            fout<<a<<endl;
            count++;
        }
    }
    fout.close();
    cout<<count<<endl;
    return 0;

}