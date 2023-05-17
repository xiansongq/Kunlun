//
// Created by 17579 on 2023/5/10.
//

#ifndef KUNLUN_LABEL_PSI_HPP
#define KUNLUN_LABEL_PSI_HPP
#include "../../filter/bloom_filter.hpp"

#include "../oprf/ote_oprf.hpp"
#include "../ot/alsz_ote.hpp"
namespace LabelPSI {
    std::string uintTostring(std::vector<uint8_t> num) {
        std::string str = "";
        for (int i = 0; i < num.size(); i++) {
            str += std::to_string(static_cast<int>(num[i]));

        }
        return str;
    }

    std::vector<block> Receive(NetIO &io, OTEOPRF::PP &pp, std::vector<block> &vec_Y) {
//        if (vec_Y.size() != pp.LEN) {
//            std::cerr << "|Y| does not match public parameter" << std::endl;
//            exit(1);
//        }
        auto start_time = std::chrono::steady_clock::now();
        std::vector<std::vector<uint8_t>> oprf_key = OTEOPRF::Server(io, pp);
        std::vector<std::vector<uint8_t>> oprf_value = OTEOPRF::Evaluate(pp, oprf_key, vec_Y, vec_Y.size());
        std::vector<uint8_t> vec_indication_bit;
        std::vector<block> ans;
        std::string choice = "bloom";
        if (choice == "bloom") {
            BloomFilter filter;
            size_t filter_size = filter.ObjectSize();
            io.ReceiveInteger(filter_size);
            char *buffer = new char[filter_size];
            io.ReceiveBytes(buffer, filter_size);
            filter.ReadObject(buffer);
            auto bloom_start_time = std::chrono::steady_clock::now();
            vec_indication_bit = filter.Contain(oprf_value);
            auto bloom_end_time = std::chrono::steady_clock::now();
            auto running_time1 = bloom_end_time - bloom_start_time;
            std::cout << "Receiver excute bloom filter query takes time= "
                      << std::chrono::duration<double, std::milli>(running_time1).count() << " ms" << std::endl;

            PrintSplitLine('-');
            std::cout << "mpOPRF-based PSI: Receive <=== BloomFilter(F_k(x_i)) <=== Sender ["
                      << (double) (filter_size) / (1024*1024) << " MB]" << std::endl;
            delete[] buffer;

        }

        std::vector<block> vec_indication_block(vec_indication_bit.size());
        for (int i = 0; i < vec_indication_bit.size(); i++) {
            //if ((int) vec_indication_bit[i] == 1) ans.push_back(vec_Y[i]);
            vec_indication_block[i]=Block::MakeBlock('0LL',vec_indication_bit[i]);
        }

        ALSZOTE::PP p1;
        p1 = ALSZOTE::Setup(128);
        std::cout<<"start execute OT"<<std::endl;
        ans=ALSZOTE::OnesidedReceive(io, p1, vec_indication_bit, 1<<20);
        auto end_time = std::chrono::steady_clock::now();
        auto running_time = end_time - start_time;
        std::cout << "mpOPRF-based PSI [bloom filter]: Receiver side takes time "
                  << std::chrono::duration<double, std::milli>(running_time).count() << " ms" << std::endl;
        return ans;
    }

    void Send(NetIO &io, OTEOPRF::PP &pp, std::vector<block> &vec_X,std::vector<block> label) {
//        if (vec_X.size() != pp.LEN) {
//            std::cerr << "|X| does not match public parameter" << std::endl;
//            exit(1);
//        }
        auto start_time = std::chrono::steady_clock::now();
        std::vector<std::vector<uint8_t>> oprf_value = OTEOPRF::Client(io, pp, vec_X, 1<<20);
        //std::vector<std::vector<uint8_t>> oprf_value = OTEOPRF::Evaluate(pp, oprf_key, vec_X, vec_X.size());
        std::string choice = "bloom";
        if (choice == "bloom") {
            BloomFilter filter(oprf_value.size(), 40);
            auto bloom_start_time = std::chrono::steady_clock::now();
            filter.Insert(oprf_value);
            auto bloom_end_time = std::chrono::steady_clock::now();
            auto running_time1 = bloom_end_time - bloom_start_time;
            std::cout << "Sender execute bloom filter insert takes time= "
                      << std::chrono::duration<double, std::milli>(running_time1).count() << " ms" << std::endl;

            size_t filter_size = filter.ObjectSize();
            io.SendInteger(filter_size);
            char *buffer = new char[filter_size];
            filter.WriteObject(buffer);
            io.SendBytes(buffer, filter_size);
            filter.ReadObject(buffer);

            std::cout << "mpOPRF-based PSI: Sender ===> BloomFilter(F_k(x_i)) ===> Receiver ["
                      << (double) (filter_size)/ (1024*1024) << " MB]" << std::endl;
            delete[] buffer;


        }

        PrintSplitLine('-');

        ALSZOTE::PP p1;
        p1 = ALSZOTE::Setup(128);
        std::cout<<"start execute OT"<<std::endl;
        ALSZOTE::OnesidedSend(io, p1, label, 1<<20);
        auto end_time = std::chrono::steady_clock::now();
        auto running_time = end_time - start_time;
        std::cout << "mpOPRF-based PSI [bloom filter]: Sender side takes time "
                  << std::chrono::duration<double, std::milli>(running_time).count() << " ms" << std::endl;
    }
}
#endif //KUNLUN_LABEL_PSI_HPP
