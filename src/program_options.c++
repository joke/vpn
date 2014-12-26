#include "program_options.h++"

#include <vector>
#include <cstdint>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include "encryption.h++"

#include "dccp.h++"
#include "sctp.h++"
#include "convert.h++"

namespace boost {
namespace asio {
namespace ip {

template <typename PROTOCOL>
void validate(any& v, std::vector<std::string> const& values, basic_endpoint<PROTOCOL>* const e, int const) {
	using namespace program_options;

	validators::check_first_occurrence(v);
	auto const option(validators::get_single_string(values));

	smatch result;
	regex const pattern(R"(^\[?(.*?)\]?:(\d+)$)", regex::perl);
	if (regex_match(option, result, pattern)) {
		v = any(basic_endpoint<PROTOCOL>(address::from_string(result[1].str()), std::atoi(result[2].str().data())));
	} else {
		throw validation_error(validation_error::invalid_option_value);
	}
}

} // namespace: ip
} // namespace: asio
} // namespace: boost

namespace gnutls {
// namespace {

void validate(boost::any& v, std::vector<std::string> const& values, std::shared_ptr<gnutls::priorities>* const e, int const) {
	using namespace boost;
	using namespace program_options;

	try {
		validators::check_first_occurrence(v);
		auto const option(validators::get_single_string(values));
		v = any(std::make_shared<gnutls::priorities>(option));
	} catch (std::exception const& e) {
		throw validation_error(validation_error::invalid_option_value);
	}
}

void validate(boost::any& v, std::vector<std::string> const& values, std::shared_ptr<gnutls::diffie_hellman>* const e, int const) {
	using namespace boost;
	using namespace program_options;

	try {
		validators::check_first_occurrence(v);
		auto const option(validators::get_single_string(values));
		v = any(std::make_shared<gnutls::diffie_hellman>(std::stol(option)));
	} catch (std::exception const& e) {
		throw validation_error(validation_error::invalid_option_value);
	}
}

void validate(boost::any& v, std::vector<std::string> const& values, std::shared_ptr<gnutls::credentials>* const e, int const) {
	using namespace boost;
	using namespace program_options;

	try {
		validators::check_first_occurrence(v);
		auto const option(validators::get_single_string(values));

		auto const public_key = gnupg::export_key(option, false);
		auto const private_key = gnupg::export_key(option, true);
		gnutls::openpgp::certificate cert(public_key);
		gnutls::openpgp::privatekey priv(private_key);

		v = any(std::make_shared<gnutls::credentials>(cert, priv));
	} catch (std::exception const& e) {
		throw validation_error(validation_error::invalid_option_value);
	}
}

// }
}

