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
	return (delta / 1024 + 259) % (259);
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


unsigned int get_four_values(int pid) {
	int transmitted = 0;
	unsigned int result = 0;
	unsigned int factor = 1;
	while(transmitted < 4) {
			result = result + (get_one_value(pid) - 1) * factor;
			transmitted++;
			factor = factor * 256;
	}
	return result;
}


buffer_type accept_block(int pid) {
	
	unsigned int quality = get_four_values(pid);
	unsigned int number = get_four_values(pid);
	unsigned int size = get_four_values(pid);

	char filename[512];
	sprintf(filename, "temp/%d.tmp", number);
	FILE* file;
	bool exists_flag = (access(filename, F_OK) == 0);

	if (!exists_flag) {
		file = fopen(filename, "wb");
	}

	int transmitted = 0;

	while(true) {
		int value = get_one_value(pid);
		if (value == 258) break;
		if ((value >= 1) && (value <= 256) && (!exists_flag)) {
			fputc(value - 1, file);
			transmitted++;
		}
	}

	buffer_type result;
	result.quality = quality;
	result.success = !exists_flag;

	if (!exists_flag) {
		fclose(file);
		if (transmitted != size) {
			remove(filename);
			result.success = false;
		}
	}

	// printf("quality = %d\nnumber = %d\nsize = %d\ntransmitted = %d\n", quality, number, size, transmitted);

	if ((size == transmitted) && (!exists_flag)) {
	printf("Block %d of %d transmitted\n", number, quality);
	}

	if ((size != transmitted) && (!exists_flag)) {
		printf("Block %d of %d tranmission failed\n", number, quality);
	}

	if (exists_flag) {
		printf("Block %d had transmitted before\n", number);
	}

	return result;
}


void final_split(unsigned int quality, const char* filename) {
	FILE* file = fopen(filename, "wb");
	FILE* read_file;
	char read_filename[512]; 
	int current;
	for(int i = 0; i < quality; i++) {
		sprintf(read_filename, "temp/%d.tmp", i + 1);
		read_file = fopen(read_filename, "rb");
		
		while((current = fgetc(read_file)) != EOF) {
			fputc(current, file);
		}
		fclose(read_file);
		remove(read_filename);
	}

	fclose(file);

	rmdir("temp");
}


int main() {
	int pid;
	unsigned int quality;

	char filename[512];
	
	printf("Please enter the Pid of the process: ");
	scanf("%d", &pid);

	printf("Enter the filename for transmittion: ");
	scanf("%s", filename);

	while(true) {
		printf("To start detection, enter 0: ");
		int buffer;
		scanf("%d", &buffer);
		if (buffer == 0) break;
	}

	mkdir("temp", ACCESSPERMS);

	bool init_flag = true;
	int transmitted_blocks = 0;

	while(true) {
		int value = get_one_value(pid);
		if ((value == 257) && (!init_flag)) {
			buffer_type results = accept_block(pid);
			quality = results.quality;
			if (results.success) transmitted_blocks++;
			if (transmitted_blocks == quality) break;
		}
		init_flag = false;
	}

	final_split(quality, filename);

	return 0;
}