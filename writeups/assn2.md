Assignment 2 Writeup
=============

My name: 유재원

My POVIS ID: jaewonyu

My student ID (numeric): 20230784

This assignment took me about 6 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): None

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
The wrap function essentially returns (n + isn) & 0xffffffff.
The unwrap function is a bit more complex. It returns (n - isn) & 0xffffffff + k * (1 << 32) for an appropriate value of k.

The value of k is determined by calculating the two possible results and selecting the one that minimizes the distance from the checkpoint.
The segment_received function in TCPReceiver sets _isn when it receives a SYN signal. If _isn is set, it pushes the payload to the reassembler.
The ackno function returns the next expected seqno if _isn is set; otherwise, it returns nullopt.
The window_size function returns the remaining capacity of the ByteStream.

Implementation Challenges:
The process of finding an appropriate value for k in the implementation of the unwrap function was challenging to figure out.
I determined the number of blocks between (n - isn) & 0xffffffff and block_offset, setting the lower bound as the value increased by that number of blocks and the upper bound as the value increased by one more block. I compared these two values to decide the result. To work this out, I studied various cases by sketching them on A4 paper.

Additionally, I had to handle an edge case where the checkpoint was smaller than (n - isn) & 0xffffffff.

Unrelated to the assignment itself, I encountered std::optional for the first time. It took some time to get used to it, but once I became familiar with it, I was able to write much cleaner code.

Remaining Bugs:
There are no bugs expected in the current implementation.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
