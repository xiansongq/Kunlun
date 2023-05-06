/***********************************************************************************
this hpp file implements the core code of Sealpir namespace SEALPIR
SEALPIR can be executed for keyword-based queries through network communication protocols
************************************************************************************
* @author      XianSong
* @paper      https://eprint.iacr.org/2017/1142.pdf
***********************************************************************************/


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

        /*
         *    add keyword query
         *    use a vector to save mapping of indexs and keywords

         * */
        /*First clinet need to read file for KeyWord*/
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
        /*Second client receive file for Server for mapping of indexs and keywords */
        block a;
        /*Recive file size */
        io.ReceiveBlock(a);
        int server_file_len = Block::BlockToInt64(a);
        /*Receive file*/
        std::vector<block> index_num;

        std::cout << "Start receive file data" << std::endl;
        for (int i = 0; i < server_file_len; i++) {
            io.ReceiveBlock(a);
            index_num.push_back(a);
        }
        std::cout << "receive file finish" << std::endl;
        //use map to save mapping indexs of keywords in order to achieve fast finding
        // construct map
        std::map<std::uint64_t, std::uint64_t> m;
        // data insert map
        for (auto a: index_num) {
            std::uint64_t index = Block::BlockToUint64High(a);
            std::uint64_t keyword = Block::BlockToInt64(a);
            m[keyword] = index;
        }
        // find index by keyword and save data to block
        for (int i = 0; i < index_data.size(); i++) {
            std::map<std::uint64_t, std::uint64_t>::iterator it = m.find(
                    Block::BlockToInt64(index_data[i]));  // 在 std::map 容器中查找 key 对应的 value
            if (it != m.end()) {
                block temp = Block::MakeBlock(it->second, Block::BlockToInt64(index_data[i]));
                index_data[i] = temp;

            }
        }
        auto start_time = std::chrono::steady_clock::now();
        /*The preprocessing of the files has been completed */
        cout << "Main: Generating galois keys for client" << endl;
        /*create GaloisKeys*/
        GaloisKeys galois_keys = client.generate_galois_keys();
        std::ostringstream output;
        /*  Serialization  GaloisKeys sending by network */
        galois_keys.save(output);
        /*Send GaloisKeys size*/
        io.SendBlock(Block::MakeBlock(0LL, output.str().size()));
        /*Send Galoiskeys*/
        std::string temp = output.str();
        io.SendString(temp);

        std::vector<std::vector<uint8_t>> ans;
        /*Used to indicate whether the query is terminated. When the server-side receives 0,
         * it means the query is finished and the network communication is disconnected*/
        std::string isend = "1";
        io.SendString(isend);
        for (int i = 0; i < index_data.size(); i++) {

            /*Generate query*/
            uint64_t ele_index = Block::BlockToUint64High(index_data[i]);
            uint64_t index = client.get_fv_index(ele_index);   // index of FV plaintext
            uint64_t offset = client.get_fv_offset(ele_index); // offset in FV plaintext
            /*Serialization query sending by network*/
            stringstream client_stream;
            int query_size = client.generate_serialized_query(index, client_stream);
            /*First send query size to server*/
            io.SendBlock(Block::MakeBlock(0LL, query_size));
            std::cout << "query_size" << query_size << std::endl;
            std::string str = client_stream.str();
            /*Send query*/
            io.SendString(str);
            block b;
            io.ReceiveBlock(b);
            int reply_size = Block::BlockToInt64(b);
            std::string str_reply(reply_size, '0');
            /*Receive Reply from server*/
            io.ReceiveString(str_reply);
            stringstream sreply(str_reply);
            /* Construct SEALContext*/
            seal::SEALContext context(enc_params);
            PirReply reply = client.deserialize_reply(sreply, context);
            vector<uint8_t> elems = client.decode_reply(reply, offset);
            ans.push_back(elems);
            if (i != index_data.size() - 1)
                isend = "1";
            else isend = "0";
            io.SendString(isend);
        }
        isend = "0";
        io.SendString(isend);
        auto end_time = std::chrono::steady_clock::now();
        auto running_time = end_time - start_time;
        std::cout << "SealPIR:Client side takes time = "
                  << std::chrono::duration<double, std::milli>(running_time).count() << " ms" << std::endl;

        return ans;
    }

    void Server(NetIO &io, PirParams params, const EncryptionParameters &enc_params, std::string filename) {
        /*, unique_ptr<uint8_t[]> &db1*/
        PIRServer server(enc_params, params);
        /*read file*/
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
            row.push_back(to_string(i++));   /*add index colunms*/
            size_t pos = 0;
            string token;
            /* Split each row of data into multiple columns.*/
            while ((pos = line.find(",")) != string::npos) {
                token = line.substr(0, pos);
                row.push_back(token);
                line.erase(0, pos + 1);
            }
            row.push_back(line);
            file_data.push_back(row); /*save data*/

        }
        fin.close();
        /*save index and keyword to block */
        std::vector<block> index;
        for (int i = 0; i < file_data.size(); i++) {
            block a = Block::MakeBlock(std::stoull(file_data[i][0]), std::stoull(file_data[i][1]));
            index.push_back(a);
        }
        std::cout << "Send file size" << std::endl;
        io.SendBlock(Block::MakeBlock(0LL, index.size()));
        std::cout << "The file length has been sent successfully" << std::endl;
        std::cout << "start send file data" << std::endl;
        for (auto a: index) {
            io.SendBlock(a);
        }
        std::cout << "file data send successfully" << std::endl;
        auto start_time = std::chrono::steady_clock::now();
        block c;
        io.ReceiveBlock(c);
        int gas_len = Block::BlockToInt64(c);
        std::string gas_key(gas_len, '0');
        io.ReceiveString(gas_key);
        GaloisKeys *galois_keys1 = deserialize_galoiskeys(gas_key, server.context_);
        server.set_galois_key(0, *galois_keys1);
        /*Before generating the database file, it is necessary to remove the special
         * symbol '-' from the second column of the raw data*/
        for (int i = 0; i < file_data.size(); i++) {
            std::string temp = file_data[i][2];
            temp.erase(std::remove(temp.begin(), temp.end(), '-'), temp.end());
            file_data[i][2] = temp;
        }
        /*Generate database */
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

        std::string isend(1, '0');
        io.ReceiveString(isend);
        while (isend == "1") {

            block a;
            io.ReceiveBlock(a);
            int query_size = Block::BlockToInt64(a);
            std::cout << "rec_query_size " << query_size << std::endl;

            std::string query(query_size, '0');
            io.ReceiveString(query);
            std::stringstream server_stream;
            std::stringstream server_stream1(query);
            PirQuery query2 = server.deserialize_query(server_stream1);

            PirReply reply = server.generate_reply(query2, 0);

            int reply_size = server.serialize_reply(reply, server_stream);

            io.SendBlock(Block::MakeBlock(0LL, reply_size));
            std::string str1 = server_stream.str();

            io.SendString(str1);
            io.ReceiveString(isend);
            //if (isend == "0") return;
        }
        auto end_time = std::chrono::steady_clock::now();
        auto running_time = end_time - start_time;
        std::cout << "SealPIR:Server side takes time = "
                  << std::chrono::duration<double, std::milli>(running_time).count() << " ms" << std::endl;

    }


}


#endif