/* asbestOS -- a best OS */

#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "filesystem.h"
#include "rtc.h"
#include "kb.h"
#include "tests_asm.h"


#define PASS 1
#define FAIL 0
#define max_kb_buf 128

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

//char test_out[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Zero Division Test
 *
 * Asserts that if a number is divided by zero, an exception is issued
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen, crashes kernel
 * Coverage: blank_page_dir, blank_page_table, loadPageDirectory,
 * 	   enablePaging, and paging_init
 * Files: paging.c/h
 */
int zero_div_test(){
	TEST_HEADER;
	int result = PASS;
	// int j = 5/0;
	// printf("j = %h\n", j);
	result = FAIL;
	return result;
}



/* Page Fault
 *
 * Asserts that if a null pointer is deferenced, the page fault detection works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen, crashes kernel
 * Coverage: blank_page_dir, blank_page_table, loadPageDirectory,
 * 	   enablePaging, and paging_init
 * Files: paging.c/h
 */
int page_fault_test1(){
	TEST_HEADER;
	int result = PASS;
	int* i = NULL;
	int j = *i;
	printf("j = %h\n", j);
	result = FAIL;
	return result;
}



/* Page Fault
 *
 * Asserts that if a pointer not in present memory is dereferenced, the page fault detection works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen, crashes kernel
 * Coverage: blank_page_dir, blank_page_table, loadPageDirectory,
 * 	   enablePaging, and paging_init
 * Files: paging.c/h
 */
int page_fault_test2(){
	TEST_HEADER;
	int result = PASS;
	/* 500 is an address that is not present lower than video memory. */
	int* i = (int*)500;
	int j = *i;
	printf("j = %h\n", j);
	result = FAIL;
	return result;
}



/* Paging Test
 *
 * Asserts that printf works, indicating that paging works
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen
 * Coverage: blank_page_dir, blank_page_table, loadPageDirectory,
 * 	   enablePaging, and paging_init
 * Files: paging.c/h
 */
int paging_test(){
	TEST_HEADER;
	int result = FAIL;
	printf("Paging works!\n");
	result = PASS;
	return result;
}



/* Paging Test 2
 *
 * Asserts that paging works by giving a value in video memory
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen
 * Coverage: blank_page_dir, blank_page_table, loadPageDirectory,
 * 	   enablePaging, and paging_init
 * Files: paging.c/h
 */
int paging_test2(){
	TEST_HEADER;
	int result = FAIL;
	/* 0xB8000 is the start of video memory */
	int* i = (int*)0xB8000;
	int j = *i;
	printf("The first value in video memory is %x\n", j);
	result = PASS;
	return result;
}



/* Paging Test 3
 *
 * Asserts that paging works by giving a value in kernel space
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen
 * Coverage: blank_page_dir, blank_page_table, loadPageDirectory,
 * 	   enablePaging, and paging_init
 * Files: paging.c/h
 */
int paging_test3(){
	TEST_HEADER;
	int result = FAIL;
	/* 1048576*4 is the first address in kernel */
	int* i = (int*)(1048576 * 4);
	int j = *i;
	printf("The first value in kernel space is %x\n", j);
	result = PASS;
	return result;
}





/* Checkpoint 2 tests */

/*
*  rtc_write_test
*	Asserts that rtc write function is operational if the user
*	can change the frequency and have it properly do so.
*	Inputs: the desired frequency (must be a power of 2)
*	Outputs: None
*	Side Effects: Changes the RTC frequency
*	Coverage: rtc.c
*/
void rtc_write_test(int32_t freq){
	clear();
	int32_t buf[1];
	buf[0] = freq;
    //int i = 0;
    // rtc_open(NULL);
    rtc_write(0,buf,4);
}


/*
*  rtc_open_test
*	Asserts that rtc open function is operational if the
*	RTC is initialized with a value of 2Hz.
*	Inputs: None
*	Outputs: None
*	Side Effects: Initializes the rtc
*	Coverage: rtc.c
*/
void rtc_open_test(){
	clear();
	rtc_open(NULL);
}



/*
*  rtc_read_test
*	Asserts that rtc read function is operational if the
*	screen is flashed only as often as the RTC receives an interrupt.
*	Inputs: the desired frequency (must be a power of 2)
*	Outputs: None
*	Side Effects: Strobes the screen on each interrupt
*	Coverage: rtc.c
*/
void rtc_read_test(int32_t freq){
	int32_t buf[1];
	buf[0] = freq;
	while(1)
	{
		test_interrupts();
		rtc_read(0, buf, 4);
	}
}

