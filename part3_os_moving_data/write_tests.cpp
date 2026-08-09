/* ========================================================================

   (C) Copyright 2023 by Molly Rocket, Inc., All Rights Reserved.
   
   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.
   
   Please see https://computerenhance.com for more information
   
   ======================================================================== */

/* ========================================================================
   LISTING 112
   ======================================================================== */

/* NOTE(casey): _CRT_SECURE_NO_WARNINGS is here because otherwise we cannot
   call fopen(). If we replace fopen() with fopen_s() to avoid the warning,
   then the code doesn't compile on Linux anymore, since fopen_s() does not
   exist there.
   
   What exactly the CRT maintainers were thinking when they made this choice,
   I have no idea. */
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <sys/mman.h>

#include "linux_metrics.cpp"

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

// #define ArrayCount(Array) (sizeof(Array)/sizeof((Array)[0]))


int main(int ArgCount, char **Args)
{
    if(ArgCount == 2)
    {
        u64 PageSize = 4096; // NOTE(casey): This may not be the OS page size! It is merely our testing page size.
        u64 PageCount = atol(Args[1]);
        u64 TotalSize = PageSize*PageCount;
        
        printf("Page Count, Touch Count, Fault Count, Extra Faults\n");
        
        for(u64 TouchCount = 0; TouchCount <= PageCount; ++TouchCount)
        {
            u64 TouchSize = PageSize*TouchCount;
	    u8 *Data = (u8 *) mmap(
		    NULL,
		    TotalSize,
		    PROT_READ | PROT_WRITE,
		    MAP_PRIVATE | MAP_ANONYMOUS,
		    -1,
		    0
	    );
            if(Data)
            {
		u64 Faults;
		assert(ReadPageFaults(&Faults) == 0);
                u64 StartFaultCount = Faults;
                for(u64 Index = 0; Index < TouchSize; ++Index)
                {
                    Data[Index] = (u8)Index;
                }
		assert(ReadPageFaults(&Faults) == 0);
                u64 EndFaultCount = Faults;
                
                u64 FaultCount = EndFaultCount - StartFaultCount;
                
                printf("%llu, %llu, %llu, %lld\n", PageCount, TouchCount, FaultCount, (FaultCount - TouchCount));
                
		munmap(Data, TotalSize);
            }
            else
            {
                fprintf(stderr, "ERROR: Unable to allocate memory\n");
            }
        }
    }
    else
    {
        fprintf(stderr, "Usage: %s [# of 4k pages to allocate]\n", Args[0]);
    }
		
    return 0;
}
