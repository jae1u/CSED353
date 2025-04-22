#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    if (seg.header().syn) {
        _isn = seg.header().seqno;
    }

    if (_isn) {
        const auto absolute_sequence_number = unwrap(seg.header().seqno, *_isn, _checkpoint);
        if (absolute_sequence_number == 0 && !seg.header().syn) {
            return;
        }
        const auto push_index = absolute_sequence_number - !seg.header().syn;
        _reassembler.push_substring(move(seg.payload().copy()), push_index, seg.header().fin);
        _checkpoint = absolute_sequence_number;
    }
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_isn) {
        return wrap(1 + stream_out().bytes_written() + static_cast<size_t>(stream_out().input_ended()), *_isn);
    }
    return nullopt;
}

size_t TCPReceiver::window_size() const { return _reassembler.stream_out().remaining_capacity(); }
