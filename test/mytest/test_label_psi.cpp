//
// Created by xiansong on 2023/5/4.
//

#include "../../mpc/psi/psi_from_oprf.hpp"
#include "../../crypto/setup.hpp"
#include "../../mpc/psi/label_psi.hpp"

/*
 * test based-OTE PRF private set intersection
 *
 * */
int main() {
    CRYPTO_Initialize();
    std::cout << "label psi is begings >>>" << std::endl;
    PrintSplitLine('-');
    OTEOPRF::PP pp;
    pp = OTEOPRF::Setup(20, 40);
    std::vector<block> vecx;
    //从文件读取数据
    std::ifstream fin;
    fin.open("A_PIR_ID.txt", std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open file" << std::endl;
        std::exit(-1);
    }
    std::string line;
    int i = 0;
    while (std::getline(fin, line, '\n')) {
        std::stringstream stream(line);
        uint64_t a;
        stream >> a;
        vecx.push_back(Block::MakeBlock(0LL, a));
    }
    fin.close();


    std::vector<block> vecy;
    std::vector<block> label;
    std::ifstream fin1;
    fin1.open("B_PIR_DATA.txt", std::ios::binary);
    if (!fin1) {
        std::cout << "Failed to open file" << std::endl;
        return -1;
    }
    while (std::getline(fin1, line)) {
        std::vector<std::string> row;
        row.push_back(std::to_string(i++));   /*add index colunms*/
        size_t pos = 0;
        std::string token;
        /* Split each row of data into multiple columns.*/
        while ((pos = line.find(",")) != std::string::npos) {
            token = line.substr(0, pos);
            row.push_back(token);
            line.erase(0, pos + 1);
        }
        row.push_back(line);
        std::string temp = row[2];
        temp.erase(std::remove(temp.begin(), temp.end(), '-'), temp.end());
        row[2] = temp;
        vecy.push_back(Block::MakeBlock('0LL',std::stoull(row[1])));
        std::string str="";
        for(auto a:row) str+=a;
        label.push_back(Hash::StringToBlock(str)); /*save data*/

    }
    fin1.close();

//    std::string line1;
//
//    while (std::getline(fin1, line1)) {
//        std::stringstream stream(line1);
//        uint64_t a;
//        stream >> a;
//        vecy.push_back(Block::MakeBlock(0LL, a));
//    }
//    fin1.close();

    /*必须做数据补全 才能成功执行协议
     * 现在 数据有1000000 条
     * 选择 2^20
     * 那么还差 48576条数据
     * */
    for (int i = 0; i < 1048566; ++i) vecx.push_back(Block::MakeBlock(0LL, 0LL));
    int len = 48576;
    //for (int i = 0; i < len; i++) vecx.push_back(Block::MakeBlock(0LL, 0LL));
    for (int i = 0; i < len; i++) vecy.push_back(Block::MakeBlock(0LL, 0LL));
    for (int i = 0; i < len; i++) label.push_back(Block::MakeBlock(0LL, 0LL));
    std::cout << vecx.size() << "---" << vecy.size() <<"----"<<label.size()<< std::endl;



    std::string party;
    std::cout
            << "please select your role between sender and receiver (hint: first start receiver, then start sender) ==> ";

    std::getline(std::cin, party);
    PrintSplitLine('-');
    if (party == "sender") {
        NetIO client("client", "127.0.0.1", 8090);
        LabelPSI::Send(client, pp, vecx,label);

    }
    std::vector<block> vec_intersection_prime;
    if (party == "receiver") {
        NetIO server("server", "", 8090);
        vec_intersection_prime = LabelPSI::Receive(server, pp, vecy);
        std::cout<<vec_intersection_prime.size()-len<<std::endl;
        std::cout<<"receiver output intersection to file"<<std::endl;
        //output intersection result to csv file
        std::ofstream fout;
        fout.open("labelPSI.csv", std::ios::out | std::ios::binary);
        if (!fout) {
            std::cerr << "labelPSI.csv open error" << std::endl;
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