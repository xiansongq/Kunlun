#include "../../filter/cuckoo_filter.hpp"
#include "../../utility/print.hpp"

int main(){
    PrintSplitLine('-');
    std::cout << "begin the test of cuckoo filter >>>" << std::endl;
    PrintSplitLine('-');
    double desired_false_positive_probability = 1/pow(2, 10);
    uint64_t len=1<<20;
    len+=10000;
    CuckooFilter filter(len, desired_false_positive_probability);
    filter.PrintInfo();

    for(int i=0;i<len;i++) filter.Insert(i);
    filter.PrintInfo();
    return 0;

}