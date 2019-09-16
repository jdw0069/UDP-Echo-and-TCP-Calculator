/*  
COMP6320 Lab1.1 - client11c.c
Authors: Christopher Tillery & John Watts
Date: September 26, 2019
Protocol Agreement: Lab11-RFC.pdf
Description:
	The first process runs a loop which sends continuously the 
	numbers from 1 to 10,000 as strings (so 10,000 strings in total).
	The second process receives the responses from the server and reports any missing echo.
	At the statistics summary line, report whether there are missing echoes,
	and the smallest, largest, and average round trip times. 
*/

