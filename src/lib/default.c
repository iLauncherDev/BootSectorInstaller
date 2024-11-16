/*
 * PROJECT:     BootSectorInstaller
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Add helpers functions for default library
 * COPYRIGHT:   Copyright 2024 Daniel Victor <ilauncherdeveloper@gmail.com>
 */

#include <lib/default.h>

int64_t GetFileSize(char *path)
{
    struct stat statbuf;

    if (stat(path, &statbuf) == -1)
        return -1;

    if (S_ISREG(statbuf.st_mode))
    {
        FILE *file = fopen(path, "rb");
        if (file == NULL)
            return -1;

        if (fseek(file, 0, SEEK_END) != 0)
        {
            fclose(file);
            return -1;
        }

        uint64_t size = ftell(file);
        fclose(file);
        return size;
    }

    if (S_ISBLK(statbuf.st_mode))
    {
        int fd = open(path, O_RDONLY);
        if (fd == -1)
            return -1;

        uint64_t size;
        if (ioctl(fd, BLKGETSIZE64, &size) == -1)
        {
            close(fd);
            return -1;
        }

        close(fd);
        return size;
    }

    return -1;
}