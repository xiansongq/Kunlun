#ifndef KUNLUN_PSI_HPP_
#define KUNLUN_PSI_HPP_

#include "../../filter/bloom_filter.hpp"
#include "../oprf/mp_oprf.hpp"
#include "../oprf/ote_oprf.hpp"
/*等待实现 */
namespace OPRFPSI{

    void Send(NetIO &io, OTEOPRF::PP &pp, std::vector<block> &vec_X, size_t LEN)
    {
        auto start_time = std::chrono::steady_clock::now();
        /**/
        std::vector<std::vector<uint8_t>> oprf_value = OTEOPRF::Client(io, pp,vec_X,vec_X.size());
        //std::vector<std::string> vec_oprf_values = OTEOPRF::Evaluate(pp, oprfkey, vec_X,vec_X.size());

        std::string choice = "bloom";

        if (choice == "bloom")
        {
            BloomFilter filter(pp.LEN, 40);
            filter.Insert(oprf_value);

            size_t filter_size = filter.ObjectSize(); 
            io.SendInteger(filter_size);

            char *buffer = new char[filter_size]; 
            filter.WriteObject(buffer);
            io.SendBytes(buffer, filter_size); 
		    delete[] buffer;

            PrintSplitLine('-');
            std::cout << "mpOPRF-based PSI: Sender ===> BloomFilter(F_k(x_i)) ===> Receiver [" << (double)(filter_size)/(1 << 20) << " MB]" << std::endl;
            
            auto end_time = std::chrono::steady_clock::now(); 
    	    auto running_time = end_time - start_time;
    	    std::cout << "mpOPRF-based PSI: Sender side takes time " 
	            << std::chrono::duration <double, std::milli> (running_time).count() << " ms" << std::endl;
        }
        
        if (choice == "plain")
        {
            for (auto i = 0; i < LEN; i++)
            {
                std::string str=std::to_string(oprf_value[i]);
                io.SendString(str);
            }

            PrintSplitLine('-');
            std::cout << "mpOPRF-based PSI: Sender ===> F_k(x_i) ===> Receiver [" << (double)(vec_oprf_values.size()*pp.H2_OUTPUT_LEN)/(1 << 20) << " MB]" << std::endl;
            
            auto end_time = std::chrono::steady_clock::now(); 
    	    auto running_time = end_time - start_time;
    	    std::cout << "mpOPRF-based PSI: Sender side takes time " 
	            << std::chrono::duration <double, std::milli> (running_time).count() << " ms" << std::endl;
        }     
    }

    std::vector<uint8_t> Receive(NetIO &io, OTEOPRF::PP &pp, std::vector<block> &vec_Y, size_t LEN)
    {
        auto start_time = std::chrono::steady_clock::now();

        std::vector<std::vector<uint8_t>> oprf_key = OTEOPRF::Server(io, pp);
        std::vector<std::vector<uint8_t>> oprf_value=OTEOPRF::Evaluate(pp,oprf_key,vec_Y,vec_Y.size());

        std::string choice = "bloom";
        std::vector<uint8_t> vec_indication_bit;

        if (choice == "bloom")
        {
            BloomFilter filter(pp.LEN, 40);

            size_t filter_size = filter.ObjectSize();
            io.ReceiveInteger(filter_size);

            char *buffer = new char[filter_size]; 
            io.ReceiveBytes(buffer, filter_size);
            filter.ReadObject(buffer);  
            delete[] buffer;

            vec_indication_bit = filter.Contain(oprf_value);

            auto end_time = std::chrono::steady_clock::now(); 
    	    auto running_time = end_time - start_time;
    	    std::cout << "mpOPRF-based PSI [bloom filter]: Receiver side takes time " 
	            << std::chrono::duration <double, std::milli> (running_time).count() << " ms" << std::endl;
        }
        
        if (choice == "plain")
        {
            std::string temp(10, '0');
            std::vector<std::string> rev_oprf_values(LEN, temp);
            std::unordered_set<std::string> S;

            for (auto i = 0; i < LEN; i++)
            {
                io.ReceiveString(rev_oprf_values[i]);
                S.insert(rev_oprf_values[i]); 
            }

            #pragma omp parallel for
            for(auto i = 0; i < LEN; i++){
                if(S.find(oprf_value[i]) == S.end()) vec_indication_bit[i] = 0;
                else vec_indication_bit[i] = 1;
            }  

            auto end_time = std::chrono::steady_clock::now(); 
    	    auto running_time = end_time - start_time;
    	    std::cout << "mpOPRF-based PSI [plain]: Receiver side takes time " 
	            << std::chrono::duration <double, std::milli> (running_time).count() << " ms" << std::endl;
        }

        return vec_indication_bit;
    }
}

#endif