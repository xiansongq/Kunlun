//
// Created by 17579 on 2023/6/5.
//


#include "../../include/std.inc"
#include <map>

void readfile(std::vector<std::string> &data, std::string filename) {
    //从文件读取数据
    std::ifstream fin;
    fin.open(filename, std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open file" << std::endl;
        exit(-1);
    }
    std::string line;
    while (std::getline(fin, line)) {
        data.push_back(line);

    }
}

void outputdata(std::vector<std::string> &data) {
    std::map<std::string, size_t> mp = {
            {"B_PSI_DATA_2_8.txt",  std::pow(2,8)},
            {"B_PSI_DATA_2_10.txt", std::pow(2,10)},
            {"B_PSI_DATA_2_12.txt", std::pow(2,12)},
            {"B_PSI_DATA_2_16.txt", std::pow(2,16)},
            {"B_PSI_DATA_2_18.txt", std::pow(2,18)}
    };
    // 遍历 map 并输出键值对
    for (const auto &entry: mp) {
        std::ofstream fout;
        fout.open(entry.first, std::ios::out | std::ios::binary);
        if (!fout) {
            std::cerr << entry.first << std::endl;
            exit(1);
        }
        if (fout.is_open()) {
            for (int i = 0; i < entry.second; i++) {
                fout << data[i] << std::endl;
            }
        }
        fout.close();
    }

}

int main() {
    std::string filename;
    std::cout
            << "please input filename ==> ";

    std::getline(std::cin, filename);
    std::vector<std::string> data;
    readfile(data, filename);
    outputdata(data);
    return 0;

}