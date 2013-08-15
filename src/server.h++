#ifndef server_h
#define server_h

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

#include <functional>
#include <map>
#include <cstdint>
#include <forward_list>
#include <memory>
#include <thread>

#include "signal.h++"
#include "gateway.h++"
#include "controller.h++"
#include "netdevice.h++"
#include "advertiser.h++"
#include "encryption.h++"

namespace {

//! the server is the main entity binding all the other classes together
template <typename GatewayProtocol, typename ControllerProtocol>
class server {
public:
	typedef gateway<server> gateway_type;
	typedef GatewayProtocol gateway_protocol_type;
	typedef ControllerProtocol controller_protocol_type;
	typedef controller<controller_protocol_type, gateway_type> controller_type;
	typedef netdevice<server> netdevice_type;
	typedef std::uint64_t prefix_type;

	//! create a server instance
	explicit server(boost::asio::io_service&);

protected:
	//! start the server. run() other components and start thread pool
	void run();

	boost::asio::io_service& io_; //!< io server proactor
	signal signal_; //!< signal handler

	gateway_type gateway_; //!< gateway used to communicate with the internet
	controller_type controller_; //!< controller to respond to command line
	netdevice_type netdevice_; //!< netdevice handles tun/tap
	advertiser advertiser_; //!< advertize prefix to lan
	std::forward_list<std::thread> thread_pool_; //!< the thread pool

private:
	explicit server(server const&) = delete;
	explicit server(server&&) = delete;
	server& operator=(server const&) = delete;
	server& operator=(server&&) = delete;
};

} // namespace: <>

#ifndef IN_IDE_PARSERER
#include "server.t++"
#endif

#endif
