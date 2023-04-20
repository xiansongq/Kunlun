
#ifndef KUNLUN_SEALPIRKEYWORD_HPP_
#define KUNLUN_SEALPIRKEYWORD_HPP_


#include "seal/seal.h"
#include "seal/util/polyarithsmallmod.h"
#include "../../netio/stream_channel.hpp"
#include "pir.hpp"
#include "pir_client.hpp"
#include "pir_server.hpp"
#include <cassert>
#include <cmath>
#include <string>
#include <vector>


using namespace seal;
using namespace seal::util;
namespace SEALPIRKEYWORD {

    std::vector<std::vector<uint8_t>>
    Clinet(NetIO &io, PirParams params, const EncryptionParameters &enc_params, std::string filename) {
        // Initialize PIR client....
        PIRClient client(enc_params, params);
        /*添加关键字查询*/
        /*
         *    使用一个 vector 二维数组来存储 index和 keyword的映射关系
         *
         *
         * */
        //读取待查询关键字文件
        std::ifstream fin;
        std::vector<block> index_data;
        fin.open("A_PIR_ID.txt", std::ios::binary);
        if (!fin) {
            std::cout << "Failed to open file" << std::endl;
            std::exit(-1);
        }
        std::string line;
        int i = 0;
        while (std::getline(fin, line)) {
            index_data.push_back(Block::MakeBlock(0LL, std::stoull(line)));
        }
        fin.close();
        //先接收服务端文件的长度
        block a;
        io.ReceiveBlock(a);
        int server_file_len = Block::BlockToInt64(a);
        // 从服务端接收 映射文件
        std::vector<block> index_num;

        std::cout << "开始接收数据" << std::endl;
        for (int i = 0; i < server_file_len; i++) {
            io.ReceiveBlock(a);
            index_num.push_back(a);
        }
        std::cout << "接收数据完成" << std::endl;
        //查找对应的映射文件   但是直接用遍历查询的话 估计是很慢的 需要一个优化的办法
        // 1、构造map
        std::map<std::uint64_t, std::uint64_t> m;
        // 将数据存入map
        for (auto a: index_num) {
            std::uint64_t index = Block::BlockToUint64High(a);
            // std::cout<<"index: "<<index<<std::endl;
            std::uint64_t keyword = Block::BlockToInt64(a);
            m[keyword] = index;
        }
        // 执行关键字查找并 保存到block
        for (int i = 0; i < index_data.size(); i++) {
            std::map<std::uint64_t, std::uint64_t>::iterator it = m.find(
                    Block::BlockToInt64(index_data[i]));  // 在 std::map 容器中查找 key 对应的 value

            if (it != m.end()) {

                block temp = Block::MakeBlock(it->second, Block::BlockToInt64(index_data[i]));
                index_data[i] = temp;

            }
        }
//        for (auto a: index_data) {
//            std::cout << Block::BlockToUint64High(a) << "-----" << Block::BlockToInt64(a) << std::endl;
//        }
        // 到此 客户端前期数据准备和处理完成
        // 发送 密钥
        cout << "Main: Generating galois keys for client" << endl;
        //创建 GaloisKeys
        GaloisKeys galois_keys = client.generate_galois_keys();
        std::ostringstream output;
        // 序列化 进行网络传输
        galois_keys.save(output);
        //先传输 其长度
        io.SendBlock(Block::MakeBlock(0LL, output.str().size()));
        //传输 GaloisKeys
        std::string temp = output.str();
        io.SendString(temp);
        /* 生成查询部分 需要另外处理
         * */
        std::vector<std::vector<uint8_t>> ans;
        /*用来标志 查询是否结束 当server端接收到 0的时候 说明查询结束 断开网络通信*/
        std::string isend="1";
        io.SendString(isend);
        for (int i = 0; i < index_data.size(); i++) {

            // 生成查询
            uint64_t ele_index = Block::BlockToUint64High(index_data[i]);
            uint64_t index = client.get_fv_index(ele_index);   // index of FV plaintext
            uint64_t offset = client.get_fv_offset(ele_index); // offset in FV plaintext
            //序列化 查询
            stringstream client_stream;
            int query_size = client.generate_serialized_query(index, client_stream);
            //先传输 query_size
            io.SendBlock(Block::MakeBlock(0LL, query_size));
            std::cout << "query_size" << query_size << std::endl;
            std::string str = client_stream.str();
            // 传输查询字符流
            io.SendString(str);
            //接收回复
            // 先接收回复的长度
            block b;
            io.ReceiveBlock(b);
            int reply_size = Block::BlockToInt64(b);
            std::string str_reply(reply_size, '0');
            //接收回复
            io.ReceiveString(str_reply);
            stringstream sreply(str_reply);
            //构造SEALContext
            seal::SEALContext context(enc_params);
            PirReply reply = client.deserialize_reply(sreply, context);
            vector<uint8_t> elems = client.decode_reply(reply, offset);
            ans.push_back(elems);
            if(i!=index_data.size()-1)
            isend="1";
            else isend="0";
            io.SendString(isend);
        }
        isend="0";
        io.SendString(isend);
        return ans;
    }

