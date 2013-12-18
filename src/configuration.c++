#include "configuration.h++"

#include <vector>
#include <cstdint>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/asio/ip/address.hpp>

#include "convert.h++"

namespace cfg {

boost::program_options::variables_map parameters;

void parse_command_line(int const argc, char const* const* const argv) {
	using namespace std;
	using namespace boost::program_options;

	options_description cmdline("commandline");
	cmdline.add_options()
		("help", "show help message")
		("config", value<string>(), "parameters file")
		("mode", value<string>()->required(), "mode to run in: server|control")
	;

	options_description server("server mode options");
	server.add_options()
		("bind", value<string>()->default_value("::"), "address to bind to")
		("port", value<uint16_t>()->default_value(45678), "port or service to listen on")
		("socket", value<string>()->default_value("/var/run/vpn.sock"), "server socket to bind to")
		("device", value<string>()->default_value(""), "name of device")
		("autoconnect", "autoconnect to peers based on keyring")
		("advertize", value<vector<string>>(), "advertize prefix")
		("lan-connect", "enable ipv6 forwarding and configure devices (dangerous without firewall)")
		("threads", value<size_t>()->default_value(0), "the number of threads")
		("link-local", value<bool>()->default_value(false), "enable link local routing")
	;

	options_description encryption("encrytion options");
	encryption.add_options()
		("dh", value<size_t>()->default_value(1024), "size of the Diffie-Hellman parameter")
		("priorities", value<string>()->default_value("NONE:+VERS-DTLS1.0:+CTYPE-OPENPGP:+KX-ALL:+CIPHER-ALL:+MAC-ALL:+CURVE-ALL:+COMP-ALL:+SIGN-ALL"), "gnutls priorities")
		("cookie", value<size_t>()->default_value(1024), "size of the session cookie")
		("gnutls-debug-level", value<int>()->default_value(0), "set gnutls debug level")
	;

	options_description gnupg("gnupg options");
	gnupg.add_options()
		("gpg", value<string>()->default_value("/usr/bin/gpg"), "path to GnuPG executable")
		("key", value<string>()->required(), "private key to use")
		("subkey", value<string>()->default_value("auto"), "subkey to use")
		("import", "import certificates send by peers into keyring")
		("keyserver", "import (update) certificates from keyserver into keyring")
		("validity", value<uint8_t>()->default_value(3), "minimum validity required for authentication: 3: marginal, 4: full, 4: ultimate")
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

	store(command_line_parser(argc, argv).options(cmdline).allow_unregistered().run(), parameters);

	if (!parameters["mode"].empty()) {
		if (parameters["mode"].as<string>() == "server")
			groups.add(server).add(encryption).add(gnupg);

		if (parameters["mode"].as<string>() == "control")
			groups.add(controller);

		if (!parameters["config"].empty())
			store(parse_config_file<char>(parameters["config"].as<string>().c_str(), groups, false), parameters);

		store(parse_command_line(argc, argv, groups), parameters);
	}

	if (parameters.count("help")) {
		cout << groups << endl;
		exit(EXIT_FAILURE);
	}

	notify(parameters);
}

} // namespace cfg
