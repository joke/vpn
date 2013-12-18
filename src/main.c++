#define BOOST_BIND_NO_PLACEHOLDERS
#include <functional>

namespace boost {
	// include std::placeholders (c++11) in boost namespace
	using namespace std::placeholders;
}

#include "configuration.h++"
#include "server.h++"

#include <string>
#include <system_error>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/local/stream_protocol.hpp>

int main(int const argc, char const* const* const argv) {
	using namespace boost::asio;
	using namespace std;
	using namespace cfg;

	parse_command_line(argc, argv);

	if (parameters["mode"].as<string>() == "server") {
// 		try {
			// create server
			if (parameters["threads"].as<size_t>() > 0)
				server<ip::udp, local::stream_protocol, true> server;
			else
				server<ip::udp, local::stream_protocol, false> server;
// 		} catch (error_code const& e) {
// 			cerr << e.message() << endl;
// 			exit(EXIT_FAILURE);
// 		} catch (exception const& e) {
// 			cerr << e.what() << endl;
// 			exit(EXIT_FAILURE);
// 		} catch (...) {
// 			exit(EXIT_FAILURE);
// 		}
	}
}
