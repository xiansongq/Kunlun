//
// Created by 17579 on 2023/5/10.
//

#include "../../mpc/pir/keywordpir.hpp"
#include "../../crypto/setup.hpp"
int main(){
    CRYPTO_Initialize();
    keywordPIR::PP pp;
    pp=keywordPIR::Setup("bloom",40,1000000,10);
    std::ifstream fin1;
    std::vector<block> vec_x;
    fin1.open("A_PIR_ID.txt", std::ios::binary);
    if (!fin1) {
        std::cout << "Failed to open file" << std::endl;
        std::exit(-1);
    }
    std::string line;
    int i = 0;
    while (std::getline(fin1, line, '\n')) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        vec_x.push_back(Block::MakeBlock('0LL',std::stoull(line)));
    }
    fin1.close();

    std::ifstream fin;
    std::vector<std::vector<std::string>> file_data;
    std::vector<block> vec_y;
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
        vec_y.push_back(Block::MakeBlock('0LL',std::stoull(row[1])));
        file_data.push_back(row); /*save data*/
    }
    fin.close();
    std::string party;
    std::cout
            << "please select your role between sender and receiver (hint: first start receiver, then start sender) ==> ";

    std::getline(std::cin, party);
    PrintSplitLine('-');
    if (party == "sender") {
        NetIO client("client", "127.0.0.1", 8090);
        keywordPIR::Server(client, pp, vec_y);

    }
    std::vector<block> vec_intersection_prime;
    if (party == "receiver") {
        NetIO server("server", "", 8090);
        vec_intersection_prime = keywordPIR::Client(server, pp, vec_x);
        //std::cout<<vec_intersection_prime.size()-len<<std::endl;
        std::cout<<"receiver output intersection to file"<<std::endl;
        //output intersection result to csv file
        std::ofstream fout;
        fout.open("keywordpir.csv", std::ios::out | std::ios::binary);
        if (!fout) {
            std::cerr << "keywordpir.csv open error" << std::endl;
            exit(1);
        }
        if (fout.is_open()) {
            for (int i = 0; i < vec_intersection_prime.size(); i++) {
                std::uint64_t a = Block::BlockToInt64(vec_intersection_prime[i]);
                std::string str= std::to_string(a);
                fout.write(str.c_str(),str.size());
                fout <<std::endl ;
            }
        }
        fout.close();
        std::cout<<"output intersection to file success"<<std::endl;
    }

    CRYPTO_Finalize();
    return 0;


}
