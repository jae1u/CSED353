#include "byte_stream.hh"
using namespace std;

ByteStream::ByteStream(const size_t capacity) : _capacity{capacity} {}

size_t ByteStream::write(const string &data) {
    const size_t write_length = _min(data.length(), remaining_capacity());
    _stream += data.substr(0, write_length);
    _written += write_length;
    return write_length;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    const size_t peek_length = _min(len, buffer_size());
    return _stream.substr(0, peek_length);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    const size_t pop_length = _min(len, buffer_size());
    _popped += pop_length;
    _stream = _stream.substr(pop_length, _stream.length());
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    const string result = peek_output(len);
    pop_output(len);
    return result;
}

void ByteStream::end_input() { _eof = true; }

bool ByteStream::input_ended() const { return _eof; }

size_t ByteStream::buffer_size() const { return _stream.length(); }

bool ByteStream::buffer_empty() const { return _stream.empty(); }

bool ByteStream::eof() const { return buffer_empty() && input_ended(); }

size_t ByteStream::bytes_written() const { return _written; }

size_t ByteStream::bytes_read() const { return _popped; }

size_t ByteStream::remaining_capacity() const { return _capacity - _stream.length(); }

size_t ByteStream::_min(const size_t num1, const size_t num2) const { return num1 > num2 ? num2 : num1; }