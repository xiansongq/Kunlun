#include "../netio/stream_channel.hpp"
#include "../mpc/psi/psi_from_oprf.hpp"
#include "../crypto/setup.hpp"
#include <thread>

void test_client(NetIO& client) {
    std::vector<block> vecy;
    for(int i=0;i<1024;i++) vecy.push_back(Block::MakeBlock('0LL',i));
    OTEOPRF::PP pp;
    pp=OTEOPRF::Setup(10,40);
    OPRFPSI::Send(client,pp,vecy);

    std::string message = "this is a test network";
    std::size_t len = message.size();
    std::cout << len << std::endl;
    block a = Block::MakeBlock(0LL, len);
    client.SendBlock(a);
    client.SendString(message);
    client.DeleteIO();
}

void test_server(NetIO& server) {
    std::vector<block> vecx;
    for(int i=100;i<1124;i++) vecx.push_back(Block::MakeBlock('0LL',i));
    OTEOPRF::PP pp;
    pp=OTEOPRF::Setup(10,40);
    OPRFPSI::Receive(server,pp,vecx);
    block a;
    server.ReceiveBlock(a);
    std::cout << "len " << Block::BlockToInt64(a) << std::endl;
    uint64_t len = Block::BlockToInt64(a);
    std::string str(len, '0');
    server.ReceiveString(str);
    std::cout << "str " << str << std::endl;
    server.DeleteIO();
}

void test_netio(std::string party) {
    NetIO io(party, "127.0.0.1", 8085);

    if (party == "server") {
        test_server(io);
    }

    if (party == "client") {
        test_client(io);
    }

    //io.DeleteIO();
}

int main() {
    std::string party;
    CRYPTO_Initialize();
    for (int i = 0; i < 2; i++) {
        std::thread serverthread(test_netio, "server");
        std::thread clientthread(test_netio, "client");
        serverthread.join();
        clientthread.join();
    }
    CRYPTO_Finalize();
    return 0;
}
