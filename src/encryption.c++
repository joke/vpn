#include "encryption.h++"
#include "convert.h++"

#include <boost/asio.hpp>
#include <cstdio>

namespace gnutls {

global* global::global_(new global());

} // namespace: gnutls

namespace gnupg {


std::vector<std::uint8_t> export_key(std::string const& id, bool private_key) {
	using namespace std;
	using namespace boost::asio;

	stringstream command;
	command << "/usr/bin/gpg2 --batch --no-tty --no --openpgp --export-options export-minimal "
	<< ((private_key) ? "--export-secret-keys " : "--export ")
	<< id;

	io_service io;
	FILE* const pipe = ::popen(command.str().data(), "r");
	posix::stream_descriptor sd(io, ::fileno(pipe));

	boost::system::error_code ec;
	vector<uint8_t> buffer(102400);
	size_t const bytes_read = read(sd, boost::asio::buffer(buffer), ec);
	if (ec)
		; // TODO ec always bad??
		else
			;
		buffer.resize(bytes_read);
	::pclose(pipe);

	return buffer;
}

} // namespace: gnupg
