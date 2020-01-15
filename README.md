# UDP Echo and TCP Calculator
Basic UDP and TCP network socket programming applications utilizing various socket programming function calls in C (clients and servers).

## Getting Started and Executing the Program
To compile this program, you will need to use the standard GCC compiler that is already installed on the AU tux machines. To compile this program into an executable, please use the following command:
```
gcc -o [File To Build] filename.c
```

To run the simulator, please use the following command: 
```
./[File to Execute]
```

## Protocol Specifications

* Client and Server 11
> Total Message Length (bytes): 2 bytes

> Sequence Number: 4 bytes

> Timestamp (ms): 8 bytes (long)

> String: Variable (Up To 1024 Bytes)

* Integers will use network byte order
* The sequence number will start at 1 and will wrap around at 2^32-1
* The timestamp is the time in milliseconds since epoch represented as a long (the difference, measured in milliseconds, between the current time and midnight, January 1, 1970 UTC)
* The string will be represented as UTF-8 characters. It can be any length up to 1024 bytes.
* The digits in part c will be sent as a string and will not include any punctuation.

## Authors

* **Christopher Tillery** - *Python Programming and Software Process Expert*
