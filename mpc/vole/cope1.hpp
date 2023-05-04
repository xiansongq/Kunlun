//
// Created by 17579 on 2023/4/23.
//

#ifndef KUNLUN_COPE_H
#define KUNLUN_COPE_H

#include "../../utility/serialization.hpp"
#include "../../crypto/prg.hpp"
#include "../ot/alsz_ote.hpp"
#include "../../crypto/block.hpp"
#include "../../netio/stream_channel.hpp"
#include "../ot/iknp_ote.hpp"

#define MERSENNE_PRIME_EXP 61
const static __uint128_t p = 2305843009213693951;
const static __uint128_t pr = 2305843009213693951;
const static uint64_t PR = 2305843009213693951;
namespace COPE {
    using Serialization::operator<<;
    using Serialization::operator>>;

    struct PP {
        IKNPOTE::PP ote_part;
        int party = 0;
        int m;
        block delta;
        __uint128_t mask;
        std::vector<PRG::Seed> G0;
        std::vector<PRG::Seed> G1;
        std::vector<bool> delta_bool;
        std::vector<block> mac;
        int test_n;

    };


    uint64_t mod(uint64_t x) {
        uint64_t i = (x & PR) + (x >> MERSENNE_PRIME_EXP);
        return (i >= p) ? i - p : i;
    }

    uint64_t add_mod(uint64_t a, uint64_t b) {
        uint64_t res = a + b;
        return (res >= PR) ? (res - PR) : res;
    }

    template<typename T>
    T mod(T k, T pv) {
        T i = (k & pv) + (k >> MERSENNE_PRIME_EXP);
        return (i >= pv) ? i - pv : i;
    }

    PP Setup(size_t computational_security_parameter, int m, int test_n) {
        PP pp;
        std::cout << computational_security_parameter << std::endl;
        pp.ote_part = IKNPOTE::Setup(computational_security_parameter);
        pp.mask = (__uint128_t) 0xFFFFFFFFFFFFFFFFLL;
        pp.m = m;
        for (int i = 0; i < m; i++) pp.G0.push_back(PRG::SetSeed(nullptr,0));
        for (int i = 0; i < m; i++) pp.G1.push_back(PRG::SetSeed(nullptr,0));
        PRG::Seed seed = PRG::SetSeed(nullptr,0);
        auto a = PRG::GenRandomBlocks(seed, 1)[0];
        __uint128_t delta = Block::TO__uint128_t(a);
        while (delta < 0) {
            a = PRG::GenRandomBlocks(seed, 1)[0];
            delta = Block::TO__uint128_t(a);
        }
        delta = mod(delta, pr);
        pp.delta = Block::TO_block(delta);
        pp.test_n = test_n;
        pp.mac.resize(pp.test_n);
        return pp;
    }

    void extract_fp(__uint128_t &x) {

        x = mod(_mm_extract_epi64((block) x, 0));
    }


    void delta64_to_bool(std::vector<bool> &bdata, __uint128_t u128, PP &pp) {
        uint64_t *ptr = (uint64_t *) (&u128);
        uint64_t in = ptr[0];
        for (int i = 0; i < pp.m; ++i) {
            bdata[i] = ((in & 0x1LL) == 1);
            in >>= 1;
        }
    }

    block prm2pr(std::vector<block> &a, PP pp) {
        block ret = Block::zero_block;
        block tmp;
        for (int i = 0; i < pp.m; i++) {
            tmp = Block::TO_block(mod(Block::TO__uint128_t(a[i]) << i, pr));
            ret = Block::TO_block(mod(Block::TO__uint128_t(ret) + Block::TO__uint128_t(tmp), pr));
        }
        return ret;
    }

    void prm2pr(std::vector<block> &ret, std::vector<block> &a, int size, PP &pp) {
        __uint128_t tmp;
        for (int i = 0; i < pp.m; ++i) {
            for (int j = 0; j < size; ++j) {
                tmp = mod(Block::TO__uint128_t(a[i * size + j]) << i, pr);
                ret[j] = Block::TO_block(mod(Block::TO__uint128_t(ret[j]) + tmp, pr));
            }
        }
    }

    void prm2pr(std::vector<block> &ret, std::vector<uint64_t> &a, int size, PP &pp) {
        __uint128_t tmp;
        for (int i = 0; i < pp.m; ++i) {
            for (int j = 0; j < size; ++j) {
                tmp = (__uint128_t) a[i * size + j];
                tmp = mod(tmp << i, pr);
                ret[j] = Block::TO_block((__uint128_t) add_mod(Block::BlockToInt64(ret[j]), tmp));
            }
        }
    }


