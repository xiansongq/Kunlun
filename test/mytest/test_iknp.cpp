//
// Created by 17579 on 2023/4/25.
//

#include "../../mpc/ot/iknp_ote.hpp"
#include "../../crypto/setup.hpp"
int main(){
    CRYPTO_Initialize();
    IKNPOTE::PP pp;
    pp=IKNPOTE::Setup(128);
}
