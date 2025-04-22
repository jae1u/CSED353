#include "tcp_connection.hh"

#include <iostream>

using namespace std;

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _tick; }

void TCPConnection::segment_received(const TCPSegment &seg) {
    _tick = 0;
    if (seg.header().rst) {
        _is_disconnected = true;
        _receiver.stream_out().set_error();
        _sender.stream_in().set_error();
        return;
    }

    _receiver.segment_received(seg);
    if (_receiver.stream_out().eof() && !_sender.stream_in().eof()) {
        _linger_after_streams_finish = false;
    }
    if (seg.header().ack) {
        _sender.ack_received(seg.header().ackno, seg.header().win);
    }

    if (_receiver.ackno()) {
        _sender.fill_window();
        if (seg.length_in_sequence_space() > 0 && _sender.segments_out().empty()) {
            _sender.send_empty_segment();
        }
    }

    if (_receiver.ackno().has_value() && (seg.length_in_sequence_space() == 0) &&
        seg.header().seqno == _receiver.ackno().value() - 1) {
        _sender.send_empty_segment();
    }

    send_segments();
}

bool TCPConnection::active() const {
    if (_is_disconnected)
        return false;
    if (!(_receiver.stream_out().eof() && _sender.stream_in().eof() && _sender.bytes_in_flight() == 0))
        return true;
    return _linger_after_streams_finish && _tick < _cfg.rt_timeout * 10;
}

size_t TCPConnection::write(const string &data) {
    const auto write_length = _sender.stream_in().write(data);
    _sender.fill_window();
    send_segments();
    return write_length;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    _tick += ms_since_last_tick;
    _sender.tick(ms_since_last_tick);
    const auto rst = _sender.consecutive_retransmissions() > TCPConfig::MAX_RETX_ATTEMPTS;
    send_segments(rst);
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input();
    _sender.fill_window();
    send_segments();
}

void TCPConnection::connect() {
    _sender.fill_window();
    send_segments();
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            cerr << "Warning: Unclean shutdown of TCPConnection\n";
            send_segments(true);
        }
    } catch (const exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}

void TCPConnection::send_segments(const bool rst) {
    if (rst && _sender.segments_out().empty())
        _sender.send_empty_segment();
    while (!_sender.segments_out().empty()) {
        auto segment = _sender.segments_out().front();
        _sender.segments_out().pop();

        if (_receiver.ackno()) {
            segment.header().ack = true;
            segment.header().ackno = *_receiver.ackno();
        }
        segment.header().rst = rst;
        segment.header().win = min(_receiver.window_size(), static_cast<size_t>(numeric_limits<uint16_t>::max()));

        _segments_out.push(segment);

        if (rst) {
            _is_disconnected = true;
            _receiver.stream_out().set_error();
            _sender.stream_in().set_error();
            return;
        }
    }
}
