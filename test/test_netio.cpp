#include "../netio/stream_channel.hpp"

void test_client()
{
	NetIO client("client", "127.0.0.1", 8080);
    std::string message="this is a test network";
    //传输不定长字符串的方法
    //先传输字符串的长度
    std::size_t len= message.size();
    std::cout<<len<<std::endl;
//    std::string str= std::to_string(len);
//    std::cout<<str<<std::endl;
//    client.SendString(str);
    block a=Block::MakeBlock(0LL,len);
    client.SendBlock(a);
	//std::getline(std::cin, message);
    client.SendString(message);
	message = "hello";  
	client.SendString(&message[0],message.size());

	message = "world";
	client.SendString(&message[0],message.size());
}

void test_server()
{
	NetIO server("server", "", 8080);

    block a;
    server.ReceiveBlock(a);
    std::cout<<"len"<<Block::BlockToInt64(a)<<std::endl;
    uint64_t len=Block::BlockToInt64(a);
    std::string str(len,'0');
    server.ReceiveString(str);
    std::cout<<"str "<<str<<std::endl;
    //server.ReceiveString(str);
    //std::cout<<str<<std::endl;
    //std::size_t len=std::stoll(str);
    //std::cout<<str<<std::endl;
    //std::cout<<"----"<<len<<std::endl;
	//std::size_t len=1024;
	std::string message;

	server.ReceiveString(&message[0],message.size());
	std::cout << "message from client: " << message << std::endl; 

	server.ReceiveString(&message[0],5);
	std::cout << "message from client: " << message << std::endl; 
}

void test_netio(std::string party)
{
	if (party == "server")
	{
		test_server(); 
	}

	if (party == "client")
	{
		test_client(); 
	}

}

int main()
{
    std::string party; 

	std::cout << "please select your role (hint: first start server, then start the client) >>> "; 
    std::getline(std::cin, party); // first receiver (acts as server), then sender (acts as client)
	test_netio(party);

	return 0; 
}