/* terminal test
*	Asserts that the terminal driver works if text can recieved,
*	stored in a buffer, then echoed back to the terminal.
*	Inputs: None
*	Outputs: Buffer to the screen.
*	Coverage: kb.c
*	Side Effects: Alters video memory.
*/
int terminal_test(){
	//TEST_HEADER;
	//move_cursor(0,0);
	int i;
	term_open(0);
	term_close(0);
	int result = PASS;
	char buf[max_kb_buf];
	for(i = 0; i < max_kb_buf; i++)
	{
		buf[i] = '\0';
	}
	char* buf2 = NULL;
	i = term_write(0, buf2, max_kb_buf);
	if(i == -1)
		term_write(0, "Bad buffer passed to term_write.\n", max_kb_buf);
	i = term_read(0, buf2, max_kb_buf);
	if(i == -1)
		term_write(0, "Bad buffer passed to term_read.\n", max_kb_buf);
	term_write(0, "Terminal write test 1!\nWait for null term.\n", max_kb_buf);
	term_write(0, "Terminal write test 2?\n", max_kb_buf);
	//term_write("Terminal write test 3?!", 23);
	term_write(0, "Enter some text please: ", max_kb_buf);
	term_read(0, buf, max_kb_buf);
	term_write(0, "You entered: ", max_kb_buf);
	term_write(0, buf, max_kb_buf);
	term_write(0,"Now wasn't that nifty?\n", max_kb_buf);
	printf("The remaining lines use the now fully integrated printf functionality.\n");
	printf("Here is a for loop which increments.\n");
	for(i = 1; i < 16; i++)
	{
		printf("Current number is %d.\n", i);
	}
	printf("Press enter to scroll.\n");
	printf("Press ctrl+l to clear.\n");
	return result;

}


/* Checkpoint 2 tests */

/* D Entry Name Test
 *
 * Asserts that read_dentry_by_name returns the correct name, type, and size
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen
 * Files: filesystem.c/h
 */
int filesystem_dentry_nametest(){
	TEST_HEADER;
	int result = FAIL;
	uint32_t de[sizeof(dentry_t)];
	dentry_t *testdentry = (dentry_t *)(&de);
	int i = 0;
	int res;
	res = read_dentry_by_name((const uint8_t *)"verylargetextwithverylongname.tx", testdentry);
	if(res == 0)
	{
		for(; i < STRING_NAME_SIZE; i++)
			term_putc(testdentry->filename[i]);
		printf("\ntype: %d\n", testdentry->type);
		printf("node index: %d\n", testdentry->nodeIndex);
	}

	result = PASS;
	return result;
}

/* Print All Files
 *
 * Asserts that all of the files can be printed, along with the file sizes
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen
 * Files: filesystem.c/h
 */
int filesystem_print_all_files(){
	TEST_HEADER;
	int result = FAIL;
	int32_t i, size;
	int res;
	uint32_t de[sizeof(dentry_t)];
	dentry_t *testdentry = (dentry_t *)(&de);
	int j = 0;
	for(i = 0; i < DENTRY_SIZE; i++)
	{
		res = read_dentry_by_index(i, testdentry);
		if(res != -1)
		{
			size = get_file_size(testdentry->nodeIndex);
			printf("filename[%d]: ", i);
			for(j = 0; j < (STRING_NAME_SIZE); j++)
				term_putc((testdentry->filename)[j]);
			printf("      size:%d\n", size);
		}
		else
			break;
	}
	result = PASS;
	return result;
}

/* Test D-entry Index
 *
 * Asserts that a dentry can be returned from the index dentry funciton
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen
 * Files: filesystem.c/h
 */
int filesystem_dentry_indextest(){
	TEST_HEADER;
	int result = FAIL;
	uint32_t de[sizeof(dentry_t)];
	dentry_t *testdentry = (dentry_t *)(&de);
	int i = 0;
	int res;
	res = read_dentry_by_index(3, testdentry);
	if(res == 0)
	{
		printf("filename: ");
		for(; i < STRING_NAME_SIZE; i++)
			term_putc(testdentry->filename[i]);
		printf("\ntype: %d\n", testdentry->type);
		printf("node index: %d\n", testdentry->nodeIndex);
	}

	result = PASS;
	return result;
}

/* Print File test
 *
 * Asserts that read_data works by printing a file to the screen
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen
 * Files: filesystem.c/h
 */
