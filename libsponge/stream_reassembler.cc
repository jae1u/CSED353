#include "stream_reassembler.hh"

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    _push_map(data, index, eof);
    _merge_map();
    auto iter = _map.find(_write_abs_index);
    if (iter != _map.end()) {
        _output.write(iter->second);
        _un_reassembled_byte -= iter->second.length();
        _write_abs_index += iter->second.length();
        _map.erase(iter);
    }
    if (empty() && _eof) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _un_reassembled_byte; }

bool StreamReassembler::empty() const { return _un_reassembled_byte == 0; }

void StreamReassembler::_push_map(const std::string &data, const size_t index, const bool eof) {
    auto _min_abs_index = _max(index, _write_abs_index);
    auto _min_rel_index = _min_abs_index - index;

    ssize_t _length;
    ssize_t diff = (index + data.length()) - (_write_abs_index + _un_reassembled_capacity());
    if (diff > 0) {
        _eof = false;
        _length = data.length() - _min_rel_index - diff;
    } else {
        _eof = _eof || eof;
        _length = data.length() - _min_rel_index;
    }
    if (_length <= 0) {
        return;
    }
    const auto _data = data.substr(_min_rel_index, _length);
    auto iter = _map.find(_min_abs_index);
    if (iter == _map.end()) {
        _un_reassembled_byte += _data.length();
        _map[_min_abs_index] = _data;
    } else if (_data.length() > iter->second.length()) {
        _un_reassembled_byte += _data.length() - iter->second.length();
        iter->second = _data;
    }
}

void StreamReassembler::_merge_map() {
    auto iter = _map.begin();
    while (iter != _map.end() && next(iter) != _map.end()) {
        auto n_iter = next(iter);
        auto iter_end_index = iter->first + iter->second.length();
        auto n_iter_end_index = n_iter->first + n_iter->second.length();
        if (n_iter != _map.end() && iter_end_index >= n_iter->first) {
            if (iter_end_index >= n_iter_end_index) {
                _un_reassembled_byte -= n_iter->second.length();
                _map.erase(n_iter);
                iter = _map.begin();
            } else {
                size_t overlap = iter_end_index - n_iter->first;
                iter->second += n_iter->second.substr(overlap);
                _un_reassembled_byte -= overlap;
                _map.erase(n_iter);
                iter = _map.begin();
            }
        } else {
            iter++;
        }
    }
}
