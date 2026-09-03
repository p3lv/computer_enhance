#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstdint>

typedef uint64_t u64;

#include "listing_0108_platform_metrics.cpp"
#include "listing_0110_pagefault_overhead_test.cpp"

int main (int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Need at least 2 arguments\n");
		exit(1);
	}

	const auto command = std::string(argv[1]);
	if (!command.compare("freq") {
		const auto freq = EstimateCPUTimerFreq();
		fprintf(stdout, "CPU_Frequency %lu\n", freq);
	} else if (!command.compare("l110") {
		WriteToAllBytes(repetition_tester *Tester, read_parameters *Params)
	}
}
