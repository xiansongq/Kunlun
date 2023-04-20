
#ifndef KUNLUN_SEALPIR_HPP_
#define KUNLUN_SEALPIR_HPP_


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
namespace SEALPIR {

    std::vector<uint8_t>
    Clinet(NetIO &io, PirParams params, const EncryptionParameters &enc_params, std::uint8_t ele_index) {
        // Initialize PIR client....
        PIRClient client(enc_params, params);
        /*添加关键字查询*/
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
        // 生成查询
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
        //vector<uint8_t> elems = client.extract_bytes(client.decrypt(reply), offset);
        return elems;
    }

    void Server(NetIO &io, PirParams params, const EncryptionParameters &enc_params, unique_ptr<uint8_t[]> &db) {
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
        int i=0;
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
        for( auto a : file_data){
            for(auto b:a) std::cout<<b<<" "<<std::endl;
        }
        // 将行号和 关键字 存进block
        std::vector<block> index;
        for( int i=0;i<file_data.size();i++){
            block a=Block::MakeBlock(std::stoull(file_data[i][0]),std::stoull(file_data[i][1]));
            index.push_back(a);
        }
        io.SendBlocks(index.data(),1000000);




        block c;
        io.ReceiveBlock(c);
        int gas_len = Block::BlockToInt64(c);
        std::string gas_key(gas_len, '0');
        io.ReceiveString(gas_key);
        GaloisKeys *galois_keys1 = deserialize_galoiskeys(gas_key, server.context_);
        server.set_galois_key(0, *galois_keys1);
        // Measure database setup
        cout << "Main: database pre processed " << endl;
        server.set_database(move(db), params.ele_num, params.ele_size);
        server.preprocess_database();
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

    }


}


#endif