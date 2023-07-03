//
// Created by 17579 on 2023/5/22.
//

#include "../../mpc/psi/psi_from_cuckoo_filter.hpp"
#include "../../crypto/setup.hpp"

/*
 * 两方平衡PSI
 * 有 A B C D
 * Receiver 有集合 A C
 * Sender 有集合 B D
 * Receiver 输出交集 A\cap B  C\cap D
 *
 * */

void readfile(std::string filename, std::vector<block> &ans) {
    //从文件读取数据
    std::ifstream fin;
    fin.open(filename, std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open file" << std::endl;
        exit(-1);
    }
    std::string line;
    while (std::getline(fin, line)) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        // 将第二段的 字符串转为整型

        std::string st = "";
        for (auto i = 0; i < tokens.size(); ++i) {
            st += std::to_string(tokens[1][i] - 'a');
        }
        std::stringstream str;
//        std::cout<<tokens[0]<<" "<<st<<" "<<tokens[2]<<std::endl;
//        str <<tokens[0]<<st<<tokens[2];
//        std::cout<<str.str()<<std::endl;
        uint64_t a;
        str >> a;
        ans.push_back(Block::MakeBlock(0LL, a));
    }
    fin.close();
}


int main() {
    CRYPTO_Initialize();
    OTEOPRF::PP pp;
    pp = OTEOPRF::Setup(20, 40);

    //读取数据 四个集合
    std::vector<block> vec_a;
    std::vector<block> vec_b;
    std::vector<block> vec_c;
    std::vector<block> vec_d;
    readfile("A_PSI_DATA_2.txt",vec_a);
    readfile("B_PSI_DATA_2.txt",vec_b);
    readfile("C_PSI_DATA_2.txt",vec_c);
    readfile("D_PSI_DATA_2.txt",vec_d);

    /*必须做数据补全 才能成功执行协议
     * 现在 数据有1000000 条
     * 选择 2^20
     * 那么还差 48576条数据
     * */
    int len = 48576;
    for (int i = 0; i < len; i++)
    {
        vec_a.push_back(Block::MakeBlock(0LL, 0LL));
        vec_b.push_back(Block::MakeBlock(0LL, 0LL));
        vec_c.push_back(Block::MakeBlock(0LL, 0LL));
        vec_d.push_back(Block::MakeBlock(0LL, 0LL));
    }
    std::cout << vec_a.size() << "---" << vec_b.size() << "---" <<vec_c.size() << "---" << vec_d.size() << std::endl;
    std::string party;
    std::cout
            << "please select your role between sender and receiver (hint: first start receiver, then start sender) ==> ";

    std::getline(std::cin, party);
    PrintSplitLine('-');
    if (party == "sender") {
        NetIO client("client", "127.0.0.1", 8090);
        OPRFPSI::Send(client, pp, vec_a);
        OPRFPSI::Send(client, pp, vec_c);

    }
    std::vector<block> vec_intersection_prime;
    if (party == "receiver") {
        NetIO server("server", "", 8090);
        vec_intersection_prime = OPRFPSI::Receive(server, pp, vec_b);
       OPRFPSI::Receive(server, pp, vec_d);
        std::cout<<vec_intersection_prime.size()-len<<std::endl;
        std::cout<<"receiver output intersection to file"<<std::endl;
        //output intersection result to csv file
        std::ofstream fout;
        fout.open("oprf_psi_out_2.csv", std::ios::out | std::ios::binary);
        if (!fout) {
            std::cerr << "oprf_psi_out_2.csv open error" << std::endl;
            exit(1);
        }
        if (fout.is_open()) {
            for (int i = 0; i < vec_intersection_prime.size()-len; i++) {
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
