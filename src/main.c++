#define BOOST_BIND_NO_PLACEHOLDERS
#include <functional>

namespace boost {

// include std::placeholders (c++11) in boost namespace
using namespace std::placeholders;

} // namespace: boost

#include "options.h++"
#include "server.h++"

#include "encryption.h++"

#include <string>
#include <system_error>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/local/stream_protocol.hpp>

int main(int const argc, char const* const* const argv) {
	using namespace boost::asio;
	using namespace std;
	using namespace cfg;

	parse_command_line(argc, argv);


	using session_builder_type = server<ip::udp, local::stream_protocol, SingleThreaded>::session_builder_type;


		try {
			// create server

			if (parameters["mode"].as<string>() == "server") {
				if (parameters["threads"].as<size_t>() > 1)
					server<ip::udp, local::stream_protocol, ThreadPool> server(
						std::move(*parameters["key"].as<vector<shared_ptr<gnutls::credentials>> const&>().front()),
						std::move(*parameters["priorities"].as<shared_ptr<gnutls::priorities> const&>())
					);
				else
					server<ip::udp, local::stream_protocol, SingleThreaded> server(
						std::move(*parameters["key"].as<vector<shared_ptr<gnutls::credentials>> const&>().front()),
						std::move(*parameters["priorities"].as<shared_ptr<gnutls::priorities> const&>())
					);
			}

		} catch (error_code const& e) {
			cerr << e.message() << endl;
			exit(EXIT_FAILURE);
		} catch (exception const& e) {
			cerr << e.what() << endl;
			exit(EXIT_FAILURE);
		} catch (...) {
			exit(EXIT_FAILURE);
		}

}
