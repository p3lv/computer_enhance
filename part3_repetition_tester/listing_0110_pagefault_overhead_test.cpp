/* ========================================================================

   (C) Copyright 2023 by Molly Rocket, Inc., All Rights Reserved.
   
   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.
   
   Please see https://computerenhance.com for more information
   
   ======================================================================== */

/* ========================================================================
   LISTING 110
   ======================================================================== */

struct read_tester {
	char *filename;
	u8 *buffer;
	u64 bsize;
};

void TestFread(char *name, read_tester *read_t) {
	FILE *fp = fopen(read_t->filename, "r");
	if (!fp) {
		fclose(fp);
		fprintf(stderr, "File '%s' could not open\n", read_t->filename);
		return;
	}

	u8 *buffer = read_t->buffer;
	bool internal_buffer = false;
	if (buffer == NULL) {
		internal_buffer = true;
		buffer = (u8 *) malloc(read_t->bsize);
	}
	
	repetition_tester rep;
	InitTester(&rep, name, read_t->bsize);

	while (IsRunning(&rep)) {
		BeginTime(&rep);
		u64 n_bytes = (u64) fread(buffer, 1, read_t->bsize, fp);
		// fprintf(stderr, "%lu:%lu\n", n_bytes, read_t->bsize);
		EndTime(&rep, n_bytes);

		rewind(fp);
	}
	fclose(fp);
	if (internal_buffer) {
		free(buffer);
	}

	PrintRepetitionTester(&rep);
}

// static void WriteToAllBytes(repetition_tester *Tester, read_parameters *Params)
// {
//     while(IsTesting(Tester))
//     {
//         buffer DestBuffer = Params->Dest;
//         HandleAllocation(Params, &DestBuffer);
//         
//         BeginTime(Tester);
//         for(u64 Index = 0; Index < DestBuffer.Count; ++Index)
//         {
//             DestBuffer.Data[Index] = (u8)Index;
//         }
//         EndTime(Tester);
//         
//         CountBytes(Tester, DestBuffer.Count);
//         
//         HandleDeallocation(Params, &DestBuffer);
//     }
// }
