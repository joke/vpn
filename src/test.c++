#include <iostream>
#include <boost/program_options.hpp>


#include <vector>

#include "dccp.h++"
#include <boost/asio.hpp>

int main() {
	using namespace std;
	boost::asio::io_service io;
	using namespace boost::asio;
	using namespace ip;


	dccp::endpoint endp(ip::address::from_string("127.0.0.1"), 54321);
	socket_base::message_flags flags;
	dccp::socket sock(io);

	sock.connect(endp);
	sock.send(buffer("asdf"), flags);


}
