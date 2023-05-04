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
    pp = OTEOPRF::Setup(18, 40);
    std::string party;
    std::cout
            << "please select your role between sender and receiver (hint: first start receiver, then start sender) ==> ";

    PRG::Seed seed = PRG::SetSeed(fixed_seed, 0);
    std::vector<block> vec_x = PRG::GenRandomBlocks(seed, pp.LEN);
    std::vector<block> vec_y = PRG::GenRandomBlocks(seed, pp.LEN);

    vec_x[10] = vec_y[10];
    vec_x[20] = vec_y[30];
    vec_x[100] = vec_y[1];
    vec_x[200] = vec_y[40];
    vec_x[500] = vec_y[90];


    std::getline(std::cin, party);
    PrintSplitLine('-');
    if (party == "sender") {
        NetIO client("client", "127.0.0.1", 8090);
        OPRFPSI::Send(client, pp, vec_x);

    }

    if (party == "receiver") {
        NetIO server("server", "", 8090);
        std::vector<block> vec_intersection_prime = OPRFPSI::Receive(server, pp, vec_y);
        for (auto a: vec_intersection_prime) {
            if (Block::BlockToInt64(a) == 0 && Block::BlockToUint64High(a) == 0) continue;
            else Block::PrintBlock(a);
        }
        std::cout << std::endl;
    }

    CRYPTO_Finalize();

    return 0;

}