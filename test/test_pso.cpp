#include "../mpc/ot/iknp_ote.hpp"
#include "../crypto/setup.hpp"

int main() {
    CRYPTO_Initialize();
    PrintSplitLine('-');
    std::cout << "IKNP OTE test begins >>>" << std::endl;
    PrintSplitLine('-');
    std::cout << "generate or load public parameters and test case" << std::endl;
    IKNPOTE::PP pp;
    size_t EXTEND_LEN = 1 << 20;
    PRG::Seed seed = PRG::SetSeed(nullptr, 0); // initialize PRG seed
    std::vector<uint8_t> vec_selection_bit = GenRandomBits(seed, EXTEND_LEN);
    std::vector<block> vec_K0 = PRG::GenRandomBlocks(seed, EXTEND_LEN);
    std::vector<block> vec_K1 = PRG::GenRandomBlocks(seed, EXTEND_LEN);

    // set instance size
    std::cout << "LENGTH of OTE = " << EXTEND_LEN << std::endl;

    std::string party;
    std::cout
            << "please select your role between (one-sided) sender and (one-sided) receiver (hint: start sender first) ==> ";
    std::getline(std::cin, party); // first sender (acts as server), then receiver (acts as client)

    if (party == "sender") {
        NetIO server_io("server", "", 8085);
        IKNPOTE::Send(server_io, pp, vec_K0, vec_K1, EXTEND_LEN);
    }
    if (party == "receiver") {
        NetIO client_io("client", "127.0.0.1", 8085);
        std::vector<block> vec_result_prime = IKNPOTE::Receive(client_io, pp, vec_selection_bit, EXTEND_LEN);
        std::ofstream fout;
        fout.open("mytest.txt", std::ios::binary);
        if (!fout) {
            std::cerr << "mytest.txt" << " open error" << std::endl;
            exit(1);
        }
        std::string str = Block::ToString(vec_result_prime[1]);
        std::cout << str << std::endl;

        //Block::operator<<(fout,vec_result_prime);
        //Block::PrintBlocks(vec_result_prime);

    }
    PrintSplitLine('-');
    std::cout << "IKNP OTE test ends >>>" << std::endl;
    PrintSplitLine('-');
    CRYPTO_Finalize();
    return 0;
}