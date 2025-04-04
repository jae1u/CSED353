#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <algorithm>
#include <random>

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity)
    , _timer(retx_timeout) {}

uint64_t TCPSender::bytes_in_flight() const { return _next_seqno - _ackno; }

void TCPSender::fill_window() {
    TCPSegment segment;
    // CLOSED
    if (next_seqno_absolute() == 0) {
        segment.header().syn = true;
        send_segment(segment);
        return;
    }
    // SYN_SENT
    if (next_seqno_absolute() > 0 && next_seqno_absolute() == bytes_in_flight()) {
        return;
    }
    // SYN_ACKED
    while (next_seqno_absolute() > bytes_in_flight() && !stream_in().eof()) {
        const uint16_t window_size = max(_window_size, static_cast<uint16_t>(1));
        const uint16_t available_window_size = window_size - bytes_in_flight();
        const uint16_t read_size = min(available_window_size, static_cast<uint16_t>(TCPConfig::MAX_PAYLOAD_SIZE));
        segment.payload() = stream_in().read(read_size);
        if (segment.length_in_sequence_space() == 0) {
            return;
        }
        if (stream_in().eof() && segment.length_in_sequence_space() < available_window_size) {
            segment.header().fin = true;
        }
        send_segment(segment);
    }
    // SYN_ACKED(also)
    if (stream_in().eof() && next_seqno_absolute() < stream_in().bytes_written() + 2) {
        const uint16_t window_size = max(_window_size, static_cast<uint16_t>(1));
        const uint16_t available_window_size = window_size - bytes_in_flight();
        if (available_window_size == 0) {
            return;
        }
        segment.header().fin = true;
        send_segment(segment);
        return;
    }
    // FIN_SENT
    if (stream_in().eof() && next_seqno_absolute() == stream_in().bytes_written() + 2 && bytes_in_flight() > 0) {
        return;
    }
    // FIN_ACKED
    if (stream_in().eof() && next_seqno_absolute() == stream_in().bytes_written() + 2 && bytes_in_flight() == 0) {
        return;
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    const uint64_t _ackno_acked = unwrap(ackno, _isn, _ackno);
    if (next_seqno_absolute() < _ackno_acked) {
        return;
    }
    if (_ackno == _ackno_acked) {
        _window_size = window_size;
    }
    if (_ackno < _ackno_acked) {
        _window_size = window_size;
        _ackno = _ackno_acked;
        while (!_segments_outstand.empty()) {
            const TCPSegment &segment = _segments_outstand.front();
            const uint64_t segment_abs_seqno = unwrap(segment.header().seqno, _isn, _next_seqno);
            if (segment_abs_seqno + segment.length_in_sequence_space() > _ackno) {
                break;
            }
            _segments_outstand.pop();
        }
        _timer.stop();
        if (!_segments_outstand.empty()) {
            _timer.start();
        }
    }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    if (_timer.is_started()) {
        _timer.tock(ms_since_last_tick);
        if (_timer.is_expired()) {
            _segments_out.push(_segments_outstand.front());
            if (_window_size != 0) {
                _timer.handle_retransmission();
            }
            _timer.start();
        }
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return _timer.consecutive_retransmissions(); }

void TCPSender::send_empty_segment() {
    TCPSegment segment;
    segment.header().seqno = next_seqno();
    _segments_out.push(segment);
}

void TCPSender::send_segment(TCPSegment &segment) {
    segment.header().seqno = next_seqno();
    _next_seqno += segment.length_in_sequence_space();
    _segments_out.push(segment);
    _segments_outstand.push(segment);
    if (!_timer.is_started()) {
        _timer.start();
    }
}
