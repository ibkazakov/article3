#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


#define SMALL_INTERVAL 25
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

	~MemoryManipulator() {
		set(0);
	}

};


void blocks_sender(MemoryManipulator* manipulator, unsigned long size, unsigned long quality) {
	for(unsigned long i = 0; i < quality; i++) {

			usleep(BIG_INTERVAL);
			for(unsigned long j = 0; j < size; j++) {
				usleep(SMALL_INTERVAL);
				manipulator->transmit(1);
				//usleep(SMALL_INTERVAL);
			}
			usleep(BIG_INTERVAL);

		printf("Block %lu was sent \n", i);
	}

	
/*	while(true) {
		manipulator->transmit(1);
		usleep(BIG_INTERVAL);
	}

*/
	
//	manipulator -> transmit(1);

}





int main() {
	printf("This process ID of this program is %d\n", getpid());

	unsigned long size;
	unsigned long quality;

	printf("Enter block size: \n");
	scanf("%lu", &size);
	printf("Enter quality of blocks: \n");
	scanf("%lu", &quality);

	MemoryManipulator* manipulator = new MemoryManipulator(1024*1024, 256);


	while(true) {
		printf("Enter 0 for transmission begin: ");
		int buffer;
		scanf("%d" , &buffer);
		if (buffer == 0) break;
	}


	

	blocks_sender(manipulator, size, quality);
	
	delete manipulator;

	return 0;
}