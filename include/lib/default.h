/*
 * PROJECT:     BootSectorInstaller
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     The default library that is used almost everywhere
 * COPYRIGHT:   Copyright 2024 Daniel Victor <ilauncherdeveloper@gmail.com>
 */

#pragma once
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

#define SECTOR_SIZE 512

#define _STR(x) _VAL(x)
#define _VAL(x) #x
#define DEBUG_STRING "(" __RELATIVE_FILE__ ":" _STR(__LINE__) "): "

#define MBRDEV_ARGUMENT_STRING "-MBR"
#define MBRDEV_ARGUMENT_STRING_MINARGS 3
#define VBRDEV_ARGUMENT_STRING "-VBR"
#define VBRDEV_ARGUMENT_STRING_MINARGS 5
#define EXPORT_ARGUMENT_STRING "-EXPORT"

#include <lang/en.h>

extern bool InvertedFlashDirection;

int64_t GetFileSize(char *path);