    void Server(NetIO &io, PirParams params, const EncryptionParameters &enc_params, std::string filename) {
        /*, unique_ptr<uint8_t[]> &db1*/
        PIRServer server(enc_params, params);

        /*添加关键字和行号的关联 查询*/
        //从文件读取数据
        std::ifstream fin;
        std::vector<std::vector<std::string>> file_data;
        fin.open("B_PIR_DATA.txt", std::ios::binary);
        if (!fin) {
            std::cout << "Failed to open file" << std::endl;
            std::exit(-1);
        }
        std::string line;
        int i = 0;
        while (std::getline(fin, line)) {
            vector<string> row;
            row.push_back(to_string(i++));
            size_t pos = 0;
            string token;
            // 将每行数据拆分为多个列
            while ((pos = line.find(",")) != string::npos)  //还有第二列数据 存在”-“ 符号的问题没有解决
            {
                token = line.substr(0, pos);
                row.push_back(token);
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            file_data.push_back(row); // 存储数据

        }
        fin.close();
        // 将行号和 关键字 存进block
        std::vector<block> index;
        for (int i = 0; i < file_data.size(); i++) {
            block a = Block::MakeBlock(std::stoull(file_data[i][0]), std::stoull(file_data[i][1]));
            index.push_back(a);
        }
        std::cout << "先发送文件长度" << std::endl;
        io.SendBlock(Block::MakeBlock(0LL, index.size()));
        std::cout << "长度发送完成" << std::endl;
        std::cout << "开始发送数据" << std::endl;
        for (auto a: index) {
            io.SendBlock(a);
        }
        std::cout << "数据发送完成" << std::endl;
        // 接收GaloisKeys  密钥 与客户端关联
        block c;
        io.ReceiveBlock(c);
        int gas_len = Block::BlockToInt64(c);
        std::string gas_key(gas_len, '0');
        io.ReceiveString(gas_key);
        GaloisKeys *galois_keys1 = deserialize_galoiskeys(gas_key, server.context_);
        server.set_galois_key(0, *galois_keys1);
        /*在生成数据库 文件前 需要将原始数据的第二列中的 特殊符号 ”-“ 去除*/
        for (int i = 0; i < file_data.size(); i++) {
            std::string temp = file_data[i][2];
            temp.erase(std::remove(temp.begin(), temp.end(), '-'), temp.end());
            file_data[i][2] = temp;
        }
        /*生成服务器端的数据库文件*/
        auto db(make_unique<uint8_t[]>(params.ele_num * params.ele_size));
        for (int i = 0; i < file_data.size(); i++) {
            std::string str = file_data[i][1] + file_data[i][2] + file_data[i][3];
            for (int j = 0; j < str.size(); j++) {
                db.get()[(i * str.size()) + j] = str[j] - '0';
            }
        }


        // Measure database setup
        cout << "Main: database pre processed " << endl;
        server.set_database(move(db), params.ele_num, params.ele_size);
        server.preprocess_database();

        std::string isend(1,'0');  //客服开始查询标志
        io.ReceiveString(isend);
        while(isend=="1"){
        //接收查询
        //接收查询 的长度
        block a;
        io.ReceiveBlock(a);
        int query_size = Block::BlockToInt64(a);
        std::cout << "rec_query_size " << query_size << std::endl;
        //接收查询字符流
        std::string query(query_size, '0');
        io.ReceiveString(query);
        std::stringstream server_stream;
        std::stringstream server_stream1(query);
        PirQuery query2 = server.deserialize_query(server_stream1);
        //答复
        PirReply reply = server.generate_reply(query2, 0);
        //序列化答复 用于网络传输
        int reply_size = server.serialize_reply(reply, server_stream);
        //传输 答复长度
        io.SendBlock(Block::MakeBlock(0LL, reply_size));
        std::string str1 = server_stream.str();
        //传输回复
        io.SendString(str1);
        io.ReceiveString(isend);
        if(isend=="0") return ;
        }
    }


}


#endif