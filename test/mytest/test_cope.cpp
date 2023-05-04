//
// Created by 17579 on 2023/4/24.
//


#include "../../mpc/vole/cope1.hpp"
#include "../../crypto/setup.hpp"

int main() {
    // 必须初始化
    CRYPTO_Initialize();
    COPE::PP pp;
//    PRG::Seed seed = PRG::SetSeed();
//    auto a = PRG::GenRandomBlocks(seed, 1)[0];
//    __uint128_t delta = Block::TO__uint128_t(a);
//    delta = delta % 2305843009213693951;
//    COPE::PP pp;
//    std::cout << "123" << std::endl;
//    pp.ote_part = IKNPOTE::Setup(128);
//    std::cout << "OT init success!!" << std::endl;
//    pp.mask = (__uint128_t) 0xFFFFFFFFFFFFFFFFLL;
//    pp.m = 61;
//    std::cout << "PRG init" << std::endl;
//    for (int i = 0; i < pp.m; i++) pp.G0.push_back(PRG::SetSeed());
//    for (int i = 0; i < pp.m; i++) pp.G1.push_back(PRG::SetSeed());
//    std::cout << "PRG init success" << std::endl;
//    pp.delta = Block::TO_block(delta);
//    pp.test_n = 1024 * 128;
//    pp.mac.resize(pp.test_n);
//    std::cout << "setup success" << std::endl;

    pp = COPE::Setup(128, 61, 1024*128);


    std::cout << pp.m << std::endl;
    std::string party;
    std::cout
            << "please select your role between sender and receiver (hint: first start receiver, then start sender) ==> ";

    std::getline(std::cin, party);
    PrintSplitLine('-');

    if (party == "sender") {
        NetIO Sender("server", "127.0.0.1", 8092);
        std::cout << "------sender ---- " << std::endl;
        pp.party=1;
        COPE::Sender(Sender, pp);
    }

    if (party == "receiver") {
        NetIO Recver("client", "127.0.0.1", 8092);
        std::cout << "------receiver ---- " << std::endl;
        COPE::Receiver(Recver, pp);
    }


    CRYPTO_Finalize();
    return 0;

}