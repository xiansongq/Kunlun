//
// Created by 17579 on 2023/5/6.
//

#include "../../mpc/psi/ecdh_psi.hpp"
#include "../../crypto/setup.hpp"
int main(){
    CRYPTO_Initialize();
#ifdef USE_CURVE_25519
    std::cerr << "Using Curve 25519" << std::endl;
#endif
    ECDHPSI::PP pp;
    pp=ECDHPSI::Setup("bloom",40,10,10);
    PRG::Seed seed=PRG::SetSeed(fixed_seed,0);
    std::vector<block> vecx=PRG::GenRandomBlocks(seed,pp.SERVER_LEN);
    std::vector<block> vecy = PRG::GenRandomBlocks(seed, pp.CLIENT_LEN);

    vecx[1]=vecy[10];
    vecx[6]=vecy[24];
    vecx[8]=vecy[27];
    vecx[12]=vecy[31];

    std::cout << vecx.size() << "---" << vecy.size() << std::endl;

    std::string party;
    std::cout
            << "please select your role between sender and receiver (hint: first start receiver, then start sender) ==> ";

    std::getline(std::cin, party);
    PrintSplitLine('-');
    std::vector<uint8_t> vec_intersection_prime;
    if (party == "sender") {
        NetIO client("client", "127.0.0.1", 8091);
        ECDHPSI::Client(client, pp, vecx);
    }

    if (party == "receiver") {
        NetIO server("server", "127.0.0.1", 8091);
        vec_intersection_prime =ECDHPSI::Server(server, pp, vecy);
        std::cout<<(int)vec_intersection_prime.size()<<std::endl;
    }
    CRYPTO_Finalize();

    return 0;
}
