#include "router.hh"

#include <iostream>

using namespace std;

//! \param[in] route_prefix The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
//! \param[in] prefix_length For this route to be applicable, how many high-order (most-significant) bits of the route_prefix will need to match the corresponding bits of the datagram's destination address?
//! \param[in] next_hop The IP address of the next hop. Will be empty if the network is directly attached to the router (in which case, the next hop address should be the datagram's final destination).
//! \param[in] interface_num The index of the interface to send the datagram out on.
void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const optional<Address> next_hop,
                       const size_t interface_num) {
    cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/" << int(prefix_length)
         << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)") << " on interface " << interface_num << "\n";

    const auto _route = Route(route_prefix, prefix_length, next_hop, interface_num);
    _routes.push_back(_route);
}

//! \param[in] dgram The datagram to be routed
void Router::route_one_datagram(InternetDatagram &dgram) {
    const uint32_t &destination = dgram.header().dst;
    std::optional<Route> best_route;
    uint32_t mask;
    for (const auto &route : _routes) {
        if (route._prefix_length == 32) {
            mask = 0xffffffff;
        } else if (route._prefix_length == 0) {
            mask = 0;
        } else {
            mask = 0xFFFFFFFF << (32 - route._prefix_length);
        }

        if ((destination & mask) == (route._route_prefix & mask)) {
            if (!best_route.has_value() || (*best_route)._prefix_length < route._prefix_length) {
                best_route = route;
            }
        }
    }
    if (!best_route.has_value() || dgram.header().ttl == 0 || --dgram.header().ttl == 0) {
        return;
    }
    const auto next_hop =
        (*best_route)._next_hop.has_value() ? (*best_route)._next_hop.value() : Address::from_ipv4_numeric(destination);
    interface((*best_route)._interface_num).send_datagram(dgram, next_hop);
}

void Router::route() {
    // Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
    for (auto &interface : _interfaces) {
        auto &queue = interface.datagrams_out();
        while (not queue.empty()) {
            route_one_datagram(queue.front());
            queue.pop();
        }
    }
}
