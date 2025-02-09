#ifndef _MEMMAP_H_
#define _MEMMAP_H_

void * memmap(unsigned long phy_addr, unsigned long size);

int memunmap(void * addr_mapped);

#endif
