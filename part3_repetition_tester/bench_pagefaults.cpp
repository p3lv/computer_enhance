#include <cstdlib>
#include <cstdio>

#include "repetition_tester.cpp"

typedef uint8_t u8;

struct write_tester {
	char *log_filename;
	u64 bsize;
};

void TestLinearWrite(char *name, write_tester *params) {
	repetition_tester rt;
	InitTester(&rt, name, params->bsize);
	InitLogger(&rt, params->log_filename);

	while (IsRunning(&rt)) {
		char *buffer = (char *) malloc(params->bsize);

		BeginTime(&rt);
		for (u64 i = 0; i < params->bsize; i++) {
			buffer[i] = (u8) i;
		}
		EndTime(&rt, params->bsize);
		
		free(buffer);
	}

	PrintRepetitionTester(&rt);
}

struct pf_event {
	u32 i;
	u32 new_since_last;
	u64 cumul;
};

void TestPFLinear(char *name, write_tester *params) {
	fprintf(stdout, "----%s----\n", name);

	pf_event *pf_events = (pf_event *) malloc(sizeof(pf_event) * params->bsize);
	u32 last_n_pf = 0;
	u64 cumul = 0;

	char *buffer = (char *) malloc(params->bsize);
	for (u32 i = 0; i < params->bsize; i++) {
		buffer[i] = (u8) i;

		u64 n;
		assert(ReadPageFaults(&n) == 0);

		cumul += n;

		pf_events[i].i = i;
		pf_events[i].cumul = cumul;
		pf_events[i].new_since_last = n - last_n_pf;

		last_n_pf = n;
	}

	fprintf(stdout, "Done collecting\n");

	FILE *fp = fopen(params->log_filename, "w");
	if (!fp) {
		fprintf(stderr, "error opening\n");
		return;
	}

	fprintf(fp, "i,\"cumulative pagefaults\",\"new pagefaults\"\n");
	for (u32 i = 0; i < params->bsize; i++) {
		fprintf(fp, "%u,%lu,%u\n", pf_events[i].i, pf_events[i].cumul, pf_events[i].new_since_last);
	}

	fclose(fp);
	free(buffer);
	free(pf_events);
}

int main () {
	u64 buffer_size = 1024 * 1024 * 100;
	write_tester linear_pf = {.log_filename = (char *) "linear_pagefaults.csv", .bsize = buffer_size};
	
	// TestLinearWrite((char *) "linear write", &linear_pf);
	TestPFLinear((char *) "pf events" , &linear_pf);
	
	return 0;
}
