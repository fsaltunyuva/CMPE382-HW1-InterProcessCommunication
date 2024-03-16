#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define FILELOCATION "/home/furkan/Desktop/CMPE382/HW1/numbers.txt"

//TODO: Make the length of the text file dynamic (Currently it is 1000)

int main()
{
    // Function prototypes
    int readFileWriteToPipe(int fd_p2, int fd_p3, int file_size);
    int nrDigits(int number);
    int isPrime(int number);

    int child_status1; // Status of the first child process
    int child_status2; // Status of the second child process

    int file_size = 0; // Size of the text file

    __pid_t parent_pid, child1_pid, child2_pid; // PIDs of the child processes

    // Create 4 pipes (pipe[0] is for reading, pipe[1] is for writing)
    int pipe1[2], pipe2[2], pipe3[2], pipe4[2];
    pipe(pipe1); // First arrow from left (P1 --> P2)
    pipe(pipe2); // Second arrow from left (P2 --> P1)
    pipe(pipe3); // Third arrow from left (P3 --> P1)
    pipe(pipe4); // Fourth arrow from left (P1 --> P3)

    printf("Original pid is %d\n", getpid());
    parent_pid = getpid();

    child1_pid = fork(); // Create the first child process

    if (child1_pid == 0){ // First child process (P2)
        printf("This is the 1st child process, pid = %d,"
               " parent pid = %d\n", getpid(), getppid());

        close(pipe1[1]); // Close the writing end of the first pipe
        close(pipe2[0]); // Close the reading end of the second pipe

        // Read the numbers from the pipe1
        char read_numbers_from_p1_string[1000][32]; // String array for the numbers read from the pipe1
        read(pipe1[0], read_numbers_from_p1_string, sizeof(read_numbers_from_p1_string));

        // int read_numbers_from_p1[1000]; // Integer array for the numbers read from the pipe1

        int one_digit_numbers = 0;
        int two_digit_numbers = 0;
        int three_digit_numbers = 0;
        int four_digit_numbers = 0;
        int five_digit_numbers = 0;

        for(int i = 0; i < 1000; i++){
            //read_numbers_from_p1[i] = atoi(read_numbers_from_p1_string[i]);

            // Count the number of digits of the numbers
            int digit_count = nrDigits(atoi(read_numbers_from_p1_string[i]));

            if(digit_count == 1){
                one_digit_numbers++;
            }
            else if(digit_count == 2){
                two_digit_numbers++;
            }
            else if(digit_count == 3){
                three_digit_numbers++;
            }
            else if(digit_count == 4){
                four_digit_numbers++;
            }
            else if(digit_count == 5){
                five_digit_numbers++;
            }
        }

        printf("\nOne digit numbers: %d\n", one_digit_numbers);
        printf("Two digit numbers: %d\n", two_digit_numbers);
        printf("Three digit numbers: %d\n", three_digit_numbers);
        printf("Four digit numbers: %d\n", four_digit_numbers);
        printf("Five digit numbers: %d\n", five_digit_numbers);

        // Convert the numbers to strings to send to the parent from the pipe in string format
        char one_digit_numbers_string[12];
        char two_digit_numbers_string[12];
        char three_digit_numbers_string[12];
        char four_digit_numbers_string[12];
        char five_digit_numbers_string[12];

        sprintf(one_digit_numbers_string, "%d", one_digit_numbers);
        sprintf(two_digit_numbers_string, "%d", two_digit_numbers);
        sprintf(three_digit_numbers_string, "%d", three_digit_numbers);
        sprintf(four_digit_numbers_string, "%d", four_digit_numbers);
        sprintf(five_digit_numbers_string, "%d", five_digit_numbers);

        char data_to_send_to_parent_from_P2[100]; // Data to be sent to the parent from P2

        // Format of the data to be sent to the parent from P2: one_digit_numbers-two_digit_numbers-three_digit_numbers-four_digit_numbers-five_digit_numbers
        strcat(data_to_send_to_parent_from_P2, one_digit_numbers_string);
        strcat(data_to_send_to_parent_from_P2, "-");
        strcat(data_to_send_to_parent_from_P2, two_digit_numbers_string);
        strcat(data_to_send_to_parent_from_P2, "-");
        strcat(data_to_send_to_parent_from_P2, three_digit_numbers_string);
        strcat(data_to_send_to_parent_from_P2, "-");
        strcat(data_to_send_to_parent_from_P2, four_digit_numbers_string);
        strcat(data_to_send_to_parent_from_P2, "-");
        strcat(data_to_send_to_parent_from_P2, five_digit_numbers_string);

        //TODO: Write the digit numbers to pipe2

        exit(0);
    }

    else if (child1_pid > 0){
        child2_pid = fork(); // Create the second child process

        if(child2_pid == 0){ // Second child process
            printf("This is the 2nd child process, pid = %d,"
                   " parent pid = %d\n", getpid(), getppid());

            close(pipe3[0]); // Close the reading end of the third pipe
            close(pipe4[1]); // Close the writing end of the fourth pipe

            //TODO: Read the numbers from the pipe4
            //TODO: Write the count of prime numbers to pipe3

            exit(0);
        }

        else if (child2_pid > 0){ // Parent process
            printf("This is the parent process, pid = %d\n", getpid());

            close(pipe1[0]); // Close the reading end of the first pipe
            close(pipe2[1]); // Close the reading end of the second pipe
            close(pipe3[1]); // Close the writing end of the third pipe
            close(pipe4[0]); // Close the reading end of the fourth pipe

            readFileWriteToPipe(pipe1[1], pipe4[1], file_size);

            waitpid(child1_pid, &child_status1, 0);
            waitpid(child2_pid, &child_status2, 0);
        }
    }

    return 0;
}

int nrDigits(int number)
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

int readFileWriteToPipe(int fd_p2, int fd_p3, int file_size) { // First arrow from left and First arrow from right (The arrows that the parent writes to)
    FILE * fp;

    //fp = fopen("/home/furkan/Desktop/CMPE382/HW1/numbers.txt", "r");
    fp = fopen(FILELOCATION, "r");

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    char buffer[32]; //Buffer for reading the file
    char* file_content = (char*)malloc(file_size * sizeof(char)); //Content of the file

    printf("File size: %d\n", file_size);
    char numbers[1000][32];
    int i = 0;

    while (fgets(buffer, 32, fp) != NULL) {
        strcpy(numbers[i++], buffer); //Copy the buffer to the numbers array (numbers[i++] = buffer;
        strcat(file_content, buffer); //Concatenate the buffer to the file content
    }

    write(fd_p2, numbers, sizeof(numbers)); //Write the numbers to the pipe2
    write(fd_p3, numbers, sizeof(numbers)); //Write the numbers to the pipe3

    // WRITING NUMBERS LINE BY LINE CAUSES PIPE OVERFLOW
    // HOW TO WRITE ONE BY ONE TO THE PIPE?

    fclose(fp);
    return 1; // Return 1 if the file is read and written to the pipes successfully
}
