/*  
COMP6320 Lab1.2 - server12.c
Authors: Christopher Tillery & John Watts
Date: September 26, 2019
Protocol Agreement: Lab12-RFC.pdf
Description:
	Receives message M from the client and extracts a (operand 1), b (operand 2), and c (opcode).
    Performs the operation requested and sends back the result with repeating the operands and operation.
	The size of the message M (at the application layer) should not exceed 16 bytes.
*/