int filesystem_print_file_test(){
	TEST_HEADER;
	int result = FAIL;
	uint32_t de[sizeof(dentry_t)];
	dentry_t *testdentry = (dentry_t *)(&de);
	char buf[BLOCK_BYTE_SIZE];
	int k= 0, bytesRead = 1, offset = 0;
	int filesize = 0;
	int res;

	// res = read_dentry_by_name((const uint8_t *)"frame0.txt", testdentry);
	res = read_dentry_by_index(4, testdentry);

	if(res == 0)
	{
		filesize = get_file_size(testdentry->nodeIndex);

		while(bytesRead > 0)
		{
			bytesRead = read_data(testdentry->nodeIndex, offset*BLOCK_BYTE_SIZE, (uint8_t *)buf, BLOCK_BYTE_SIZE);
			offset++;
			if(bytesRead > 0)
			{
				for(k = 0; k < bytesRead; k++)
				{
					//term_putc(test_out[buf[k]]);
					term_putc(buf[k]);
				}
			}
		}
	}
	result = PASS;
	return result;
}

/* Open/Read File test
 *
 * Asserts that read_data works by printing a file to the screen
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen
 * Files: filesystem.c/h
 */
int filesystem_open_read_file_test(){
	TEST_HEADER;
	int result = FAIL;
	char buf[BLOCK_BYTE_SIZE];
	int k= 0, bytesRead = 1, fd;

	fd = open_file("verylargetextwithverylongname.tx");
	while(bytesRead > 0)
	{
		bytesRead = read_file(fd, (uint8_t *)buf, BLOCK_BYTE_SIZE);
		for(k = 0; k < bytesRead; k++)
		{
			term_putc(buf[k]);
		}
	}
	result = PASS;
	return result;
}

/* Open/Read File test
 *
 * Asserts that read_data works by printing a file to the screen
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen
 * Files: filesystem.c/h
 */
int filesystem_open_read_dir_test(){
	TEST_HEADER;
	int result = FAIL;
	char buf[BLOCK_BYTE_SIZE];
	int k= 0, bytesRead = 1;
	int fd;

	fd = open_dir(".");
	while(bytesRead > 0)
	{
		bytesRead = read_dir(fd, (uint8_t *)buf, STRING_NAME_SIZE);
		if(bytesRead <= 0)
			break;
		for(k = 0; k < bytesRead; k++)
		{
			term_putc(buf[k]);
		}
		printf("\n");
	}
	printf("------");
	result = PASS;
	return result;
}

/* Checkpoint 3 tests */


/* Syscall test
 *
 * Asserts that read_data works by printing a file to the screen
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: Prints to screen
 * Files: filesystem.c/h
 */
// int syscall_open_and_read_file_test(){
// 	TEST_HEADER;
// 	int result = FAIL;
// 	int fd, res = -1;
// 	int buf[BLOCK_BYTE_SIZE];

// 	fd = syscalls_test_open("frame0.txt");
// 	res = syscalls_test_read(fd,buf, BLOCK_BYTE_SIZE);
// 	syscalls_test_close(fd);
// 	if(res == -1)
// 		return FAIL;
// 	else
// 	{
// 		term_write(1, buf, BLOCK_BYTE_SIZE);
// 		// for(k = 0; k < res; k++)
// 		// {
// 		// 	term_putc(buf[k]);
// 		// }

// 	}

// 	printf("------");

// 	result = PASS;
// 	return result;
// }

/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// launch your tests here
	clear();
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("paging", paging_test());
	// TEST_OUTPUT("paging in video memory", paging_test2());
	// TEST_OUTPUT("paging in kernel space", paging_test3());
	// clear();
	// TEST_OUTPUT("page fault 1", page_fault_test1());
	// TEST_OUTPUT("page fault 2", page_fault_test2());
	// TEST_OUTPUT("divide_by_zero", zero_div_test());

	// clear();
	// CHECKPOINT 2 TESTS
	// TEST_OUTPUT("list all files", filesystem_print_all_files());
	// TEST_OUTPUT("Dentry name test", filesystem_dentry_nametest());
	// TEST_OUTPUT("Dentry index test", filesystem_dentry_indextest());
	// TEST_OUTPUT("Read data", filesystem_print_file_test());
	// TEST_OUTPUT("Open/Read File", filesystem_open_read_file_test());
	// TEST_OUTPUT("Open/Read File", filesystem_open_read_dir_test());
	// rtc_open_test();
	// rtc_write_test(2);
	// rtc_read_test(2);
	// terminal_test();
	// filesystem_dentry_indextest();
	// filesystem_dentry_nametest();
	// filesystem_print_file_test();
	// filesystem_print_all_files();
	// filesystem_open_read_file_test();
	// filesystem_open_read_dir_test();

	// syscall_open_and_read_file_test();
	// TEST_OUTPUT("Open/Read File Syscall", syscall_open_and_read_file_test());

}
