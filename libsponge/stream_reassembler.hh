#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <map>
#include <string>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    ByteStream _output;                    //!< The reassembled in-order byte stream
    size_t _capacity;                      //!< The maximum number of bytes
    std::map<size_t, std::string> _map{};  //!< A map to store byte data with their respective indices
    size_t _un_reassembled_byte = 0;       //!< Number of bytes that are yet to be reassembled
    size_t _write_abs_index = 0;           //!< The absolute index for writing data
    bool _eof = false;                     //!< Flag indicating whether the end of the data has been reached

    void _push_map(const std::string &data, const size_t index, const bool eof);
    void _merge_map();
    size_t _un_reassembled_capacity() const { return _capacity - _output.buffer_size(); }
    size_t _max(const size_t num1, const size_t num2) const { return num1 > num2 ? num1 : num2; }
    size_t _min(const size_t num1, const size_t num2) const { return num1 > num2 ? num2 : num1; }

  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receive a substring and write any newly contiguous bytes into the stream.
    //!
    //! The StreamReassembler will stay within the memory limits of the `capacity`.
    //! Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte in `data`
    //! \param eof the last byte of `data` will be the last byte in the entire stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
