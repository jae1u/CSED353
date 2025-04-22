Assignment 4 Writeup
=============

My name: 유재원

My POVIS ID: jaewonyu

My student ID (numeric): 20230784

This assignment took me about 12 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): None

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Your benchmark results (without reordering, with reordering): [2.08, 1.39]

Program Structure and Design of the TCPConnection:
- **State Management**:  
  The `_is_disconnected` flag tracks whether the connection is active. The `_linger_after_streams_finish` variable ensures proper handling of lingering after streams close.

- **Segment Handling**:  
  The `segment_received` method processes incoming segments, updating the receiver and sender states, handling acknowledgments, and sending necessary responses.

- **Timeout and Retransmission**:  
  The `tick` method manages time-based events, such as retransmissions and connection termination after exceeding the retransmission limit.

- **Graceful Shutdown**:  
  The destructor ensures clean shutdown by sending a reset segment if the connection is still active.

- **Encapsulation**:  
  The design delegates tasks like window management and segment assembly to the sender and receiver components.

Implementation Challenges:
- **Reset Handling**:  
  Managing `RST` segments was tricky, as it required ensuring both sender and receiver streams transitioned to an error state. This was resolved by carefully updating `_is_disconnected` and stream error flags.

- **Edge Cases in Acknowledgments**:  
  Handling empty segments and sequence number edge cases required additional logic to ensure compliance with TCP behavior.

Remaining Bugs:
There are no bugs expected in the current implementation.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
