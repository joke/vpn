#define BOOST_BIND_NO_PLACEHOLDERS
#include <functional>

namespace boost {
	// include std::placeholders (c++11) in boost namespace
	using namespace std::placeholders;
}

#include "program_options.h++"

// #include <string>
// #include <sstream>
// #include <boost/asio/ip/address.hpp>
// #include <boost/asio/ip/udp.hpp>
// #include <boost/asio/local/stream_protocol.hpp>
//
// #include "encryption.h++"
// #include <iostream>
// #include <cstdio>
// #include <memory>
// #include <thread>
// #include <functional>
#include "dccp.h++"
#include "sctp.h++"
#include <iostream>

#include "gateway.h++"

int main(int const argc, char const* const* const argv) {
	using namespace boost::asio;
	using namespace boost::asio::ip;
	using namespace std;
	using namespace cfg;

	parse_command_line(argc, argv);

	boost::asio::io_service io;
	gateway<dccp, sctp, tcp> gateway(
		io,
		configuration["dccp"].as<vector<dccp::endpoint>>(),
		configuration["sctp"].as<vector<sctp::endpoint>>(),
		configuration["tcp"].as<vector<tcp::endpoint>>()
	);

	io.run();

// 	boost::asio::io_service io;

// 	if (configuration["mode"].as<string>() == "server") {
// 		try {
// 			// create server
// 			server<ip::udp, local::stream_protocol> server{io};
// 		} catch (error_code const& e) {
// 			cerr << e.message() << endl;
// 			exit(EXIT_FAILURE);
// 		} catch (exception const& e) {
// 			cerr << e.what() << endl;
// 			exit(EXIT_FAILURE);
// 		} catch (...) {
// 			exit(EXIT_FAILURE);
// 		}
// 	} else if (configuration["mode"].as<string>() == "control") {
// 		local::stream_protocol::endpoint socket{configuration["socket"].as<string>()};
//
// 		// create client and parse commandline
// 		client<local::stream_protocol> client{io, socket};
//
// 		if (!configuration["node-del"].empty())
// 			for (auto node : configuration["node-del"].as<vector<string>>())
// 				client.node_del(node);
// 		if (!configuration["node-add"].empty())
// 			for (auto node : configuration["node-add"].as<vector<string>>())
// 				client.node_add(node);
// 		if (!configuration["node-list"].empty())
// 			client.node_list();
// 	}
}
