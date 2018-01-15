#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
using namespace std;

const int WAIT = 9999;

struct tlbData{
	unsigned int page_num;
	unsigned int frame_num;
	int counter;
};

int main(int argc, char *argv[]) {

	FILE *backingStore = fopen(argv[1], "r");
	ifstream inFile_address(argv[2]);
	FILE *output = fopen("results.txt","w");

	int num_address = 0;
	inFile_address >> num_address;

	// tlb, pagetable, physicalmem
	tlbData tlb[16];
	int pageTable[256];
	char physicalmemory[65536];

	// initialize pageTable and tlb
	for (int i = 0; i < 256 ; ++i){
		pageTable[i] = -1;
		if (i < 16) {
			tlb[i].page_num = -1;
			tlb[i].frame_num = -1;
			tlb[i].counter = WAIT;
		}
	}

	int tlb_size = 0;
	int frame_num = 0;
	int tlb_Hit_Count = 0;
	int page_Fault_Count = 0;

	// buffer for reading backstorage
	char buffer[256];

	while (!inFile_address.eof()) {
		int logical_address = 0;
		int physical_address = 0;
		int page_num = 0;
		int offset = 0;

		// Get page number and offset from the integer
		inFile_address >> logical_address ;
		page_num = logical_address / 256;
		offset = logical_address % 256;

		bool hit = false;


		// Search through tlb for a match
		for (int index = 0; index < tlb_size ; ++index) {
			if (tlb[index].page_num == page_num) {
				hit = true;
				tlb[index].counter = 0; //reset counter
				physical_address = tlb[index].frame_num * 256 + offset;
				// need aging others
				for (int i = 0 ; i < 16 ; ++i) {
					if (i != index &&  tlb[i].counter != WAIT)
						tlb[i].counter += 1;
				}
			}
		}
		if (hit) {
			tlb_Hit_Count++;
		} else if (pageTable[page_num] == -1) { 
			// read from backing storage
			fseek(backingStore, page_num*256, SEEK_SET);
			fread(buffer, sizeof(char), 256, backingStore);
			pageTable[page_num] = frame_num;
			// store value from backing storage into physicalmemory
			for (int i = 0; i < 256 ; ++i) {
				physicalmemory[frame_num*256 + i] = buffer[i];
			}
			// TLB miss case 1 : Page Fault occurs
			if (tlb_size < 16){
				tlb[tlb_size].page_num = page_num;
				tlb[tlb_size].frame_num = pageTable[page_num];
				tlb[tlb_size].counter = 0;
				for (int i = 0 ; i < 16 ; ++i)
					if (tlb[i].counter != WAIT && i != tlb_size)
						tlb[i].counter += 1;
				tlb_size++;
			} else {
				// LRU algorithm
				int max = -1;
				int index = -1;
				for (int i = 0 ; i < 16 ; ++i){
					if (tlb[i].counter > max ){
						max = tlb[i].counter;
						index = i;
					}
				}
				tlb[index].page_num = page_num;
				tlb[index].frame_num = pageTable[page_num];
				tlb[index].counter = 0;
				// need aging others
				for (int i = 0 ; i < 16 ; ++i) {
					if (i != index && tlb[i].counter != WAIT)
						tlb[i].counter += 1;
				}
			}

			page_Fault_Count++;
			frame_num++;

			physical_address = pageTable[page_num] * 256 + offset;
		} else {
			physical_address = pageTable[page_num] * 256 + offset;

			// TLB miss case 2 : Page Fault doesn't occur, TLB must be fulled
			// LRU Algorithm
			int max = -1;
			int index = -1;
			for (int i = 0 ; i < 16 ; ++i){
				if (tlb[i].counter > max ){
					max = tlb[i].counter;
					index = i;
				}
			}
			tlb[index].page_num = page_num;
			tlb[index].frame_num = pageTable[page_num];
			tlb[index].counter = 0;
			for (int i = 0 ; i < 16 ; ++i) {
				if (i != index && tlb[i].counter != WAIT)
					tlb[i].counter += 1;
			}
		}
		int value = physicalmemory[physical_address];
		fprintf(output, "%d %d \n", physical_address, value);
	}
	fprintf(output, "TLB hits: %d\n", tlb_Hit_Count);
  	fprintf(output, "Page Faults: %d\n", page_Fault_Count);
  	
	return 0;
}