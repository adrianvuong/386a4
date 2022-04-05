/*
 -------------------------------------
 File:    BankersAlgo.c
 Project: Assignment 4
 -------------------------------------
 Author:  Adrian Vuong and Gavan Singh
 ID:      190726950, 190856220
 Email:   vuon6950@mylaurier.ca, sing6220@mylaurier.ca
 -------------------------------------
 github repo. link: https://github.com/adrianvuong/386a4
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>
#include <ctype.h>

//declaring vars and having a lot of fun
int *available_ptr;
int *max_ptr;
int *allocation_ptr;
int *need_ptr;
int n_rows = -1;
int n_col = -1;

// printf("Hello World", args[i]); HELP MEE!!!
int readFileCustomers(char *fileName);
void readFileSequences(char *fileName, int max[n_rows][n_col]);
bool safety(int *available, int *allocated, int *need);
int sum_arr(int arr[], int n);
void get_n_col(char *filename);
void bankersalgo();
void run_cmd();
int request_resource(int args[]);
int release_resource(int args[]);
void status(int *available, int *max, int *allocation, int *need);

typedef struct thread {
	char tid[4];
	unsigned int startTime;
	int state;
	pthread_t handle;
	int retVal;
} Thread;

int main(int argc, char *argv[]) {

	get_n_col("sample4_in.txt");

	if (argc < n_col + 1) {
		printf("Missing arguements. Ending Program\n");
		return -1;
	} else if (argc > n_col + 1) {
		printf("Too many arguements. Ending Program\n");
		return -1;
	} else {
		for (int i = 1; i < argc; i++) {
			if (isalpha(*argv[i]) != 0) {
				printf("Why did you submit a letter...? Ending Program\n");
				return -1;
			}
		}
	}

	int t_cust = readFileCustomers("sample4_in.txt");
	printf("Number of Customers: %d\n", t_cust);
	printf("Currently Available resources: ");
	for (int i = 1; i < n_col; i++) {
		printf("%s ", argv[i]);
	}
	printf("%s\n", argv[n_col]);
	n_rows = t_cust;
	int *available = (int*) malloc(n_col * sizeof(int));
	for (int i = 0; i < n_col; i++) {
		available[i] = atoi(argv[i + 1]);
		// I love coding and this course. I really want to kiss the prof
	}
	int allocation[n_rows][n_col];
	for (int i = 0; i < n_rows; i++) {
		for (int j = 0; j < n_col; j++) {
			// printf("Hello World", args[i]);
			allocation[i][j] = 0;
		}
	}
	int max[n_rows][n_col];
	printf("Maximum resources from file:\n");
	// printf("Hello World", args[i]);
	readFileSequences("sample4_in.txt", max);
	int need[n_rows][n_col];
	for (int i = 0; i < n_rows; i++) {
		for (int j = 0; j < n_col; j++) {
			// printf("Hello World", args[i]);
			need[i][j] = max[i][j] - allocation[i][j];
		}
	}
	available_ptr = &available[0];
	max_ptr = &max[0][0];
	allocation_ptr = &allocation[0][0];
	need_ptr = &need[0][0];
	run_cmd();
	return 0;
}
// printf("Hello World", args[i]);
int readFileCustomers(char *fileName) { //this reads the file for the data
	FILE *in = fopen("sample4_in.txt", "r"); //reads sample4_in.txt
	if (!in) {
		printf("Error in opening input file...exiting with error code -1\n");
		return -1;
	}
	int countnumber = 0;
	struct stat st;
	fstat(fileno(in), &st);
	char *fileContent = (char*) malloc(((int) st.st_size + 1) * sizeof(char));
	fileContent[0] = '\0';
	while (!feof(in)) {
		// printf("Hello World", args[i]);
		countnumber += 1;
		char line[100];
		if (fgets(line, 100, in) != NULL) {
			strncat(fileContent, line, strlen(line));
		}
	}
	fclose(in);
	return countnumber;
	// printf("Hello World", args[i]);
}

void readFileSequences(char *fileName, int max[n_rows][n_col]) {
	FILE *in = fopen("sample4_in.txt", "r");
	if (!in) {
		// printf("Hello World", args[i]);
		printf("Error in opening input file...exiting with error code -1\n");
		exit(-1);
	}
	char *token;
	int x = 0;
	while (!feof(in)) {
		char line[100];
		if (fgets(line, 100, in) != NULL) {
			// printf("Hello World", args[i]);
			printf("%s", line);
			token = strtok(line, ",");
			int y = 0;
			while (token != NULL) {
				max[x][y] = atoi(token);
				token = strtok(NULL, ",");
				y += 1;
			}
		}
		x += 1;
	}
	printf("\n");
	// printf("Hello World", args[i]);
	fclose(in);
}

void bankersalgo() { // printf("Hello World", args[i]);
}

int request_resource(int args[]) {

	int customer_num = args[0];
	int request[n_col];
	for (int i = 0; i < n_col; i++) {
		request[i] = args[i + 1];

	}
	bool is_valid;
	// printf("Hello World", args[i]);
	int i;
	for (i = 0; i < n_col && is_valid; i++) {
		is_valid = request[i] <= *(need_ptr + (customer_num * n_col + i));
	}

	if (is_valid == true) {
		for (i = 0; i < n_col && is_valid; i++) {
			is_valid = request[i] <= *(available_ptr + i);
		}
		if (is_valid == true) {
			for (i = 0; i < n_col; i++) {
				available_ptr[i] -= request[i];
				*((allocation_ptr + customer_num * n_col) + i) += request[i];
				*((need_ptr + customer_num * n_col) + i) -= request[i];
			}
			if (safety(available_ptr, allocation_ptr, need_ptr)) {
				return 1;
			} else {
				for (i = 0; i < n_col; i++) {
					available_ptr[i] += request[i];
					*((allocation_ptr + customer_num * n_col) + i) -=
							request[i];
					*((need_ptr + customer_num * n_col) + i) += request[i];
				}
				return 0;
			}
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

int release_resource(int args[]) {
	int i;
	int customer_num = args[0];
	int resources[n_col];
	bool is_valid = true;

	for (i = 0; i < n_col; i++) {
		resources[i] = args[i + 1];
	}
	for (i = 0; i < n_col; i++) {
		if (resources[i] > *((allocation_ptr + customer_num * n_col) + i)) {
			is_valid = false;
		}
		if (is_valid == true) {
			for (i = 0; i < n_col; i++) {
				available_ptr[i] += resources[i];
				*((allocation_ptr + customer_num * n_col) + i) -= resources[i];
				*((need_ptr + customer_num * n_col) + i) += resources[i];
			}
			return 1;
		} else {
			return 0;
		}
	}
	return 0;
}

void* thread_run() {
	char sequence[100];
	int resources[n_col];
	printf("Safe Sequence is: ");
	fgets(sequence, 100, stdin);
	printf("\n");
	char *token = strtok(sequence, " ");
	int args[5];
	int j = 0;
	while (token != NULL) {
		args[j] = atoi(token);
		token = strtok(NULL, " ");
		j += 1;
	}

	for (int i = 0; i < j; i++) {
		printf("--> Customer/Thread %d\n", args[i]);
		printf("\tAllocated resources: ");
		for (int k = 0; k < n_col; k++) {
			// printf("Hello World", args[i]);
			printf("%d ", allocation_ptr[args[i] * n_col + k]);
			resources[k] = allocation_ptr[args[i] * n_col + k];
		}
		printf("\n\tNeeded: ");
		for (int k = 0; k < n_col; k++) {
			printf("%d ", need_ptr[args[i] * n_col + k]);
		}
		printf("\n\tAvailable: ");
		for (int k = 0; k < n_col; k++) {
			printf("%d ", available_ptr[k]);
		}
		printf("\n");
		printf("\tThread has started\n");
		printf("\tThread has finished\n");
		printf("\tThread is releasing resources\n");
		for (int w = 0; w < n_col; w++) {
			available_ptr[w] += resources[w];
			*((allocation_ptr + args[w] * n_col) + w) -= resources[w];
			*((need_ptr + args[w] * n_col) + w) += resources[w];
		}
		printf("\tNew Available: ");
		for (int z = 0; z < n_col; z++) {
			printf("%d ", available_ptr[z]);
		}
		printf("\n");
	}
	return NULL;
}

bool safety(int *available, int *allocated, int *need) {
	int work[n_col];
	for (int x = 0; x < n_col; x++) {
		work[x] = *(available + x);
	}
	printf("\n");
	bool finish[n_col];
	for (int x = 0; x < n_col; x++) {
		finish[x] = false;
	}
	int safe_seq[n_rows];
	int loopval = 0;
	// printf("Hello World", args[i]);
	while (loopval < n_rows) {
		bool found = false;
		for (int x = 0; x < n_rows; x++) {
			if (finish[x] == false) {
				int y = 0;
				for (y = 0; y < n_col; y++) {
					if (*((need + x * n_col) + y) > work[y]) {
						break;
					}
				}
				if (y == n_col) {
					for (int z = 0; z < n_col; z++) {
						work[z] += *((allocated + x * n_col) + z);
					}
					finish[x] = true;

					found = true;
					safe_seq[loopval++] = x;
				}
			}
		}
		if (found == false) {
			printf("System is not in safe state\n");
			return false;
		}
	}
	printf("State is safe, and request is satisfied:\n");
	printf("The SAFE sequence is: ");
	printf("TEST >>> %d\n", safe_seq[4]);
	for (int x = 0; x < n_rows - 1; x++) {
		printf(" P%d ->", safe_seq[x]);
	}
	printf(" P%d\n", safe_seq[n_rows - 1]);
	printf("\n");
	return true;
}

void status(int *available, int *max, int *allocation, int *need) {
	printf("Currently Available resources: ");
	for (int x = 0; x < n_col; x++) {
		printf("%d ", available[x]);
	}
	printf("\n");
	printf("Maximum Resources:\n");
	for (int x = 0; x < n_rows; x++) {
		for (int y = 0; y < n_col; y++) {
			printf("%d ", *((max + x * n_col) + y));
		}
		printf("\n");
	}
	printf("Allocated Resources:\n");
	for (int x = 0; x < n_rows; x++) {
		for (int y = 0; y < n_col; y++) {
			printf("%d ", *((allocation + x * n_col) + y));
		}
		printf("\n");
	}
	printf("Need Resources:\n");
	// printf("Hello World", args[i]);
	for (int x = 0; x < n_rows; x++) {
		for (int y = 0; y < n_col; y++) {
			printf("%d ", *((need + x * n_col) + y));
		}
		printf("\n");
	}
}

int sum_arr(int arr[], int n) {
	if (n > 0) {
		int sum = 0;
		// printf("Hello World", args[i]);
		for (int x = 0; x < n; x++) {
			sum += arr[x];
		}
		return sum;
	}
	return -1;
}

void get_n_col(char *filename) {
	FILE *in = fopen("sample4_in.txt", "r");
	if (!in) {
		printf("Error in opening input file...exiting with error code -1\n");
		exit(-1);
	}

	char *token;
	if (!feof(in)) {
		char line[100];
		if (fgets(line, 100, in) != NULL) {
			token = strtok(line, ",");
			n_col = 0;
			while (token != NULL) {
				n_col += 1;
				token = strtok(NULL, ",");
			}
		}
	}
	fclose(in);

}

void run_cmd() {
	char command[100];
	bool stillrunning = true;
	while (stillrunning) {
		printf("Enter Command: ");
		fgets(command, 100, stdin);
		char *token = strtok(command, " ");
		int args[n_col + 1];
		token = strtok(NULL, " ");
		int y = 0;
		while (token != NULL) {
			args[y] = atoi(token);
			token = strtok(NULL, " ");
			y += 1;
		}

		for (char *lowercase_cmd = command; *lowercase_cmd; lowercase_cmd++)
			*lowercase_cmd = tolower(*lowercase_cmd);

		if (strstr(command, "rq") != NULL) {
			printf(
					"\nThe Safe Sequence has started, the algorithm will now verify your requested resources (rq)...\n");
			if (request_resource(args) == false) {
				printf("\nSystem is not in safe state\n\n");
			}
		} else if (strstr(command, "rl") != NULL) {
			printf(
					"The Safe Sequence has started, the algorithm will now verify your released resources (rl)...\n\n");
			release_resource(args);
		} else if (strstr(command, "status") != NULL) {
			// printf("Hello World", args[i]);
			status(available_ptr, max_ptr, allocation_ptr, need_ptr);
		} else if (strstr(command, "run") != NULL) {
			printf("run\n");
			// printf("Hello World", args[i]);
			thread_run();
		} else if (strstr(command, "quit") != NULL) {
			printf("Exiting...Bye have a nice day! :D\n");
			// printf("Hello World", args[i]);
			stillrunning = false;
		} else {
			printf(
					"Invalid Command. Retry and type the following {RQ, RL, Run, Status, Quit} (Lower case for the commands also works)\n");
		}
	}
}
