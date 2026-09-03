#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/resource.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t b32;

typedef float f32;
typedef double f64;

#define ArrayCount(Array) (sizeof(Array)/sizeof((Array)[0]))

#include "../listing_0068_buffer.cpp"
#include "../listing_0108_platform_metrics.cpp"
#include "../listing_0109_pagefault_repetition_tester.cpp"

// NOTE: listing_0106_mallocread_overhead_test.cpp is the file that normally
// defines these, but it unconditionally includes <windows.h> (it exercises
// ReadFile alongside fread/_read), so it cannot build on Linux at all. This
// is a minimal stand-in for just the read_parameters/allocation_type plumbing
// that listing_0110's WriteToAllBytes actually needs.
enum allocation_type
{
    AllocType_none,
    AllocType_malloc,

    AllocType_Count,
};

struct read_parameters
{
    allocation_type AllocType;
    buffer Dest;
    char const *FileName;
};

static void HandleAllocation(read_parameters *Params, buffer *Buffer)
{
    if(Params->AllocType == AllocType_malloc)
    {
        *Buffer = AllocateBuffer(Params->Dest.Count);
    }
}

static void HandleDeallocation(read_parameters *Params, buffer *Buffer)
{
    if(Params->AllocType == AllocType_malloc)
    {
        FreeBuffer(Buffer);
    }
}

#include "../listing_0110_pagefault_overhead_test.cpp"

// NOTE: taking the address through a volatile pointer keeps WriteToAllBytes
// from being optimized away entirely, since main() never calls it.
typedef void write_func(repetition_tester *, read_parameters *);
volatile write_func *KeepWriteToAllBytes = WriteToAllBytes;

int main(int ArgCount, char **Args)
{
    (void)&IsInBounds;
    (void)&AreEqual;
    return 0;
}
