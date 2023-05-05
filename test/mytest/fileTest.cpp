//
// Created by 17579 on 2023/4/7.
//


#include "../../mpc/pso/mqrpmt_psi.hpp"
#include "../../crypto/setup.hpp"
#include "../../mpc/psi/cwprf_psi.hpp"
#include "../../mpc/psi/psi_from_oprf.hpp"

int main() {
    CRYPTO_Initialize();
    mqRPMTPSI::PP pp;
    pp=mqRPMTPSI::Setup("bloom",128, 40, 20, 20);
    //pp = cwPRFPSI::Setup("bloom", 128, 40, 20, 20);
    std::vector<block> vecx;
    //从文件读取数据
    std::ifstream fin;
    fin.open("A_PSI_DATA.txt", std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open file" << std::endl;
        return -1;
    }
    std::string line;
    while (std::getline(fin, line)) {
        std::stringstream stream(line);
        uint64_t a;
        stream >> a;
        vecx.push_back(Block::MakeBlock(0LL, a));
    }
    fin.close();
    std::vector<block> vecy;
    std::ifstream fin1;
    fin1.open("B_PSI_DATA.txt", std::ios::binary);
    if (!fin1) {
        std::cout << "Failed to open file" << std::endl;
        return -1;
    }
    std::string line1;
    while (std::getline(fin1, line1)) {
        std::stringstream stream(line1);
        uint64_t a;
        stream >> a;
        vecy.push_back(Block::MakeBlock(0LL, a));
    }
    fin1.close();

    /*必须做数据补全 才能成功执行协议
     * 现在 数据有1000000 条
     * 选择 2^20
     * 那么还差 48576条数据
     * */
    int len = 48576;
    for (int i = 0; i < len; i++) vecx.push_back(Block::MakeBlock(0LL, 0LL));
    for (int i = 0; i < len; i++) vecy.push_back(Block::MakeBlock(0LL, 0LL));
    std::cout << vecx.size() << "---" << vecy.size() << std::endl;

    std::string party;
    std::cout
            << "please select your role between sender and receiver (hint: first start receiver, then start sender) ==> ";

    std::getline(std::cin, party);
    PrintSplitLine('-');
    std::vector<block> vec_intersection_prime;
    if (party == "sender") {
        NetIO client("client", "127.0.0.1", 8090);
        mqRPMTPSI::Send(client, pp, vecx);
    }

    if (party == "receiver") {
        NetIO server("server", "", 8090);
        vec_intersection_prime = mqRPMTPSI::Receive(server, pp, vecy);
//        std::set<block, BlockCompare> set_diff_result =
//                ComputeSetDifference(vec_intersection_prime, testcase.vec_intersection);
//
//        double error_probability = set_diff_result.size()/double(testcase.vec_intersection.size());
//        std::cout << "mqRPMT-based PSI test succeeds with probability " << (1 - error_probability) << std::endl;
        //输出交集 最后得到的结果应该提出后

    }

    //输出交集到文件
    std::ofstream fout;
    fout.open("out.txt", std::ios::out | std::ios::binary);
    if (!fout) {
        std::cerr << "out.txt open error" << std::endl;
        exit(1);
    }

    if (fout.is_open()) {
        //std::vector<std::uint64_t> buffer(vec_intersection_prime.size() - len);
        // fout<<vec_intersection_prime;
        for (int i = 0; i < vec_intersection_prime.size() - len; i++) {
            //buffer[i] = Block::BlockToInt64(vec_intersection_prime[i]);
            std::uint64_t a = Block::BlockToInt64(vec_intersection_prime[i]);
            //std::stringstream ss;
            //ss<< a <<"  ";
            std::string str= std::to_string(a);
            fout.write(str.c_str(),str.size());
            fout <<std::endl ;
        }

    }
    fout.close();
    std::cout << std::to_string(Block::BlockToInt64(vec_intersection_prime[0])) << std::endl;
    CRYPTO_Finalize();

    return 0;


}