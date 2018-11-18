#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


#define SMALL_INTERVAL 100
#define BIG_INTERVAL 10000


class MemoryManipulator {
private:

	int block_size;
	int capacity;
	int current_blocks;
	
	void increase(int delta) {
		sbrk(delta*block_size);
		current_blocks += delta;
	}

public:

	MemoryManipulator(int block_size, int capacity) {
		this->block_size = block_size;
		this->capacity = capacity;
		this->current_blocks = 0;
	}

	void set(int number) {
		if ((0 <= number) && (number <= capacity)) {
				increase(number - current_blocks);
		}
		else {
			printf("Out of bounds exception!\n");
			printf("number = %d\n", number);
		}
	}

	void transmit(int number) {
		set((current_blocks + number) % (capacity + 1));
	}

	void transmit_int(unsigned int number) {
		for(int i = 0; i < 4; i++) {
			usleep(BIG_INTERVAL);
			transmit(number % 256 + 1);
			number = number / 256;
		}
	}

	~MemoryManipulator() {
		set(0);
	}

};

void send_block(MemoryManipulator* manipulator, unsigned int quality, unsigned int number, unsigned int size, FILE* file) {
	usleep(BIG_INTERVAL);
	manipulator->transmit(257);
	usleep(BIG_INTERVAL);

	manipulator->transmit_int(quality);
	usleep(BIG_INTERVAL);
	manipulator->transmit_int(number);
	usleep(BIG_INTERVAL);
	manipulator->transmit_int(size);
	usleep(BIG_INTERVAL);

	for(int i = 0; i < size; i++) {
		manipulator->transmit(fgetc(file) + 1);
		usleep(SMALL_INTERVAL);
	}

	usleep(BIG_INTERVAL);
	manipulator->transmit(258);
	usleep(BIG_INTERVAL);
}

void blocks_sender(MemoryManipulator* manipulator, unsigned int size, FILE* file) {
	fseek(file, 0, SEEK_END);
	long int file_size = ftell(file);
	unsigned int remain = (unsigned int)(file_size % size);
	unsigned int q = (unsigned int)(file_size / size);

	unsigned int quality = (remain != 0) ? q + 1 : q;

	while(true) {
		fseek(file, 0, SEEK_SET);
		for(int i = 0; i < q; i++) {
			send_block(manipulator, quality, i + 1, size, file);
			printf("Block %d transmitted\n", i + 1);
		}

		if (remain != 0) {
			send_block(manipulator, quality, quality, remain, file);
			printf("Remainer block transmitted\n");
		}
	}
}




int main() {
	printf("This process ID of this program is %d\n", getpid());

	char filename[512];
	printf("Enter the filename for transmittion: ");
	scanf("%s", filename);

	while(true) {
		printf("Enter 0 for transmission begin: ");
		int buffer;
		scanf("%d" , &buffer);
		if (buffer == 0) break;
	}


	FILE* file = fopen(filename, "rb");
	MemoryManipulator* manipulator = new MemoryManipulator(1024*1024, 258);

	blocks_sender(manipulator, 1024, file);
	
	delete manipulator;
	fclose(file);
	

	return 0;
}