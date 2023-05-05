//
// Created by 17579 on 2023/5/4.
//

#include "../../mpc/psi/psi_from_oprf.hpp"
#include "../../crypto/setup.hpp"
#include "../../mpc/oprf/ote_oprf.hpp"
#include "../../crypto/prg.hpp"
#include "../../mpc/ot/alsz_ote.hpp"

int main() {
    CRYPTO_Initialize();
    std::cout << "oprf psi is begings >>>" << std::endl;
    PrintSplitLine('-');
    OTEOPRF::PP pp;
    pp = OTEOPRF::Setup(20, 40);

//    std::vector<block> vecx;
//    //从文件读取数据
//    std::ifstream fin;
//    fin.open("A_PSI_DATA.txt", std::ios::binary);
//    if (!fin) {
//        std::cout << "Failed to open file" << std::endl;
//        return -1;
//    }
//    std::string line;
////#pragma omp parallel num_threads(thread_count)
//    while (std::getline(fin, line) ) {
//        std::stringstream stream(line);
//        uint64_t a;
//        stream >> a;
//        vecx.push_back(Block::MakeBlock(0LL, a));
//
//    }
//    fin.close();
//    std::vector<block> vecy;
//    std::ifstream fin1;
//    fin1.open("B_PSI_DATA.txt", std::ios::binary);
//    if (!fin1) {
//        std::cout << "Failed to open file" << std::endl;
//        return -1;
//    }
//    std::string line1;
////#pragma omp parallel num_threads(thread_count)
//    while (std::getline(fin1, line1)) {
//        std::stringstream stream(line1);
//        uint64_t a;
//        stream >> a;
//        vecy.push_back(Block::MakeBlock(0LL, a));
//    }
//    fin1.close();
//
//    /*必须做数据补全 才能成功执行协议
//     * 现在 数据有1000000 条
//     * 选择 2^20
//     * 那么还差 48576条数据
//     * */
//    int len = 48576;
////#pragma omp parallel for num_threads(thread_count)
//    for (int i = 0; i < len; i++) vecx.push_back(Block::MakeBlock(0LL, 0LL));
////#pragma omp parallel for num_threads(thread_count)
//    for (int i = 0; i < len; i++) vecy.push_back(Block::MakeBlock(0LL, 0LL));
//    std::cout << vecx.size() << "---" << vecy.size() << std::endl;


    PRG::Seed seed = PRG::SetSeed(fixed_seed, 0); // initialize PRG seed
    //std::vector<uint8_t> vec_selection_bit = GenRandomBits(seed, pp.LEN);
    std::vector<block> vec_x = PRG::GenRandomBlocks(seed, pp.LEN);
    std::vector<block> vec_y = PRG::GenRandomBlocks(seed, pp.LEN);
    vec_y[1]=vec_x[10];
    vec_y[5]=vec_x[14];
    vec_y[7]=vec_x[6];
    vec_y[9]=vec_x[8];
//    std::cout<<std::to_string(Block::BlockToInt64(vec_y[1]))<<std::endl;
//    std::cout<<std::to_string(Block::BlockToInt64(vec_y[5]))<<std::endl;
//    std::cout<<std::to_string(Block::BlockToInt64(vec_y[7]))<<std::endl;
//    std::cout<<std::to_string(Block::BlockToInt64(vec_y[9]))<<std::endl;
    Block::PrintBlock(vec_y[1]);
    Block::PrintBlock(vec_y[5]);
    Block::PrintBlock(vec_y[7]);
    Block::PrintBlock(vec_y[9]);
    Block::PrintBlock(vec_x[563866]);
    Block::PrintBlock(vec_y[563866]);

    std::string party;
    std::cout
            << "please select your role between sender and receiver (hint: first start receiver, then start sender) ==> ";


    std::getline(std::cin, party);
    PrintSplitLine('-');
    if (party == "sender") {
        NetIO client("client", "127.0.0.1", 8090);
        OPRFPSI::Send(client, pp, vec_x);

    }
    std::vector<block> vec_intersection_prime;
    if (party == "receiver") {
        NetIO server("server", "", 8090);
        vec_intersection_prime = OPRFPSI::Receive(server, pp, vec_y);

        std::cout<<vec_intersection_prime.size()<<std::endl;
        Block::PrintBlocks(vec_intersection_prime);
        std::cout<<"receiver output intersection to file"<<std::endl;
        //输出交集到文件
        std::ofstream fout;
        fout.open("oprf_psi_out.txt", std::ios::out | std::ios::binary);
        if (!fout) {
            std::cerr << "oprf_psi_out.txt open error" << std::endl;
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
    }

    CRYPTO_Finalize();
    return 0;

}