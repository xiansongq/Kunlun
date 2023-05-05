/***********************************************************************************
this hpp file implements the core code of Sealpir namespace SEALPIR
SEALPIR can be executed for index-based queries through network communication protocols
************************************************************************************
* @author     XianSong
* @paper      https://eprint.iacr.org/2017/1142.pdf
***********************************************************************************/

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
        auto start_time = std::chrono::steady_clock::now();
        // Initialize PIR client....
        PIRClient client(enc_params, params);

        cout << "Main: Generating galois keys for client" << endl;
        //create GaloisKeys
        GaloisKeys galois_keys = client.generate_galois_keys();
        std::ostringstream output;
        // Serialization  GaloisKeys sending by network
        galois_keys.save(output);
        //First send GaloisKeys length to Server
        io.SendBlock(Block::MakeBlock(0LL, output.str().size()));
        //send GaloisKeys
        std::string temp = output.str();
        io.SendString(temp);
        // generation query
        uint64_t index = client.get_fv_index(ele_index);   // index of FV plaintext
        uint64_t offset = client.get_fv_offset(ele_index); // offset in FV plaintext
        //Serialization query sending by network
        stringstream client_stream;
        int query_size = client.generate_serialized_query(index, client_stream);
        //First send query size to server
        io.SendBlock(Block::MakeBlock(0LL, query_size));
        std::cout << "query_size" << query_size << std::endl;
        std::string str = client_stream.str();
        // Send query
        io.SendString(str);
        // Receive Reply from server
        block b;
        // First receiver  Reply size
        io.ReceiveBlock(b);
        int reply_size = Block::BlockToInt64(b);
        std::string str_reply(reply_size, '0');
        //receiver Reply
        io.ReceiveString(str_reply);
        stringstream sreply(str_reply);
        //Construct SEALContext
        seal::SEALContext context(enc_params);

        // Deserialization  Reply
        PirReply reply = client.deserialize_reply(sreply, context);
        //Decode Reply
        vector<uint8_t> elems = client.decode_reply(reply, offset);
        auto end_time = std::chrono::steady_clock::now();
        auto running_time = end_time - start_time;
        std::cout << "SealPIR:Client side takes time = "
                  << std::chrono::duration<double, std::milli>(running_time).count() << " ms" << std::endl;

        return elems;
    }

    void Server(NetIO &io, PirParams params, const EncryptionParameters &enc_params, unique_ptr<uint8_t[]> &db) {
        auto start_time = std::chrono::steady_clock::now();
        // Initialize PIR Server....
        PIRServer server(enc_params, params);
        // Receive GaloisKeys size
        block c;
        io.ReceiveBlock(c);
        int gas_len = Block::BlockToInt64(c);
        std::string gas_key(gas_len, '0');
        // Receive GaloisKeys
        io.ReceiveString(gas_key);
        GaloisKeys *galois_keys1 = deserialize_galoiskeys(gas_key, server.context_);
        server.set_galois_key(0, *galois_keys1);
        // Measure database setup
        cout << "Main: database pre processed " << endl;
        server.set_database(move(db), params.ele_num, params.ele_size);
        server.preprocess_database();
        //Receive query size
        block a;
        io.ReceiveBlock(a);
        int query_size = Block::BlockToInt64(a);
        std::cout << "rec_query_size " << query_size << std::endl;
        //Receive query
        std::string query(query_size, '0');
        io.ReceiveString(query);
        std::stringstream server_stream;
        std::stringstream server_stream1(query);
        PirQuery query2 = server.deserialize_query(server_stream1);
        //Generate Reply
        PirReply reply = server.generate_reply(query2, 0);
        //Serialization Reply send to network
        int reply_size = server.serialize_reply(reply, server_stream);
        //First send Reply size
        io.SendBlock(Block::MakeBlock(0LL, reply_size));
        std::string str1 = server_stream.str();
        //Send Reply
        io.SendString(str1);
        auto end_time = std::chrono::steady_clock::now();
        auto running_time = end_time - start_time;
        std::cout << "SealPIR:Server side takes time = "
                  << std::chrono::duration<double, std::milli>(running_time).count() << " ms" << std::endl;
    }
}


#endif