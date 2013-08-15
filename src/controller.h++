#ifndef controller_h
#define controller_h

#include <memory>
#include <vector>
#include <cstdint>
#include <boost/asio/io_service.hpp>
#include <boost/system/error_code.hpp>

#include "gateway.h++"
#include "controller_protocol.h++"

namespace {

//! the controller is the connection to the command line interface within the server
template <typename Protocol, typename Gateway>
class controller {
public:
	//! construct controller
	explicit controller(boost::asio::io_service&, Gateway&);
	//! just destruct it
	~controller();

	//! run the controller and activate controlling socket
	void run();
	//! create buffer and more
	void listen();
	//! accept incoming connection
	void accept(boost::system::error_code const&, std::shared_ptr<typename Protocol::socket>);
	//! read header. header is a hex encoded size of the body
	void read_header(boost::system::error_code const&, std::size_t const bytes_transferred, std::shared_ptr<typename Protocol::socket>, std::shared_ptr<std::vector<char>>);
	//! read until body is complete
	void read_body(boost::system::error_code const&, std::size_t const bytes_transferred, std::shared_ptr<typename Protocol::socket>, std::shared_ptr<std::vector<char>>);
	//! evaluate query
	void evaluate(query&);

protected:
	boost::asio::io_service& io_service_; //!< proactor
	typename Protocol::acceptor acceptor_; //!< accepting socket
	Gateway& gateway_; //!< gateway we want to control

private:
	constexpr static std::size_t header_size = sizeof(std::size_t) * 2; //!< size of the header

	explicit controller(controller const&) = delete;
	explicit controller(controller&&) = delete;
	controller operator=(controller const&) = delete;
	controller operator=(controller&&) = delete;
};

} // namespace: <>

#ifndef IN_IDE_PARSERER
#include "controller.t++"
#endif

#endif
