#ifndef signal_h
#define signal_h

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/config/no_tr1/complex.hpp>

namespace {

//! handle all kind of signals in the client might encounter and act on accordingly
class signal {
public:
	//! create the signal handler
	explicit signal(boost::asio::io_service&);

protected:
	//! handle shutdown and shutdown the io_service
	void shutdown(boost::system::error_code const&, int const signal_number);

	boost::asio::io_service& io_; //!< proactor
	boost::asio::signal_set set_; //!< our signal set

private:
	explicit signal() = delete;
	explicit signal(signal const&) = delete;
	explicit signal(signal&&) = delete;
	signal const operator=(signal const&) = delete;
};

} // namespace: <>

#ifndef IN_IDE_PARSERER
#include "signal.t++"
#endif

#endif
