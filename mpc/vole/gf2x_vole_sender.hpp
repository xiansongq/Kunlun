//
// Created by 17579 on 2023/4/28.
//

#ifndef KUNLUN_GF2X_VOLE_SENDER_HPP_
#define KUNLUN_GF2X_VOLE_SENDER_HPP_

#include "../../include/global.hpp"
#include "../../crypto/block.hpp"
namespace GF2XVOLESENDER{
    struct PP{
        int maxnum;
        int num=maxnum;  // 返回的实例数量
    };
    void initsender(int maxNum){
        PP pp;
        pp.maxnum=maxNum;
        pp.num=maxNum;
    }
    void initsender(int maxNum,int Num){
        if(Num <0 || Num >maxNum){
            std::cerr<<"num must be in range [0,"<<maxNum<<"]"<<std::endl;
        }
        PP pp;
        pp.maxnum=maxNum;
        pp.num=Num;
    }
    struct SenderOutput{
        std::vector<block> x;
        std::vector<block> t;
    };
    
    SenderOutput

}


#endif //KUNLUN_GF2X_VOLE_SENDER_HPP_
