# UDP Echo and TCP Calculator
Basic UDP and TCP network socket programming applications utilizing various socket programming function calls in C (clients and servers).

## Getting Started and Executing the Program
To compile this program, you will need to use the standard GCC compiler that is already installed on the AU tux machines. To compile this program into an executable, please use the following command:
```
gcc -o [File to Build] [File to Build].c
```

To run the simulator, please use the following command: 
```
./[File to Execute]
```

## Protocol Specifications

### UDP Client and Server
> Total Message Length (bytes): 2 bytes

> Sequence Number: 4 bytes

> Timestamp (ms): 8 bytes (long)

> String: Variable (Up To 1024 Bytes)

* Integers will use network byte order
* The sequence number will start at 1 and will wrap around at 2^32-1
* The timestamp is the time in milliseconds since epoch represented as a long (the difference, measured in milliseconds, between the current time and midnight, January 1, 1970 UTC)
* The string will be represented as UTF-8 characters. It can be any length up to 1024 bytes.
* The digits in part c will be sent as a string and will not include any punctuation.

### TCP Client and Server
#### Request Message
> Operation Code: 1 byte (‘+’ = 0x2b (43), ‘-‘ = 0x2d (45), ‘x’ = 0x78 (120), ‘/’ = 0x2f (47))

> Operand A: 4 bytes (unsigned integer)

> Operand B: 4 bytes (unsigned integer)

* The total request message length will always be nine bytes.
* Each operand is an unsigned 32-bit integer.
* The standard UTF-8 character values are used to convert the operation code into a hex value (decimal equivalent given in parenthesis).

#### Response Message
> Operation Code: 1 byte (‘+’ = 0x2b (43), ‘-‘ = 0x2d (45), ‘x’ = 0x78 (120), ‘/’ = 0x2f (47))

> Operand A: 4 bytes (unsigned integer)

> Operand B: 4 bytes (unsigned integer)

> Answer: 4 bytes (unsigned integer)

> Is Answer Valid: 1 byte (1 = Valid, 2 = Invalid or NaN result)

* The total response message length will always be fourteen bytes.
* The result is an unsigned 32-bit integer.
* Is Answer Valid will take care of NaN / divide by zero situations. (If the answer is invalid, the answer field should be set to 0).
* The answer is unsigned, because the operands were specified as unsigned in the specifications for the lab.

## Authors

* **Christopher Tillery** - *C Programming and Lead Project Engineer*
* **John David Watts** - *C Programming and Lead Technical Writer*
