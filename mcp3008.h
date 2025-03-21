#ifndef MCP3008_H
#define MCP3008_H
/*
* CS 107e 
* Authors: Mario Portillo and Sahan Samarakoon
* Date: 13th March 2025
* This file is very standard and has very little difference from a standard
* mcp3008 driver. 
*/

void mcp3008_init();
unsigned int mcp3008_read_channel(int channel);

#endif
