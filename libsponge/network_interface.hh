#ifndef SPONGE_LIBSPONGE_NETWORK_INTERFACE_HH
#define SPONGE_LIBSPONGE_NETWORK_INTERFACE_HH

#include "ethernet_frame.hh"
#include "tcp_over_ip.hh"
#include "tun.hh"

#include <map>
#include <optional>
#include <queue>

//! \brief A "network interface" that connects IP (the internet layer, or network layer)
//! with Ethernet (the network access layer, or link layer).

//! This module is the lowest layer of a TCP/IP stack
//! (connecting IP with the lower-layer network protocol,
//! e.g. Ethernet). But the same module is also used repeatedly
//! as part of a router: a router generally has many network
//! interfaces, and the router's job is to route Internet datagrams
//! between the different interfaces.

//! The network interface translates datagrams (coming from the
//! "customer," e.g. a TCP/IP stack or router) into Ethernet
//! frames. To fill in the Ethernet destination address, it looks up
//! the Ethernet address of the next IP hop of each datagram, making
//! requests with the [Address Resolution Protocol](\ref rfc::rfc826).
//! In the opposite direction, the network interface accepts Ethernet
//! frames, checks if they are intended for it, and if so, processes
//! the the payload depending on its type. If it's an IPv4 datagram,
//! the network interface passes it up the stack. If it's an ARP
//! request or reply, the network interface processes the frame
//! and learns or replies as necessary.

class Mapping {
  private:
    std::optional<EthernetAddress> _ethernet_address{};
    size_t _last_update_time{};
    const size_t _ttl{30000};

  public:
    Mapping() = default;
    Mapping(const std::optional<EthernetAddress> &ethernet_address, const size_t last_update_time)
        : _ethernet_address(ethernet_address), _last_update_time(last_update_time) {}
    const EthernetAddress &ethernet_address() const { return *_ethernet_address; }
    bool is_valid(const size_t current_time) const {
        const size_t age = current_time - _last_update_time;
        return _ethernet_address && (age < _ttl);
    }
    void update(const std::optional<EthernetAddress> &ethernet_address, const size_t current_time) {
        _ethernet_address = ethernet_address;
        _last_update_time = current_time;
    }
    bool is_need_update(const size_t current_time) const {
        const size_t age = current_time - _last_update_time;
        return (!_ethernet_address && (age >= 5000)) || (_ethernet_address && (age >= _ttl));
    }
};

class NetworkInterface {
  private:
    //! Ethernet (known as hardware, network-access-layer, or link-layer) address of the interface
    EthernetAddress _ethernet_address;

    //! IP (known as internet-layer or network-layer) address of the interface
    Address _ip_address;

    //! outbound queue of Ethernet frames that the NetworkInterface wants sent
    std::queue<EthernetFrame> _frames_out{};

    size_t _time{};
    std::map<uint32_t, Mapping> _mapping_table{};
    std::map<uint32_t, std::queue<EthernetFrame>> _frames_queue{};
    void cache_mapping(const uint32_t ip, const EthernetAddress &ethernet);
    void send_arp_reply(const uint32_t ip, const EthernetAddress &ethernet);
    void send_arp_request(const uint32_t next_hop_ip);

  public:
    //! \brief Construct a network interface with given Ethernet (network-access-layer) and IP (internet-layer) addresses
    NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address);

    //! \brief Access queue of Ethernet frames awaiting transmission
    std::queue<EthernetFrame> &frames_out() { return _frames_out; }

    //! \brief Sends an IPv4 datagram, encapsulated in an Ethernet frame (if it knows the Ethernet destination address).

    //! Will need to use [ARP](\ref rfc::rfc826) to look up the Ethernet destination address for the next hop
    //! ("Sending" is accomplished by pushing the frame onto the frames_out queue.)
    void send_datagram(const InternetDatagram &dgram, const Address &next_hop);

    //! \brief Receives an Ethernet frame and responds appropriately.

    //! If type is IPv4, returns the datagram.
    //! If type is ARP request, learn a mapping from the "sender" fields, and send an ARP reply.
    //! If type is ARP reply, learn a mapping from the "sender" fields.
    std::optional<InternetDatagram> recv_frame(const EthernetFrame &frame);

    //! \brief Called periodically when time elapses
    void tick(const size_t ms_since_last_tick);
};

#endif  // SPONGE_LIBSPONGE_NETWORK_INTERFACE_HH
