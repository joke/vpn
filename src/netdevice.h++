#ifndef netdevice_h
#define netdevice_h

#include <string>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <mutex>


//! netdevice handles the tun/tap device
template <typename SendHandler, typename ReceiveHandler>
class netdevice {
public:
	//using gateway_type = typename Server::gateway_type&;

	//! create netdevice
	explicit netdevice(boost::asio::io_service&, SendHandler, ReceiveHandler);
	//! just destruction
	~netdevice() noexcept;

	//! check if the device is up
	bool is_up() const;
	//! query the mtu
	unsigned int mtu() const;
	//! set up
	void up();
	//! set mtu
	void mtu(unsigned int);
	//! set address
	void address(boost::asio::ip::address_v6 const&);
	//! set routing prefix
	void route(boost::asio::ip::address_v6 const&, std::uint8_t const prefix);
	//! send data to device
	void write(boost::asio::const_buffers_1);
	//! get name of device
	std::string const& name() noexcept;

protected:
	//! get device index
	int ifindex() const;
	//! read data and forward it
	void read(boost::system::error_code const&, std::size_t const bytes_transferred, std::shared_ptr<std::vector<std::uint8_t>>);

	boost::asio::io_service& io_; //!< proactor
	//gateway_type gateway_; //!< the gateway. forward data there
	std::string name_; //!< string holding device name
	boost::asio::posix::stream_descriptor stream_descriptor_; //!< stream descriptor of tuntap
	std::mutex stream_descriptor_mutex_; //!< stream_descriptor should be mutexed

	//
	class ioctl {
	public:
		ioctl(int const name, void* data);
		int name() const;
		void* data();
	private:
		int name_;
		void* data_;
	};

private:
	explicit netdevice() = delete;
	explicit netdevice(netdevice const&) = delete;
	explicit netdevice(netdevice&&) = delete;
	netdevice& operator=(netdevice const&) = delete;
	netdevice& operator=(netdevice&&) = delete;

	SendHandler send_;
	ReceiveHandler receive_;
};


#ifndef IN_IDE_PARSERER
#include "netdevice.t++"
#endif

#endif
