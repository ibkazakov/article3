#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct{
	unsigned int quality;
	bool success;
} buffer_type;


int getVmData(int pid) {
	char filename[512];
	snprintf(filename, sizeof(filename), "/proc/%d/status", pid);
	FILE * file = fopen(filename, "r");
	char current_string[512];
	const char* VmData = "VmData:";
	int result = 0;

	bool is_VmData = false;

	while(true) {
		fscanf(file, "%s", current_string);

		if (is_VmData) {
			sscanf(current_string, "%d", &result);
			is_VmData = false;
			break;
		}

		if (strcmp(VmData, current_string) == 0) {
			is_VmData = true;
		}
	}

	if (file != NULL) fclose(file);
	return result;
}



int decode_delta(int delta) {
	return (delta / 1024 + 257) % (257);
}

int get_one_value(int pid) {
	int currentVmData = getVmData(pid);
	int previousVmData = currentVmData;
	while(true) {
		previousVmData = currentVmData;
		currentVmData = getVmData(pid);
		int value = decode_delta(currentVmData - previousVmData);
		if (value != 0) return value;
	}
}


unsigned long accept_block(int pid, unsigned long size) {
	
	unsigned long transmitted = 0;
	unsigned long delta = 0;
	int currentVmData = getVmData(pid);
	int previousVmData = previousVmData;
	int value = 0;

	while(true) {
		previousVmData = currentVmData;
		currentVmData = getVmData(pid);
		value = decode_delta(currentVmData - previousVmData);
		transmitted += value;
		if (value > 1) delta += (value - 1);
		if (transmitted == size) break;
	}

	return delta;
}


void print_results(unsigned long size, unsigned long quality, unsigned long max_delta,  unsigned long* counter) {
	FILE* f = fopen("report.txt", "w+");
	fprintf(f, "blocks = %lu\nblock_size = %lu\n", quality, size);
	for(unsigned long i = 0; i <= max_delta; i++) {
		fprintf(f, "%lu blocks of delta = %lu\n", counter[i], i);
	}
}

int main() {
	int pid;
	unsigned long quality;
	unsigned long size;
	unsigned long counter[256];
	

	for(int i = 0; i < 256; i++) {
		counter[i] = 0;
	}

	
	printf("Please enter the Pid of the process: ");
	scanf("%d", &pid);
	printf("Please enter size of block: \n");
	scanf("%lu", &size);
	printf("Please enter quality of blocks: \n");
	scanf("%lu", &quality);


	while(true) {
		printf("To start detection, enter 0: ");
		int buffer;
		scanf("%d", &buffer);
		if (buffer == 0) break;
	}

	unsigned long max_delta = 0;

	for(unsigned long i = 0; i < quality; i++) {
		unsigned long delta = accept_block(pid, size);
		if (delta > max_delta) max_delta = delta;
		counter[delta]++;
		printf("Block %lu accepted, delta = %lu\n", i, delta);
	}

	print_results(size, quality, max_delta, counter);

	return 0;
}