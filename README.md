# ENGR580A-Project
 An application of cryptography to SAE J1708 and J1587 based network communication.

# Limitations
The J1708.h library used in my examples have been tested on a real system. However, I have found that the program drops frames when the bus is connected to more than two ECUs. It has also returned errors during contention. My plan is to sunset this project as v0.1 and revamp the software to improve concurrency. Be warned - the perfomance of this library is not to be relied on for puposes outside of simple logging of a single controller!
