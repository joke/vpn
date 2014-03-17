#ifndef advertiser_h
#define advertiser_h

#include <cstdint>
#include <array>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/icmp.hpp>
#include <boost/asio/high_resolution_timer.hpp>


//! the advertiser sends router advertise packages SLAAC
class advertiser {
public:
	//! create advertiser and
	explicit advertiser(boost::asio::io_service&, std::uint64_t const prefix);
	//! just a destructor
	~advertiser();
	//! send data to local network and set asynchronous timer
	void send(boost::system::error_code const& = boost::system::error_code());
	//! initialize advertiser
	void run();

protected:
	boost::asio::io_service& io_; //!< proactor
	boost::asio::high_resolution_timer timer_; //!< timer used to send router advertises
	boost::asio::ip::icmp::socket socket_; //!< socket
	std::vector<boost::asio::ip::icmp::endpoint> endpoints_; //!< multicast icmp endpoint
	std::array<std::uint8_t, 80> buf_; //!< buffer large enough to hold packet
	std::uint64_t prefix_; //!< vpn prefix to advertise

private:
	explicit advertiser() = delete;
	explicit advertiser(advertiser const&) = delete;
	explicit advertiser(advertiser&&) = delete;
	advertiser& operator=(advertiser const&) = delete;
	advertiser& operator=(advertiser&&) = delete;
};


#ifndef IN_IDE_PARSERER
#include "advertiser.t++"
#endif

#endif
