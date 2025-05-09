#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();

    EthernetFrame frame;
    frame.header().src = _ethernet_address;
    frame.header().type = EthernetHeader::TYPE_IPv4;
    frame.payload() = dgram.serialize();

    const auto it = _mapping_table.find(next_hop_ip);
    if (it != _mapping_table.end() && it->second.is_valid(_time)) {
        const auto &ethernet = it->second.ethernet_address();
        frame.header().dst = ethernet;
        _frames_out.push(frame);

    } else {
        send_arp_request(next_hop_ip);
        _frames_queue[next_hop_ip].push(frame);
    }
}

//! \param[in] frame the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    const auto _header = frame.header();
    const auto _payload = frame.payload();
    if (_header.dst != _ethernet_address && _header.dst != ETHERNET_BROADCAST) {
        return nullopt;
    }

    if (_header.type == EthernetHeader::TYPE_IPv4) {
        InternetDatagram datagram;
        const auto result = datagram.parse(_payload);
        if (result != ParseResult::NoError) {
            return nullopt;
        }
        return datagram;

    } else if (_header.type == EthernetHeader::TYPE_ARP) {
        ARPMessage message;
        const auto result = message.parse(_payload);
        if (result != ParseResult::NoError) {
            return nullopt;
        }

        if (message.opcode == ARPMessage::OPCODE_REPLY) {
            _mapping_table[message.sender_ip_address].update(message.sender_ethernet_address, _time);
            auto it = _frames_queue.find(message.sender_ip_address);
            if (it != _frames_queue.end()) {
                while (!it->second.empty()) {
                    auto _frame = it->second.front();
                    _frame.header().dst = message.sender_ethernet_address;
                    _frames_out.push(_frame);
                    it->second.pop();
                }
                _frames_queue.erase(it);
            }
            return nullopt;

        } else if (message.opcode == ARPMessage::OPCODE_REQUEST) {
            _mapping_table[message.sender_ip_address].update(message.sender_ethernet_address, _time);
            if (message.target_ip_address == _ip_address.ipv4_numeric()) {
                send_arp_reply(message.sender_ip_address, message.sender_ethernet_address);
            }
            return nullopt;
        }
    }
    return nullopt;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) { _time += ms_since_last_tick; }

void NetworkInterface::send_arp_reply(const uint32_t ip, const EthernetAddress &ethernet) {
    ARPMessage message;
    message.opcode = ARPMessage::OPCODE_REPLY;
    message.sender_ethernet_address = _ethernet_address;
    message.sender_ip_address = _ip_address.ipv4_numeric();
    message.target_ethernet_address = ethernet;
    message.target_ip_address = ip;

    EthernetFrame frame;
    frame.header().dst = ethernet;
    frame.header().src = _ethernet_address;
    frame.header().type = EthernetHeader::TYPE_ARP;
    frame.payload() = message.serialize();

    _frames_out.push(frame);
}

void NetworkInterface::send_arp_request(const uint32_t next_hop_ip) {
    auto it = _mapping_table.find(next_hop_ip);
    if (it != _mapping_table.end() && !it->second.is_need_update(_time)) {
        return;
    }
    _mapping_table[next_hop_ip].update(nullopt, _time);

    ARPMessage message;
    message.opcode = ARPMessage::OPCODE_REQUEST;
    message.sender_ethernet_address = _ethernet_address;
    message.sender_ip_address = _ip_address.ipv4_numeric();
    message.target_ip_address = next_hop_ip;

    EthernetFrame frame;
    frame.header().dst = ETHERNET_BROADCAST;
    frame.header().src = _ethernet_address;
    frame.header().type = EthernetHeader::TYPE_ARP;
    frame.payload() = message.serialize();

    _frames_out.push(frame);
}
