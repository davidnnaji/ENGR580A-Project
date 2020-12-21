Function Descriptions for J1708.h
by David Nnaji

J1708Rx()
	Description: Checks serial bus for bytes and saves it to a buffer. Frames these messages based on 
	elapsed time. This timing has been found to vary based on the micro-controller. This function will
	check if the checksum is correct and if the buffer is full.
	INPUT: Buffer reference
	OUTPUT: 0 or Length of J1708 frame

RxEnable()
	Description: Configures Teensy pins so that it can receive messages.
	INPUT: -
	OUTPUT: -

TxEnable()
	Description: Configures Teensy pins so that it can transmit messages.
	INPUT: -
	OUTPUT: -

RTS_Handler()
	Description: Parses a J1587 RTS message, builds, and queues a corresponding CTS message. It also 
	initializes Rxtransport variables and buffers.
	INPUT: Buffer containing the RTS message bytes with or without it's checksum.
	OUTPUT: True (Finished and responded), False (Error)

CTS_Handler()
	Description: Parses a J1587 CTS message, builds, and queues the corresponding CDP message(s).
	INPUT: A buffer containing the CTS message bytes with or without it's checksum.
	OUTPUT: True (Finished and responded), False (Error)

CDP_Handler()
	Description: Parses a J1587 CDP message and appends its data to a CDP buffer. Once complete, it
	builds and queues the corresponding EOM message(s). Resets all variables associated with Rxtranport
	protocol functions.
	INPUT: A buffer containing the CDP message bytes with or without it's checksum.
	OUTPUT: True (Finished and responded), False (Error)

EOM_Handler()
	Description: Parses a J1587 EOM message and resets all Txtransport variables associated with transport.
	protocol functions.
	INPUT: A buffer containing the EOM message bytes with or without it's checksum.
	OUTPUT: True (Finished and responded), False (Error)

Abort_Handler()
	Description: Parses a J1587 Abort message and resets all variables associated with transport
	protocol functions.
	INPUT: A buffer containing the any J1708 message bytes with or without it's checksum. (Not used)
	OUTPUT: -

J1708CheckChecksum()
	Description: Checks if message matches checksum.
	INPUT: A J1708 message buffer and its frame length.
	OUTPUT: T/F

parseJ1708()
	Description: Saves the current message in the global Rxbuffer to a more stable 32 byte location for later use. This new buffer will not be overwritten until it has been parsed. This frees up the Rxbuffer for to be used by the main loop() after it has been called. This function also parses the MID and PID of the message and passes it to a message handler if the PID is relevant. It will return the handler code or 0 to denote that the message isn't important.
	INPUT: -
	OUTPUT: -

J1708Listen()
	Description: This function serves as the main runtime function during loop(). It checks if a 
	message has been received, prints it to the serial monitor, transmits messages in the Tx queue, and queues received messages for parsing.
	INPUT: -
	OUTPUT: -

J1708TransportTx()
	Description: Takes a data buffer and queues the corresponding RTS message to be sent to a specific
	address. It also initializes Txtransport variables and buffers.
	INPUT: A buffer containing the data to be sent, the number of bytes, and the destination MID.
	OUTPUT: True (Finished and responded), False (Error)