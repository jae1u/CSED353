#include "wrapping_integers.hh"

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { return WrappingInt32(n) + isn.raw_value(); }

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    const uint64_t pow_2_32 = 1ul << 32;
    const uint64_t abs_seqno_mod_32 = (n - isn) & 0xffffffff;

    if (checkpoint < static_cast<uint64_t>(abs_seqno_mod_32))
        return abs_seqno_mod_32;

    const uint64_t block_offset = (checkpoint - abs_seqno_mod_32) / pow_2_32 * pow_2_32;
    const uint64_t lower = abs_seqno_mod_32 + block_offset;
    const uint64_t higher = abs_seqno_mod_32 + block_offset + pow_2_32;
    return (higher - checkpoint) <= (1ul << 31) ? higher : lower;
}
