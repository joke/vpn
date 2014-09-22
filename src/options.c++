#include "options.h++"

#include <vector>
#include <cstdint>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/asio/ip/address.hpp>

#include "convert.h++"
#include "encryption.h++"


namespace gnutls {

void validate(boost::any& v, std::vector<std::string> const& values, std::vector<std::shared_ptr<credentials>>*, int) {
	using namespace boost::program_options::validators;
	using namespace std;
	using any_type = vector<shared_ptr<credentials>>;

	if (v.empty())
		v = any_type();

	auto const& value(get_single_string(values));
	auto& any(boost::any_cast<any_type&>(v));

	openpgp::certificate certificate(value);
	openpgp::privatekey privatekey(value);
	any.emplace_back(make_shared<credentials>(std::move(certificate), std::move(privatekey)));

	v = move(any);
}

void validate(boost::any& v, std::vector<std::string> const& values, std::shared_ptr<diffie_hellman_parameters>*, int) {
	using namespace boost::program_options::validators;
	using namespace std;
	using any_type = shared_ptr<diffie_hellman_parameters>;

	check_first_occurrence(v);
	auto const& value(get_single_string(values));
	v = make_shared<diffie_hellman_parameters>(std::stoi(value));
}

void validate(boost::any& v, std::vector<std::string> const& values, std::shared_ptr<priorities>*, int) {
	using namespace boost::program_options::validators;
	using namespace std;
	using any_type = shared_ptr<priorities>;

	check_first_occurrence(v);
	auto const& value(get_single_string(values));
	v = move(make_shared<priorities>(value));
}

} // namespace: gnutls



namespace cfg {

boost::program_options::variables_map parameters;

void parse_command_line(int const argc, char const* const* const argv) {
	using namespace boost::program_options;
	using namespace std;
	using namespace gnutls;


	options_description cmdline("commandline");
	cmdline.add_options()
		("help", "show help message")
		("config", value<string>(), "parameters file")
		("mode", value<string>()->required(), "mode to run in: server|control")
		("verbose-gnutls", value<unsigned short>()->default_value(0), "set gnutls debug level")
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
		("dh", value<shared_ptr<diffie_hellman_parameters>>()
			->required()
			->default_value(make_shared<diffie_hellman_parameters>(1024), "1024"),
			"size of the Diffie-Hellman parameter")
		("priorities,p", value<shared_ptr<priorities>>()
			->required()
			->default_value(make_shared<priorities>("NONE:+VERS-DTLS-ALL:+CTYPE-OPENPGP:+PFS:+COMP-ALL"), "NONE:+VERS-DTLS-ALL:+CTYPE-OPENPGP:+PFS:+COMP-ALL"),
			"gnutls priorities")
		("cookie", value<size_t>()->default_value(1024), "size of the session cookie")
	;

	options_description gnupg("gnupg options");
	gnupg.add_options()
		("gpg", value<string>()->default_value("/usr/bin/gpg"), "path to GnuPG executable")
		("key,k", value<vector<shared_ptr<credentials>>>()->required(), "private key to use")
// 		("subkey", value<string>()->default_value("auto"), "subkey to use")
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

	gnutls::global::debug(parameters["verbose-gnutls"].as<unsigned short const>());

	if (!parameters["mode"].empty()) {
		if (parameters["mode"].as<string>() == "server")
			groups.add(server).add(encryption).add(gnupg);

		if (parameters["mode"].as<string>() == "control")
			groups.add(controller);

		if (!parameters["config"].empty())
			store(parse_config_file<char>(parameters["config"].as<string>().c_str(), groups, false), parameters);

		store(parse_command_line(argc, argv, groups), parameters);
	}

	for (auto const& key : parameters["key"].as<vector<shared_ptr<credentials>> const&>()) {
		key->set(*parameters["dh"].as<shared_ptr<diffie_hellman_parameters> const&>());
	}

	if (parameters.count("help")) {
		cout << groups << endl;
		exit(EXIT_FAILURE);
	}

	notify(parameters);
}

} // namespace cfg
