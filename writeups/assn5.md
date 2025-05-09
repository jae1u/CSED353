Assignment 5 Writeup
=============

My name: 유재원

My POVIS ID: jaewonyu

My student ID (numeric): 20230784

This assignment took me about 4.5 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): None

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Program Structure and Design of the NetworkInterface:
- **ARP-based Address Resolution**:
  The `NetworkInterface` resolves Ethernet addresses for IP datagrams using ARP. It maintains a mapping table (`_mapping_table`) to cache resolved addresses and sends ARP requests when an address is missing or expired.

- **Frame Transmission**:
  Outbound frames are queued in `_frames_out`. If the Ethernet address of the next hop is known, frames are directly sent. Otherwise, they are temporarily stored in `_frames_queue` and sent once the ARP reply is received.

- **Frame Reception**:
  Incoming Ethernet frames are processed based on their type (IPv4 or ARP). IPv4 frames are parsed and passed up the stack, while ARP frames are used to update the address mapping table or respond to ARP requests.

- **Time Management**:
  The `tick` method handles time-based events, such as expiring outdated ARP mappings and ensuring timely ARP requests.

Implementation Challenges:
- **Efficient ARP Handling**:
  Managing the ARP mapping table (`_mapping_table`) and ensuring timely updates to avoid stale entries required careful design. This was addressed by introducing a time-based validity check (`is_valid`) and periodic updates (`is_need_update`).

- **Handling Frame Queues**:
  Managing `_frames_queue` for pending frames while waiting for ARP replies introduced complexity. Ensuring frames are sent in order and clearing queues after address resolution required precise logic.

Remaining Bugs:
There are no bugs expected in the current implementation.

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
