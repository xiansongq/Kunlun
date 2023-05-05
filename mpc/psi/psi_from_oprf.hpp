#ifndef KUNLUN_PSI_HPP_
#define KUNLUN_PSI_HPP_

#include "../../filter/bloom_filter.hpp"

#include "../oprf/ote_oprf.hpp"
#include "../ot/alsz_ote.hpp"
//为什么会出现错误率过高的情况

namespace OPRFPSI {
    std::vector<block> Receive(NetIO &io, OTEOPRF::PP &pp, std::vector<block> &vec_Y) {
        if(vec_Y.size()!= pp.LEN){
            std::cerr << "|Y| does not match public parameter" << std::endl;
            exit(1);
        }
        auto start_time = std::chrono::steady_clock::now();
        std::vector<std::vector<uint8_t>> oprf_value = OTEOPRF::Client(io, pp, vec_Y, vec_Y.size());
        std::vector<uint8_t> vec_indication_bit(vec_Y.size());
        std::string choice = "bloom";
        if (choice == "bloom") {
            BloomFilter filter;
            size_t filter_size = filter.ObjectSize();
            io.ReceiveInteger(filter_size);
            char *buffer = new char[filter_size];
            io.ReceiveBytes(buffer, filter_size);
            filter.ReadObject(buffer);

            auto bloom_start_time = std::chrono::steady_clock::now();
            vec_indication_bit=filter.Contain(oprf_value);
            auto bloom_end_time = std::chrono::steady_clock::now();
            auto running_time1 = bloom_end_time - bloom_start_time;
            std::cout << "Receiver excute bloom filter query time "
                      << std::chrono::duration<double, std::milli>(running_time1).count() << " ms" << std::endl;
            delete[] buffer;

            PrintSplitLine('-');
            std::cout << "mpOPRF-based PSI: Sender ===> BloomFilter(F_k(x_i)) ===> Receiver ["
                      << (double) (filter_size) / (1 << 20) << " MB]" << std::endl;
        }
        int count=0;
        for(int i=0;i<vec_indication_bit.size();i++){
            if((int)vec_indication_bit[i]==1) std::cout<<i<<std::endl,count++;

        }
        std::cout<<count<<std::endl;
        std::vector<block> ans;
        for(int i=0;i<vec_indication_bit.size();i++){
            if((int)vec_indication_bit[i]==1) ans.push_back(vec_Y[i]);
        }
        //OT
//        auto stime = std::chrono::steady_clock::now();
//        ALSZOTE::PP p1;
//        p1 = ALSZOTE::Setup(128);
//        std::cout<<"start execute OT"<<std::endl;
//        std::vector<block> ans = ALSZOTE::OnesidedReceive(io, p1, vec_indication_bit, vec_indication_bit.size());
//        auto etime = std::chrono::steady_clock::now();
//        auto running_time = etime - stime;
//        std::cout << "mpOPRF-based PSI: Receiver OT receive time "
//                  << std::chrono::duration<double, std::milli>(running_time).count() << " ms" << std::endl;
        auto end_time = std::chrono::steady_clock::now();
        auto running_time1 = end_time - start_time;
        std::cout << "mpOPRF-based PSI: Receiver side takes time "
                  << std::chrono::duration<double, std::milli>(running_time1).count() << " ms" << std::endl;
        return ans;
    }

    void Send(NetIO &io, OTEOPRF::PP &pp, std::vector<block> &vec_X) {
        if(vec_X.size()!= pp.LEN){
            std::cerr << "|X| does not match public parameter" << std::endl;
            exit(1);
        }
        auto start_time = std::chrono::steady_clock::now();
        std::vector<std::vector<uint8_t>> oprf_key = OTEOPRF::Server(io, pp);
        std::vector<std::vector<uint8_t>> oprf_value = OTEOPRF::Evaluate(pp, oprf_key, vec_X, vec_X.size());
        std::string choice = "bloom";
        if (choice == "bloom") {
            BloomFilter filter(pp.LEN, 40);

            auto bloom_start_time = std::chrono::steady_clock::now();
            filter.Insert(oprf_value);
            auto bloom_end_time = std::chrono::steady_clock::now();
            auto running_time1 = bloom_end_time - bloom_start_time;
            std::cout << "Receiver execute bloom filter insert time "
                      << std::chrono::duration<double, std::milli>(running_time1).count() << " ms" << std::endl;

            size_t filter_size = filter.ObjectSize();
            io.SendInteger(filter_size);
            char *buffer = new char[filter_size];
            filter.WriteObject(buffer);
            io.SendBytes(buffer, filter_size);
            filter.ReadObject(buffer);
            delete[] buffer;

        }
        PrintSplitLine('-');
//        auto stime = std::chrono::steady_clock::now();
//        ALSZOTE::PP p1;
//        p1 = ALSZOTE::Setup(128);
//        std::cout<<"start execute OT"<<std::endl;
//        ALSZOTE::OnesidedSend(io, p1, vec_X, vec_X.size());
//        auto etime = std::chrono::steady_clock::now();
//        auto running_time = etime - stime;
//        std::cout << "mpOPRF-based PSI: Sender send OT time "
//                  << std::chrono::duration<double, std::milli>(running_time).count() << " ms" << std::endl;
        auto end_time = std::chrono::steady_clock::now();
        auto running_time1 = end_time - start_time;
        std::cout << "mpOPRF-based PSI [bloom filter]: Sender side takes time "
                  << std::chrono::duration<double, std::milli>(running_time1).count() << " ms" << std::endl;
    }
}

#endif
