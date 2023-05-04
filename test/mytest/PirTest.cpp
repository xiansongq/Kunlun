//
//#include "../../mpc/pir/seal_pir.hpp"
//
//
//int main() {
//    //首先定义基本参数
//    uint64_t number_of_items = 1 << 8;
//    uint64_t size_per_item = 64; // in bytes
//    uint32_t N = 4096;
//
//    // Recommended values: (logt, d) = (20, 2).
//    uint32_t logt = 20;
//    uint32_t d = 2;
//    bool use_symmetric = true; // use symmetric encryption instead of public key
//    // (recommended for smaller query)
//    bool use_batching = true;  // pack as many elements as possible into a BFV
//    // plaintext (recommended)
//    bool use_recursive_mod_switching = true;
//
//    EncryptionParameters enc_params(scheme_type::bfv);
//    PirParams pir_params;
//    std::cout << "Main: Generating SEAL parameters" << std::endl;
//    gen_encryption_params(N, logt, enc_params);
//
//    std::cout << "Main: Verifying SEAL parameters" << std::endl;
//    verify_encryption_params(enc_params);
//    std::cout << "Main: SEAL parameters are good" << std::endl;
//
//    std::cout << "Main: Generating PIR parameters" << std::endl;
//    gen_pir_params(number_of_items, size_per_item, d, enc_params, pir_params,
//                   use_symmetric, use_batching, use_recursive_mod_switching);
//
//    print_seal_params(enc_params);
//    print_pir_params(pir_params);
//    /*
//     * 1、建立 服务器和客户端的通信连接
//     * 2、服务器将 关键字和位置映射的文件发给客户端
//     * 3、客户端 查询关键字和位置的映射记录 得到自己所要查询关键字数据的位置映射
//     * 4、服务器将 数据 编码放入db
//     * 5、客户端执行查询
//    */
//    std::string party;
//    std::cout
//            << "please select your role between server and client (hint: first start server, then start client) ==> ";
//
//    std::getline(std::cin, party);
//    PrintSplitLine('-');
//    if (party == "client") {
//        NetIO client("client", "127.0.0.1", 8090);
//        random_device rd;
//
//        //Choose an index of an element in the DB
//        uint64_t ele_index = rd() % number_of_items; // element in DB at random position
//        std::cout << "elenumber index:" << ele_index << std::endl;
//        std::vector<uint8_t> ans = SEALPIR::Clinet(client, pir_params, enc_params, ele_index);
//        for (int i; i < ans.size(); i++) {
//
//        }
//        for (auto a: ans) {
//            std::cout << (int) a << " ";
//        }
//        std::cout << "ans_size: " << ans.size() << std::endl;
//    } else if (party == "server") {
//
//        NetIO Server("server", "127.0.0.1", 8090);
//
//
////         Copy of the database. We use this at the end to make sure we retrieved
////         the correct element.
////         Create test database
//        auto db(make_unique<uint8_t[]>(number_of_items * size_per_item));
//
//        // Copy of the database. We use this at the end to make sure we retrieved
//        // the correct element.
//        auto db_copy(make_unique<uint8_t[]>(number_of_items * size_per_item));
//
//        random_device rd;
//        for (uint64_t i = 0; i < number_of_items; i++) {
//            for (uint64_t j = 0; j < size_per_item; j++) {
//                uint8_t val = rd() % 256;
//                db.get()[(i * size_per_item) + j] = val;
//                db_copy.get()[(i * size_per_item) + j] = val;
//            }
//        }
//
//        for (uint64_t i = 0; i < number_of_items; i++) {
//
//            std::cout << "index " << i << ": ";
//            for (uint64_t j = 0; j < size_per_item; j++) {
//                std::cout << (int) (db.get()[(i * size_per_item) + j]) << " ";
//            }
//            std::cout << std::endl;
//        }
//
//        SEALPIR::Server(Server, pir_params, enc_params, db);
//    }
//    return 0;
//}