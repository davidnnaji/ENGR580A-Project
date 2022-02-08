# ENGR580A-Project
 An application of cryptography to SAE J1708 and J1587 based network communication.

# Limitations
The J1708.h library used in my examples has been tested on a real ECU. However, I have found that the program drops frames when the bus is connected to more than two ECUs and a VDA. It has also returned errors during contention, and struggles to re-sync with the bus after an error event has occured. My plan is to sunset this project as v1.0 and revamp the software in a new repo to improve usability, reliability, and concurrency. 

TL;DR **Be warned** - the perfomance of this library is not to be relied on for puposes outside of simple logging of a single controller!
