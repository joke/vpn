#include <iostream>
#include <boost/program_options.hpp>


// #include <vector>
// #include <linux/if_tun.h>
// #include "dccp.h++"
// #include <boost/asio.hpp>
// #include "netdevice.h++"
//
// class ioctl {
// public:
// 	ioctl(int const name, void *data) : name_(name), data_(data) {
//
// 	}
// 	int name() const { return name_; }
// 	void* data() { return data_; }
// private:
// 	int name_;
// 	void* data_;
// };

int main() {
// 	set_debug_traps();
// 	breakpoint();

// 	using namespace std;
// 	boost::asio::io_service io;
// 	using namespace boost::asio;
// 	using namespace ip;
//
//
// // 	netdevice<bool> n(io, "vpn");
// // 	n.startup();
//
// 	auto fd(::open("/dev/net/tun", O_RDWR));
// 	if (fd < 0)
// 		throw system_error(errno, system_category(), "Could not open tun/tap control device");
// 	boost::asio::posix::stream_descriptor sd(io);
// 	sd.assign(fd);
//
// 	string name_("vpn");
//
// 	// do some io control
// 	ifreq req({});
// 	req.ifr_ifru.ifru_flags = IFF_TUN | IFF_NO_PI;
// 	name_.copy(req.ifr_name, IFNAMSIZ);
// 	class ioctl set(TUNSETIFF, &req);
// 	sd.io_control(set);
// 	cerr << "_______________NAME: " << req.ifr_name << std::endl;
// 	system(string("ifconfig " + name_ + " up add fc45:7f3:398a:6e1f:8e7::4/8").data());
//
//
// 	std::array<char, 4096> buf;
// 	cerr << "size: " << sd.read_some(buffer(buf)) << endl;
//
//
// 	io.run();


}
