Assignment 1 Writeup
=============

My name: 유재원

My POVIS ID: jaewonyu

My student ID (numeric): 20230784

This assignment took me about 12 hours to do (including the time on studying, designing, and writing the code).

Program Structure and Design of the StreamReassembler:
I implemented the `push_substring` function in the following steps:
First, the given input is pushed into `_map`, an internally managed `std::map` class.
At this stage, the input is adjusted to fit within the capacity, and exceptions for the same index are handled.
Next, the elements in `_map` are merged, removing any overlapping substrings.
Finally, the assembled substring is written to the `ByteStream`.
Through this process, the program can assemble out-of-order input into the correct order.

Implementation Challenges:
In fact, when I started the assignment, I expected the merging part to be more challenging. However, the real difficulty came with implementing the `_push_map` function. Handling the given input to fit within the capacity involved more cases than I initially anticipated. The best approach was to add handling for each issue as it occurred during testing.

Remaining Bugs:
In my opinion, the current implementation is solid and free of major issues. It passes all the test cases, performs efficiently, and appears to be implemented with minimal memory usage.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
