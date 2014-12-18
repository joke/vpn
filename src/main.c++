#define BOOST_BIND_NO_PLACEHOLDERS
#include <functional>

namespace boost {
	// include std::placeholders (c++11) in boost namespace
	using namespace std::placeholders;
}

#include "program_options.h++"
#include "dccp.h++"
#include "sctp.h++"
#include "gateway.h++"
#include <iostream>
#include "server.h++"

int main(int const argc, char const* const* const argv) {
	using namespace boost::asio;
	using namespace boost::asio::ip;
	using namespace std;
	using namespace cfg;

	parse_command_line(argc, argv);

	server<tcp, dccp, sctp> server(
		configuration["threads"].as<size_t>(),
		*(configuration["key"].as<std::shared_ptr<gnutls::credentials>>()),
		configuration["tcp"].as<vector<tcp::endpoint>>(),
		configuration["dccp"].as<vector<dccp::endpoint>>(),
		configuration["sctp"].as<vector<sctp::endpoint>>()
	);

	if (!configuration["c-dccp"].empty())
		server.connect(configuration["c-dccp"].as<vector<dccp::endpoint>>());
	if (!configuration["c-sctp"].empty())
		server.connect(configuration["c-sctp"].as<vector<sctp::endpoint>>());
	if (!configuration["c-tcp"].empty())
		server.connect(configuration["c-tcp"].as<vector<tcp::endpoint>>());

	server.run();

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
