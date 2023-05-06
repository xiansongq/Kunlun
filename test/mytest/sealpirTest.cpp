//
// Created by 17579 on 2023/4/17.
//

#include "../../mpc/pir/sealpir_keyword.hpp"


#include<iostream>

using namespace std;

int main() {
    uint64_t number_of_items = 1000000;
    uint64_t size_per_item = 28; // in bytes
    uint32_t N = 4096;

    // Recommended values: (logt, d) = (20, 2).
    uint32_t logt = 20;
    uint32_t d = 2;
    bool use_symmetric = true; // use symmetric encryption instead of public key
    // (recommended for smaller query)
    bool use_batching = true;  // pack as many elements as possible into a BFV
    // plaintext (recommended)
    bool use_recursive_mod_switching = true;

    EncryptionParameters enc_params(scheme_type::bfv);
    PirParams pir_params;
    cout << "Main: Generating SEAL parameters" << endl;
    gen_encryption_params(N, logt, enc_params);

    cout << "Main: Verifying SEAL parameters" << endl;
    verify_encryption_params(enc_params);
    cout << "Main: SEAL parameters are good" << endl;

    cout << "Main: Generating PIR parameters" << endl;
    gen_pir_params(number_of_items, size_per_item, d, enc_params, pir_params,
                   use_symmetric, use_batching, use_recursive_mod_switching);

    print_seal_params(enc_params);
    print_pir_params(pir_params);
    std::string party;
    std::cout
            << "please select your role between server and client (hint: first start server, then start client) ==> ";

    std::getline(std::cin, party);

    PrintSplitLine('-');
    if (party == "client") {
        NetIO client("client", "127.0.0.1", 8090);

        std::vector<std::vector<uint8_t>> ans = SEALPIRKEYWORD::Clinet(client, pir_params, enc_params, "");
/*        for (auto a: ans) {
            std::cout << "id: ";
            for (int i = 0; i <= 17; i++) {
                std::cout << (int) a[i];
            }
            std::cout << " register_date: ";
            for (int i = 18; i <= 25; i++) std::cout << (int) a[i];
            std::cout << " age: ";
            for (int i = 26; i < 28; i++) std::cout << (int) a[i];
            std::cout << std::endl;
        }*/
        //输出查询结果到文件
        std::ofstream fout;
        fout.open("sealpir_out.csv", std::ios::binary);
        if (!fout) {
            std::cerr << "sealpir_out.csv open error" << std::endl;
            exit(1);
        }
        if (fout.is_open()) {
            for (auto a: ans) {
                std::string string1 = "id: ";
                for (int i = 0; i <= 17; i++)
                    string1 += to_string(static_cast<int>(a[i]));

                string1 += " register_date: ";
                for (int i = 18; i <= 25; i++)
                    string1 += to_string(static_cast<int>(a[i]));

                string1 += " age: ";
                for (int i = 26; i < 28; i++)
                    string1 += to_string(static_cast<int>(a[i]));
                fout.write(string1.c_str(),string1.size());
                fout <<std::endl;
            }
        }
        fout.close();
        std::cout << "ans_size: " << ans.size() << std::endl;
    } else if (party == "server") {

        NetIO Server("server", "127.0.0.1", 8090);
        SEALPIRKEYWORD::Server(Server, pir_params, enc_params, "");
    }


    return 0;
}