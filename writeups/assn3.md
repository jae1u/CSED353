Assignment 3 Writeup
=============

My name: 유재원

My POVIS ID: jaewonyu

My student ID (numeric): 20230784

This assignment took me about 8 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): None

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the TCPSender:
First, I created a helper class called `timer`. The `timer` is initialized with `retx_timeout` and consists of several functions:  

- `start`: Starts the timer.  
- `stop`: Stops the timer.  
- `handle_retransmission`: Doubles the RTO (Retransmission Timeout) and increments the retransmission count.  
- `is_expired`: Checks whether the timer has expired.  
- `tock`: Increases the timer tick.  

When the `tick` function is called, it invokes the `tock` function to update the time. If the timer has expired, the oldest segment is retransmitted, and depending on the window size, the `handle_retransmission` function is called, ensuring compliance with TCP standards.  

When the `ack_received` function is called, it first verifies whether the acknowledgment (ACK) is valid. Then, it updates the window size and `ackno`. The updated `ackno` is reflected in `_segments_outstand`, and the timer is used in accordance with the TCP standards outlined in the documentation.  

The actual transmission of TCP segments is handled by the `send_segment` function. This function receives a segment, sets its `seqno`, and pushes it to both `_segments_out` and `_segments_outstand`. (At this point, it is assumed that the `send_segment` function is always called with a valid segment whose length in the sequence space is greater than 0.) If the timer is in a stopped state, it is started.  

For the `fill_window` function, I actively utilized the FSM (Finite State Machine) described in the documentation. The function differentiates operations based on each state, with most cases falling under the `SYN_ACKED` state. In this state, it creates a segment and calls the `send_segment` function.

Implementation Challenges:
A significant portion of the code was modified to handle test cases where the window is sufficiently large, requiring multiple segment transmissions in a single `fill_window` function call. I carefully considered how to maintain the original design, which operates based on predefined states. To resolve this, I replaced the existing state logic with a `while` loop and determined appropriate break conditions.

Remaining Bugs:
There are no bugs expected in the current implementation.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
