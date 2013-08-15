#ifndef signal_h
#define signal_h

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>

namespace {

//! handle all kind of signals in the client might encounter
class signal {
public:
	//! create the signal handler
	explicit signal(boost::asio::io_service&);

private:
	//! handle to respond the asynchronous event
	void handler(boost::system::error_code const&, int const signal_number);

	boost::asio::io_service& io_; //!< proactor
	boost::asio::signal_set set_; //!< our signal set
};

} // namespace: <>

#ifndef IN_IDE_PARSERER
#include "signal.t++"
#endif

#endif