namespace cfg {

boost::program_options::variables_map configuration;

void parse_command_line(int const argc, char const* const* const argv) {
	using namespace std;
	using namespace boost::program_options;
	using namespace boost::asio::ip;

	options_description cmdline("commandline");
	cmdline.add_options()
		("help", "show help message")
		("config", value<string>(), "configuration file")
		("mode", value<string>()->required(), "mode to run in: server|control")
	;

	vector<dccp::endpoint>{{address_v6::any(), 54321}};

	options_description server("server mode options");
	server.add_options()
		("dccp", value<vector<dccp::endpoint>>()
// 			->composing()
			->default_value(vector<dccp::endpoint>(), "{}")
			->implicit_value(vector<dccp::endpoint>{{address_v6::any(), 54321}}, "[::]:54321"),
			"DCCP address and port to bind to. Can be specified multiple times.")
		("sctp", value<vector<sctp::endpoint>>()
// 			->composing()
			->default_value(vector<sctp::endpoint>(), "{}")
			->implicit_value(vector<sctp::endpoint>{{address_v6::any(), 54321}}, "[::]:54321"),
			"SCTP address and port to bind to. Can be specified multiple times.")
		("tcp", value<vector<tcp::endpoint>>()
// 			->composing()
			->default_value(vector<tcp::endpoint>(), "{}")
			->implicit_value(vector<tcp::endpoint>{{address_v6::any(), 54321}}, "[::]:54321"),
			"TCP address and port to bind to. Can be specified multiple times.")
		("c-sctp", value<vector<sctp::endpoint>>()
// 			->composing()
// 			->default_value(vector<dccp::endpoint>(), "{}")
			->implicit_value(vector<sctp::endpoint>{{address_v4::any(), 54321}}, "[::]:54321"),
			"DCCP address and port to bind to. Can be specified multiple times.")
		("c-dccp", value<vector<dccp::endpoint>>()
		// 			->composing()
		// 			->default_value(vector<dccp::endpoint>(), "{}")
		->implicit_value(vector<dccp::endpoint>{{address_v4::any(), 54321}}, "[::]:54321"),
		 "DCCP address and port to bind to. Can be specified multiple times.")
		("c-tcp", value<vector<tcp::endpoint>>()
		// 			->composing()
		// 			->default_value(vector<dccp::endpoint>(), "{}")
		->implicit_value(vector<tcp::endpoint>{{address_v4::any(), 54321}}, "[::]:54321"),
		 "DCCP address and port to bind to. Can be specified multiple times.")

// 		("socket", value<string>()->default_value("/var/run/vpn.sock"), "server socket to bind to")
		("device", value<string>()->default_value(""), "name of device")
// 		("autoconnect", "autoconnect to peers based on keyring")
// 		("advertize", value<vector<string>>(), "advertize prefix")
// 		("lan-connect", "enable ipv6 forwarding and configure devices (dangerous without firewall)")
		("threads", value<size_t>()->default_value(0), "the number of threads")
// 		("link-local", value<bool>()->default_value(false), "enable link local routing")
	;

	options_description encryption("encrytion options");
	encryption.add_options()
		("dh", value<shared_ptr<gnutls::diffie_hellman>>()
			->default_value(make_shared<gnutls::diffie_hellman>(1024)),
			"size of the Diffie-Hellman parameter")
		("priorities", value<shared_ptr<gnutls::priorities>>()
			->default_value(make_shared<gnutls::priorities>("NONE:+VERS-DTLS1.0:+CTYPE-OPENPGP:+KX-ALL:+CIPHER-ALL:+MAC-ALL:+CURVE-ALL:+COMP-ALL:+SIGN-ALL")),
			"list of TLS priorities used by GnuTLS")
		("gnutls-debug-level", value<int>()->default_value(0), "set gnutls debug level")
	;

	options_description gnupg("gnupg options");
	gnupg.add_options()
		("gpg", value<string>()->default_value("/usr/bin/gpg"), "path to GnuPG executable")
		("key", value<shared_ptr<gnutls::credentials>>()->required(), "gnupg key to use")
// 		("key", value<gnutls::priorities>(), "gnupg key")
// 		("key", value<string>()->required(), "private key to use")
// 		("subkey", value<string>()->default_value("auto"), "subkey to use")
// 		("import", "import certificates send by peers into keyring")
// 		("keyserver", "import (update) certificates from keyserver into keyring")
// 		("validity", value<uint8_t>()->default_value(3), "minimum validity required for authentication: 3: marginal, 4: full, 4: ultimate")
	;

	options_description controller("control mode options");
	controller.add_options()
		("socket", value<string>()->default_value("/var/run/vpn.sock"), "server socket to connect with")
		("node-list", "list all nodes")
		("node-add", value<vector<string>>(), "add node")
		("node-del", value<vector<string>>(), "delete node")
	;

	options_description groups;
	groups.add(cmdline);

	store(command_line_parser(argc, argv).options(cmdline).allow_unregistered().run(), configuration);

	if (!configuration["mode"].empty()) {
		if (configuration["mode"].as<string>() == "server")
			groups.add(server).add(encryption).add(gnupg);

		if (configuration["mode"].as<string>() == "control")
			groups.add(controller);

		if (!configuration["config"].empty())
			store(parse_config_file<char>(configuration["config"].as<string>().data(), groups, false), configuration);

		store(parse_command_line(argc, argv, groups), configuration);
	}

	if (configuration.count("help")) {
		cout << groups << endl;
		exit(EXIT_SUCCESS);
	}

	notify(configuration);
}

} // namespace cfg
