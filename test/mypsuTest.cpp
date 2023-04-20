//
// Created by 17579 on 2023/4/7.
//


#include "../mpc/pso/mqrpmt_psu.hpp"
#include "../crypto/setup.hpp"

int main() {
    CRYPTO_Initialize();

    std::cout << "-----------psu----------" << std::endl;
    mqRPMTPSU::PP pp;
    pp = mqRPMTPSU::Setup("bloom", 128, 40, 3, 3);
    PRG::Seed seed = PRG::SetSeed(nullptr, 0); // initialize PRG seed
    //std::vector<block> vec_x = PRG::GenRandomBlocks(seed, pp.SENDER_LEN);
    //std::vector<block> vec_y = PRG::GenRandomBlocks(seed, pp.RECEIVER_LEN);
    std::vector<std::vector<uint8_t>> vec_x;
    std::vector<std::vector<uint8_t>> vec_y;
    for(int i=1;i<9;i++){
        vec_x.push_back(std::vector<uint8_t>(i));
    }
    for(int i=8;i<16;i++) vec_y.push_back(std::vector<uint8_t>(i));
    std::cout <<  vec_x.size() << "----" << vec_y.size() << std::endl;
    block a = Block::MakeBlock(12, 43);
    std::cout << Block::BlockToInt64(a) << std::endl;
    Block::PrintBlock(a);
    int *ptr = (int *) &vec_x[0];
    for (int i = 0; i < 4; ++i) {
        std::cout << *(ptr + i) << " ";
    }
    std::cout << std::endl;
    std::cout << "---------" << std::endl;
    std::string party;
    std::cout
            << "please select your role between sender and receiver (hint: first start receiver, then start sender) ==> ";

    std::getline(std::cin, party);
    PrintSplitLine('-');


    if (party == "sender") {
        NetIO client("client", "127.0.0.1", 8087);
        mqRPMTPSU::Send(client, pp, vec_x,vec_x.size());
    }

    if (party == "receiver") {
        NetIO server("server", "", 8087);
        std::vector<std::vector<uint8_t>>  vec_union_prime = mqRPMTPSU::Receive(server, pp, vec_y,vec_y.size());
        for(std::vector<uint8_t> b: vec_union_prime){
            for(int i=0;i<b.size();i++)
                std::cout << b[i]<<std::endl;
        }


//        std::set<block, BlockCompare> set_diff_result =
//                ComputeSetDifference(vec_union_prime, testcase.vec_union);
//
//        double error_probability = set_diff_result.size()/double(testcase.vec_union.size());
//        std::cout << "mqRPMT-based PSU test succeeds with probability " << (1 - error_probability) << std::endl;
    }
    CRYPTO_Finalize();

}