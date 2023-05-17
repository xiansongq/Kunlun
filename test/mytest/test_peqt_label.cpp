#include "../../mpc/peqt/peqt_from_ddh.hpp"
#include "../../crypto/setup.hpp"


struct PEQTTestcase{
    size_t LEN;
    std::vector<block> vec_X;
    std::vector<block> vec_Y;
    std::vector<uint8_t> vec_indication_bit;
};

PEQTTestcase GenTestInstance(size_t LEN)
{
    PEQTTestcase testcase;
    testcase.LEN = LEN;
    PRG::Seed seed = PRG::SetSeed(fixed_seed, 0); // initialize PRG
    testcase.vec_X = PRG::GenRandomBlocks(seed, LEN);
    testcase.vec_Y = PRG::GenRandomBlocks(seed, LEN);
    testcase.vec_indication_bit = PRG::GenRandomBits(seed, LEN);
    for(auto i = 0; i < LEN; i++){
        if(testcase.vec_indication_bit[i] == 1){
            testcase.vec_Y[i] = testcase.vec_X[i];
        }
    }

    return testcase;
}

void SaveTestInstance(PEQTTestcase &testcase, std::string testcase_filename)
{
    std::ofstream fout;
    fout.open(testcase_filename, std::ios::binary);
    if(!fout)
    {
        std::cerr << testcase_filename << " open error" << std::endl;
        exit(1);
    }
    fout << testcase.LEN;
    fout << testcase.vec_X;
    fout << testcase.vec_Y;
    fout << testcase.vec_indication_bit;

    fout.close();
}

void FetchTestInstance(PEQTTestcase &testcase, std::string testcase_filename)
{
    std::ifstream fin;
    fin.open(testcase_filename, std::ios::binary);
    if(!fin)
    {
        std::cerr << testcase_filename << " open error" << std::endl;
        exit(1);
    }
    fin >> testcase.LEN;
    testcase.vec_X.resize(testcase.LEN);
    testcase.vec_Y.resize(testcase.LEN);
    testcase.vec_indication_bit.resize(testcase.LEN);

    fin >> testcase.vec_X;
    fin >> testcase.vec_Y;
    fin >> testcase.vec_indication_bit;

    fin.close();
}


int main()
{
    CRYPTO_Initialize();
    //从文件读取数据
    std::vector<block> vecx;
    std::ifstream fin;
    fin.open("A_PSI_DATA.txt", std::ios::binary);
    if (!fin) {
        std::cout << "Failed to open file" << std::endl;
        return -1;
    }
    std::string line;
    while (std::getline(fin, line) ) {
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
//    int len = 48576;
//    for (int i = 0; i < len; i++) vecx.push_back(Block::MakeBlock(0LL, 0LL));
//    for (int i = 0; i < len; i++) vecy.push_back(Block::MakeBlock(0LL, 0LL));
    std::cout << vecx.size() << "---" << vecy.size() << std::endl;
    PrintSplitLine('-');
    std::cout << "DDH-based PEQT test begins >>>" << std::endl;
    PrintSplitLine('-');
    std::cout << "generate or load public parameters and test case" << std::endl;

    // generate pp (must be same for both server and client)

    size_t ROW_NUM = size_t(pow(2, 0));
    size_t COLUMN_NUM = size_t(pow(2, 8));
    std::cout << "matrix dimension = " << ROW_NUM << "*" << COLUMN_NUM << std::endl;

    size_t LEN = ROW_NUM * COLUMN_NUM;
    std::string testcase_filename = "PEQT.testcase";
    PEQTTestcase testcase;
    if(!FileExist(testcase_filename)){
        testcase = GenTestInstance(LEN);
        SaveTestInstance(testcase, testcase_filename);
    }
    else{
        FetchTestInstance(testcase, testcase_filename);
    }

    if(testcase.LEN != LEN) std::cerr << "testcase LEN does not match" << std::endl;

    PrintSplitLine('-');

    std::string party;
    std::cout << "please select your role between sender and receiver (hint: first start sender, then start receiver) ==> ";
    std::getline(std::cin, party); // first the server, then the client
    PrintSplitLine('-');

    if(party == "sender"){
        NetIO server("server", "", 8080);
        std::vector<uint64_t> permutation_map = DDHPEQT::Send(server, vecx, 1,1000000);
    }

    if(party == "receiver")
    {
        NetIO client("client", "127.0.0.1", 8080);
        std::vector<uint8_t> vec_result = DDHPEQT::Receive(client, vecy, 1, 1000000);
    }

    PrintSplitLine('-');
    std::cout << "DDH-based PEQT test ends >>>" << std::endl;
    PrintSplitLine('-');

    CRYPTO_Finalize();

    return 0;
}