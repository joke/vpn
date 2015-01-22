#ifndef netdevice_h
#define netdevice_h

#include <string>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/asio/ip/icmp.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/strand.hpp>

//! netdevice handles the tun/tap device
template <typename Server>
class netdevice {
public:
// 	typedef typename Server::gateway_type& gateway_type;

	//! create netdevice
	template <typename Callback>
	explicit netdevice(boost::asio::io_service&, std::string name, boost::asio::ip::address_v6 const&, Callback c);
	//! just destruction
	~netdevice() noexcept;

	//! startup
	void startup();
	//! check if the device is up
	bool is_up() const;
	//! send data to device
	void send(boost::asio::const_buffers_1);
	//! get name of device
	std::string const& name() noexcept;

protected:
	//! get device index
	int ifindex() const;
	//! accept now connection
	void accept();
	//! read data and forward it
	void read(std::shared_ptr<std::vector<std::uint8_t>> buffer, boost::system::error_code const& error, std::size_t bytes_transferred);
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
	//! create unreachable response
	void unreachable(std::shared_ptr<std::vector<std::uint8_t>>);

	boost::asio::io_service& io_; //!< proactor
	boost::asio::strand strand_; //!< strand for synchronization
	std::string name_; //!< string holding device name
	boost::asio::posix::stream_descriptor stream_descriptor_; //!< stream descriptor of tuntap
	boost::asio::ip::icmp::socket icmp_;
	std::function<bool(std::uint64_t const, std::shared_ptr<std::vector<std::uint8_t>> const)> to_gateway_;

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


};

#ifndef IN_IDE_PARSERER
#include "netdevice.t++"
#endif

#endif
