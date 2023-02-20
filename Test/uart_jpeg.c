#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int fd;
    unsigned char buffer[100000];
    unsigned char tempBuff[1];
    // Open the UART device
    fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        perror("Error opening UART device");
        return -1;
    }

    // Configure the UART settings
    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    tcsetattr(fd, TCSANOW, &options);

    // Read a single byte from the UART device

    int fileCount = 1;

    while (1)
    {

        if (read(fd, tempBuff, 1) != 1)
        {
            continue;
        }
        if (tempBuff[0] != (unsigned char)255)
        {
            continue;
        }
        if (read(fd, tempBuff, 1) != 1)
        {
            continue;
        }
        if (tempBuff[0] != (unsigned char)216)
        {
            continue;
        }

        unsigned int currentIndex = 2;
        buffer[0] = 255;
        buffer[1] = 216;
        while (1)
        {

            if (read(fd, tempBuff, 1) != 1)
            {
                continue;
            }
            buffer[currentIndex++] = tempBuff[0];
            if (tempBuff[0] != 0xFF)
            {
                continue;
            }
            if (read(fd, tempBuff, 1) != 1)
            {
                continue;
            }
            buffer[currentIndex++] = tempBuff[0];
            if (tempBuff[0] == 0xD8)
            {
                printf("Corrupted File, start over...\n");
                memset(buffer, 0x00, sizeof(buffer));
                currentIndex = 2;
                buffer[0] = 0xFF;
                buffer[1] = 0xD8;
                continue;
            }
            if (tempBuff[0] == 0xD9)
            {
                // END FOUND
                char fileName[11] = {'0', '0', '0', '0', '0', '.', 'j', 'p', 'e', 'g', '\0'};
                char secondDigit = (fileCount % 10) + '0';
                char firstDigit = (fileCount / 10) + '0';
                fileName[0] = firstDigit;
                fileName[1] = secondDigit;

                FILE *fp = fopen(fileName, "wb");
                if (fp == NULL)
                {
                    printf("Error opening file!\n");
                    return 1;
                }

                for (unsigned int i = 0; i < currentIndex; i++)
                {
                    unsigned char myChar = buffer[i];
                    fwrite(&myChar, sizeof(unsigned char), 1, fp);
                }

                // Close the file
                fclose(fp);
                printf("File %s saved!\n", fileName);
                memset(buffer, 0x00, sizeof(buffer));
                fileCount++;
                break;
            }
        }
    }

    // Close the UART device
    close(fd);

    return 0;
}