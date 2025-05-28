Assignment 7 Writeup
=============

My name: 유재원

My POVIS ID: jaewonyu

My student ID (numeric): 20230784

My assignment partner's name: 이성재

My assignment partner's POVIS ID: dltjdwo

My assignment partner's ID (numeric): 20230345

This assignment took me about 1.5 hours to do (including the time on studying, designing, and writing the code).

If you used any part of best-submission codes, specify all the best-submission numbers that you used (e.g., 1, 2): None

- **Caution**: If you have no idea about above best-submission item, please refer the Assignment PDF for detailed description.

Solo portion:
✔ Did your implementation successfully start and end a conversation with another copy of itself?
Yes. I confirmed that my implementation was able to successfully communicate by connecting to the relay server using both the server and client ports.

✔ Did it successfully transfer a one-megabyte file, with contents identical upon receipt?
Yes. I successfully transferred a 1MB file and verified that the SHA256 hashes matched.

✔ Please describe what code changes, if any, were necessary to pass these steps.
No code modifications were required to pass these steps.

Group portion:
✔ What is your team’s name? Who is your partner (and what is their POVIS ID)?
Our team name is `Berners-Lee`. My partner’s name is `이성재`, and their POVIS ID is `dltjdwo`.

✔ Did your implementations successfully start and end a conversation with each other (with each implementation acting as “client” or as “server”)?
Yes. My teammate and I took turns acting as both client and server with our own implementations, and we could confirm successful communication.

✔ Did you successfully transfer a one-megabyte file between your two implementations, with contents identical upon receipt?
Yes. We were able to successfully send and receive a 1MB file between our implementations.

✔ Please describe what code changes, if any, were necessary to pass these steps, either by you or your partner.
No code modifications were required to pass these steps.

Creative portion (optional):
This update replaces `bidirectional_stream_copy` with a custom `chatting` function for one-to-one chat. Now, every outgoing message is prefixed with the sender's username (e.g., `@username: message`), so recipients can easily identify who sent each message.

Other remarks:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
