Assignment 6 Writeup
=============

My name: 유재원

My POVIS ID: jaewonyu

My student ID (numeric): 20230784

This assignment took me about 2 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): None

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the Router:
The routing information was stored in a separate structure and saved in a std::vector. During the routing process, the vector was traversed to find the route with the longest prefix, and the datagram was forwarded along that route.

Implementation Challenges:
During the process of bit masking, there were some undefined behaviors that needed to be handled carefully. Additionally, I recently came across some principles related to code writing from Google, and I made an effort to write code that is as readable as possible for others.

Remaining Bugs:
There are no bugs expected in the current implementation.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
