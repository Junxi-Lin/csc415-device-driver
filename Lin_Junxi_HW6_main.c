/**************************************************************
 * Class: CSC-415-02 Spring 2026
 * Name: Junxi Lin
 * Student ID: 923696927
 * GitHub Name: Junxi-Lin
 * Project: Assignment 6 - Device Driver
 *
 * File: Lin_Junxi_HW6_main.c
 *
 * Description:
 *   Interactive user test program for /dev/secret. It allow the
 *   user to enter text, to set a Caesar cipher key, and choose
 *   encryption or decryption using ioctl.
 *
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/secret"

#define MODE_ENCRYPT 'e'
#define MODE_DECRYPT 'd'
#define SET_KEY      'k'

#define BUFFER_SIZE 512

int main(void)
{
    int fd;
    int choice;
    int key;
    char input[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    /* Open the device file use /dev/secret */
    fd = open(DEVICE, O_RDWR);
    if (fd < 0)
    {
        perror("Cannot open /dev/secret");
        return 1;
    }

    printf("Secret Device Driver Test Program\n");
    printf("---------------------------------\n");

    /* Get input string from user */
    printf("Enter text: ");
    fgets(input, BUFFER_SIZE, stdin);

    /* Remove newline character from input */
    input[strcspn(input, "\n")] = '\0';

    /* Get encryption key from user */
    printf("Enter Caesar cipher key: ");
    scanf("%d", &key);

    printf("\nChoose an option:\n");
    printf("1. Encrypt\n");
    printf("2. Decrypt\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    /* Send user input to the device driver */
    write(fd, input, strlen(input));

    /* Set encryption key in driver use ioctl */
    ioctl(fd, SET_KEY, key);

    if (choice == 1)
    {
        ioctl(fd, MODE_ENCRYPT);
    }
    else if (choice == 2)
    {
        ioctl(fd, MODE_DECRYPT);
    }
    else
    {
        printf("Invalid choice.\n");
        close(fd);
        return 1;
    }

    /* Clear buffer and read result from driver */
    memset(buffer, 0, BUFFER_SIZE);
    read(fd, buffer, BUFFER_SIZE);

    printf("\nResult: %s\n", buffer);

    close(fd);

    return 0;
}
