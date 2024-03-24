#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define FILELOCATION "/home/furkan/Desktop/CMPE382/HW1/numbers.txt"
#define SLEEPDURATIONBETWEENWRITES 2000

int main(int argc, char *argv[])
{
    // Function prototypes
    int getDigitCount(int number);
    int isPrime(int number);
    int printDigitCountFromArr(int arr[], int size);
    int printIsPrimeFromArr(int arr[], int size);

    int child_status1; // Status of the first child process
    int child_status2; // Status of the second child process

    __pid_t child1_pid, child2_pid; // PIDs of the child processes

    // Create 4 pipes (pipe[0] is for reading, pipe[1] is for writing)
    int pipe1[2], pipe2[2];
    pipe(pipe1); // First arrow from left (P1 --> P2)
    pipe(pipe2); // Second arrow from left (P2 --> P1)

    child1_pid = fork(); // Create the first child process

    if (child1_pid == 0){ // First child process (P2)

        close(pipe1[1]); // Close the writing end of the pipe1
        close(pipe2[0]); // Close the reading end of the pipe2

        int read_number = 0;
        while(read_number != -1){
            read(pipe1[0],&read_number,sizeof(read_number)); // Read the current number from the pipe1

            int digit_count = getDigitCount(read_number); // Get the number of digits of the current number
            write(pipe2[1], &digit_count, sizeof(digit_count)); // Write the number of digits of the current number to the pipe2
        }

        exit(0); // Terminate the first child process
    }

    else if (child1_pid > 0){
        int pipe3[2], pipe4[2];
        pipe(pipe3); // Third arrow from left (P3 --> P1)
        pipe(pipe4); // Fourth arrow from left (P1 --> P3)

        child2_pid = fork(); // Create the second child process

        if(child2_pid == 0){ // Second child process
            close(pipe3[0]); // Close the reading end of the third pipe
            close(pipe4[1]); // Close the writing end of the fourth pipe

            int read_number = 0;
            while(read_number != -1){
                read(pipe4[0],&read_number,sizeof(read_number)); // Read the current number from the pipe4

                int is_prime = isPrime(read_number); // Check if the current number is prime or not
                write(pipe3[1], &is_prime, sizeof(is_prime)); // Write the result of the prime check to the pipe3
            }

            exit(0); // Terminate the second child process
        }

        else if (child2_pid > 0){ // Parent process
            close(pipe1[0]); // Close the reading end of the first pipe
            close(pipe2[1]); // Close the writing end of the second pipe
            close(pipe3[1]); // Close the writing end of the third pipe
            close(pipe4[0]); // Close the reading end of the fourth pipe

            int digit_count_array[1000]; //TODO: Make the length of the array dynamic
            int current_index_of_digit_count_array = 0;

            int is_prime_array[1000]; //TODO: Make the length of the array dynamic
            int current_index_of_is_prime_array = 0;

            FILE* fp;
            char* line = NULL;
            size_t len = 0;

            fp = fopen(argv[1], "r"); // Open the file in read mode

            while (getline(&line, &len, fp) != -1) {
                int number = atoi(line); // Convert the read line to an integer

                write(pipe1[1], &number, sizeof(number)); // Write the number to the pipe1
                //usleep(SLEEPDURATIONBETWEENWRITES); // Sleep for 2ms (In windows use Sleep(2))
                write(pipe4[1], &number, sizeof(number)); // Write the number to the pipe4

                int read_digit_count;
                read(pipe2[0], &read_digit_count, sizeof(read_digit_count)); // Read the number of digits of the current number from the pipe2
                digit_count_array[current_index_of_digit_count_array++] = read_digit_count; // Store the number of digits of the current number

                int read_is_prime;
                read(pipe3[0], &read_is_prime, sizeof(read_is_prime)); // Read the result of the prime check of the current number from the pipe3
                is_prime_array[current_index_of_is_prime_array++] = read_is_prime; // Store the result of the prime check of the current number
            }

            int stopping_number = -1;
            write(pipe1[1], &stopping_number, sizeof(stopping_number)); // Write the stopping number to the pipe1 to stop the infinite loop in the 1st child process
            write(pipe4[1], &stopping_number, sizeof(stopping_number)); // Write the stopping number to the pipe4 to stop the infinite loop in the 2nd child process

            waitpid(child1_pid, &child_status1, 0); // Wait for the first child process to terminate
            waitpid(child2_pid, &child_status2, 0); // Wait for the second child process to terminate

            printf("Input file: %s\n", argv[1]); // Print the name of the input file

            //TODO: Ensure that the size parameter is correct
            printDigitCountFromArr(digit_count_array, current_index_of_digit_count_array); // Print the number of digits of the numbers read from the file
            printIsPrimeFromArr(is_prime_array, current_index_of_is_prime_array); // Print the number of prime and non-prime numbers read from the file

            fclose(fp);
        }
    }

    return 0;
}

int getDigitCount(int number)
{
    int count = 0;
    do {
        number /= 10;
        ++count;
    } while (number != 0);

    return count;
}

int isPrime(int number)
{
    if (number <= 1)
        return 0;
    for (int i = 2; i < number; i++)
        if (number % i == 0)
            return 0;
    return 1;
}

int printDigitCountFromArr(int arr[], int size){
    int one_digit_count = 0;
    int two_digit_count = 0;
    int three_digit_count = 0;
    int four_digit_count = 0;
    int five_digit_count = 0;

    for(int i = 0; i < size; i++){
        if(arr[i] == 1)
            one_digit_count++;
        else if(arr[i] == 2)
            two_digit_count++;
        else if(arr[i] == 3)
            three_digit_count++;
        else if(arr[i] == 4)
            four_digit_count++;
        else if(arr[i] == 5)
            five_digit_count++;
    }

    printf("\n1 digits - %d\n", one_digit_count);
    printf("2 digits - %d\n", two_digit_count);
    printf("3 digits - %d\n", three_digit_count);
    printf("4 digits - %d\n", four_digit_count);
    printf("5 digits - %d", five_digit_count);
    return 1;
}

int printIsPrimeFromArr(int arr[], int size){
    int prime_count = 0;
    int non_prime_count = 0;

    for(int i = 0; i < size; i++){
        if(arr[i] == 1)
            prime_count++;
        else if(arr[i] == 0)
            non_prime_count++;
    }

    printf("\nPrimes - %d\n", prime_count);
    printf("Nonprimes - %d\n", non_prime_count);
    return 1;
}