    void initSender(NetIO &io, PP &pp) {
        pp.delta_bool.resize(pp.m);
        delta64_to_bool(pp.delta_bool,Block::TO__uint128_t(pp.delta),pp);
        std::vector<uint8_t> selection_bit;
        for (auto i: pp.delta_bool) {
            if (i == 1) selection_bit.push_back(1);
            else selection_bit.push_back(0);
        }
        for(auto a: selection_bit){
            std::cout<<(int)a<<" "<<std::endl;
        }

       // delta64_to_bool(pp.delta_bool, Block::TO__uint128_t(pp.delta), pp);
        std::vector<block> k = IKNPOTE::Receive(io, pp.ote_part, selection_bit, pow(2, 20));
        //Block::PrintBlocks(k);
        pp.G0.resize(pp.m);

        for (int i = 0; i < pp.m; ++i) {
            PRG::ReSeed(pp.G0[i], &k[i],0);

        }
        std::cout << "init sender success" << std::endl;
    }

    void initRecver(NetIO &io, PP &pp) {
        std::cout << "init Recver" << std::endl;
        std::vector<block> k(2 * pp.m);
        PRG::Seed seed = PRG::SetSeed(fixed_seed, 0);
        k = PRG::GenRandomBlocks(seed, 2 * pp.m);
        std::vector<block> vec_m0;
        std::vector<block> vec_m1;
        for (int i = 0; i < pp.m; i++) {
            vec_m0.push_back(k[i]);
            vec_m1.push_back(k[i + pp.m]);
        }
        IKNPOTE::Send(io, pp.ote_part, vec_m0, vec_m1, pow(2, 20));
        //pp.G0.resize(pp.m);
        //pp.G1.resize(pp.m);

        for (int i = 0; i < pp.m; i++) {
            PRG::ReSeed(pp.G0[i], &k[i],0);
            PRG::ReSeed(pp.G0[i], &k[i + pp.m],0);

        }
        std::cout << "init Recver success" << std::endl;

    }

    block extend(NetIO &io, PP &pp) {
        std::cout << "Recver extend" << std::endl;
        std::vector<block> w(pp.m);
        std::vector<block> v(pp.m);

        for (int i = 0; i < pp.m; ++i) {
            w[i]=PRG::GenRandomBlocks(pp.G0[i], 1)[0];
            //w.push_back(PRG::GenRandomBlocks(pp.G0[i], 1)[0]);
            __uint128_t a = Block::TO__uint128_t(w[i]);
            extract_fp(a);
        }
        // io 接收数据
        io.ReceiveBlocks(v.data(), pp.m);
        std::vector<block> ch(2);
        for (int i = 0; i < pp.m; i++) {
            ch[1] = v[i];
            __uint128_t a = mod(Block::TO__uint128_t(w[i]) + Block::TO__uint128_t(ch[pp.delta_bool[i]]), pr);
            v[i] = Block::TO_block(a);
        }
        std::cout << "Recver extend success" << std::endl;
        return prm2pr(v, pp);

    }

    // sender batch
    void extend(NetIO &io, std::vector<block> &ret, int size, PP &pp) {

        std::vector<uint64_t> w;
        std::vector<uint64_t> v;
        w.resize(pp.m * size);
        v.resize(pp.m * size);

        for (int i = 0; i < pp.m; ++i) {
            auto tmp = PRG::GenRandomBlocks(pp.G0[i], size);
            for (int i = 0; i < size; i++)
                w[i] = Block::BlockToInt64(tmp[i]);

            for (int j = 0; j < size; ++j) {
                w[i * size + j] = mod(w[i * size + j]);
            }
        }

        uint64_t ch[2];
        ch[0] = (uint64_t) 0;
        for (int i = 0; i < pp.m; ++i) {
            for (int j = 0; j < size; ++j) {
                block a;
                io.ReceiveBlock(a);
                v[i * size + j] = Block::BlockToInt64(a);
                ch[1] = v[i * size + j];
                v[i * size + j] = add_mod(w[i * size + j], ch[pp.delta_bool[i]]);
            }
        }
        // 将 v 转为block 的vector
//        std::vector<block> v1;
//        for (int i = 0; i < pp.m * size; i++) {
//            v1.push_back(Block::MakeBlock(0LL, v[i]));
//        }
        prm2pr(ret, v, size, pp);
        std::cout << "sender extend batch ending" << std::endl;

    }

    // recver extend
    block extend(NetIO &io, block u, PP &pp) {
        std::vector<block> w0(pp.m);
        std::vector<block> w1(pp.m);
        std::vector<block> tau(pp.m);

        for (int i = 0; i < pp.m; ++i) {
            w0[i]=PRG::GenRandomBlocks(pp.G0[i], 1)[0];
            w1[i]=PRG::GenRandomBlocks(pp.G1[i], 1)[0];
            __uint128_t a = Block::TO__uint128_t(w0[i]);
            extract_fp(a);
            a = Block::TO__uint128_t(w1[i]);
            extract_fp(a);
            w1[i] = Block::TO_block(mod(Block::TO__uint128_t(w1[i]) + Block::TO__uint128_t(u), pr));
            w1[i] = pr - w1[i];
            tau[i] = Block::TO_block(mod(Block::TO__uint128_t(w0[i]) + Block::TO__uint128_t(w1[i]), pr));
        }

        io.SendBlocks(tau.data(), pp.m);

        return prm2pr(w0, pp);
    }
    // recver extend batch
    void extend(NetIO &io, std::vector<block> &ret, std::vector<uint64_t> &u, int size, PP &pp) {
        std::vector<uint64_t> w0;
        std::vector<uint64_t> w1;
        w0.resize(pp.m * size);
        w1.resize(pp.m * size);

        for (int i = 0; i < pp.m; ++i) {
            auto tmp = PRG::GenRandomBlocks(pp.G0[i], size);
            for (int i = 0; i < size; i++)
                w0[i] = Block::BlockToInt64(tmp[i]);
            auto tmp1 = PRG::GenRandomBlocks(pp.G0[i], size);
            for (int i = 0; i < size; i++)
                w1[i] = Block::BlockToInt64(tmp1[i]);
            for (int j = 0; j < size; ++j) {
                w0[i * size + j] = mod(w0[i * size + j]);
                w1[i * size + j] = mod(w1[i * size + j]);
                w1[i * size + j] = add_mod(w1[i * size + j], u[j]);
                w1[i * size + j] = PR - w1[i * size + j];
                uint64_t tau = add_mod(w0[i * size + j], w1[i * size + j]);
                io.SendBlock(Block::MakeBlock(0LL, tau));

            }
        }
        std::cout << "ending for " << std::endl;

//        std::vector<block> w;
//        for (int i = 0; i < pp.m * size; i++) {
//            w[i]=Block::MakeBlock(0LL, w0[i]);
//        }
        prm2pr(ret, w0, size, pp);


        std::cout << "recver extend batch success" << std::endl;
    }

    // Debug function
    void check_triple(NetIO &io, std::vector<uint64_t> a, std::vector<block> b, int sz, PP &pp) {
        if (pp.party == 1) {
            std::cout << "sender check start" << std::endl;
            //将a转为 block
            std::vector<block> as;
            std::cout<<"a size "<<a.size()<<std::endl;
            for (auto s: a) {
                as.push_back(Block::MakeBlock(0LL, s));
            }
            std::cout<<"as size "<<as.size()<<std::endl;
            std::cout << "sender send delta value=" << Block::BlockToInt64(as[0]) << std::endl;

            io.SendBlocks(as.data(), as.size());
            io.SendBlocks(b.data(), sz);
            std::cout << "sender check io send ending" << std::endl;
        } else {
            std::cout << "receiver check start" << std::endl;
            uint64_t delta;
            block tmp;
            std::vector<block> d(sz);
            io.ReceiveBlock(tmp);
            std::cout << "tmp" << Block::BlockToInt64(tmp) << "-----" << Block::BlockToUint64High(tmp) << std::endl;
            delta = Block::BlockToInt64(tmp);
            std::cout << "receiver recver delta value=" << delta << std::endl;
            io.ReceiveBlocks(d.data(), sz);
            for (int i = 0; i < sz; i++) {
                __uint128_t tmp1 = mod((__uint128_t) a[i] * delta, pr);
                tmp1 = mod(tmp1 + Block::TO__uint128_t(d[i]), pr);

                if (tmp1 != Block::TO__uint128_t(b[i])) {
                    std::cout << "wrong triple " << i << std::endl;
                    abort();
                }
            }
            std::cout << "pass check" << std::endl;

        }
        std::cout << "receiver check ending" << std::endl;
    }

    void Sender(NetIO &io, PP &pp) {
        std::cout << "------sender ---- " << std::endl;
        COPE::initSender(io, pp);
        COPE::extend(io, pp.mac, pp.test_n, pp);
        std::vector<uint64_t> ans;
        ans.push_back(Block::BlockToInt64(pp.delta));
        COPE::check_triple(io, ans, pp.mac, pp.test_n, pp);

    }

    void Receiver(NetIO &io, PP &pp) {
        std::cout << "------receiver ---- " << std::endl;
        COPE::initRecver(io, pp);
        std::vector<uint64_t> u;
        u.resize(pp.test_n);
        PRG::Seed seed = PRG::SetSeed();
        auto ans = PRG::GenRandomBlocks(seed, pp.test_n / 2);
        for (int i = 0; i < pp.test_n / 2; i++) {
            u[i] = Block::BlockToInt64(ans[i]);
        }

        std::cout << u[0] << std::endl;
        for (int i = 0; i < pp.test_n; ++i) {
            u[i] = mod(u[i]);
        }
        std::cout << u[0] << std::endl;
        COPE::extend(io, pp.mac, u, pp.test_n, pp);
        std::cout << "start check recver" << std::endl;
        COPE::check_triple(io, u, pp.mac, pp.test_n, pp);
        std::cout << "test successfully" << std::endl;

    }


}
#endif //KUNLUN_COPE_H
