/*
 * PROJECT:     BootSectorInstaller
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     The program entry point
 * COPYRIGHT:   Copyright 2024 Daniel Victor <ilauncherdeveloper@gmail.com>
 */

#include <lib/default.h>
#include <mbr.h>
#include <vbr.h>

bool InvertedFlashDirection;

char *MBRDevice = NULL;
char *MBRFile = NULL;

char *VBRDevice = NULL;
char *VBRFile = NULL;
char *VBRFileSystem = NULL;
uint8_t VBRPartitionNumber = 0;
uint64_t VBRPartitionStartSector = 0;
uint64_t VBRPartitionEndSector = 0;

int main(int argc, char **argv)
{
    int argStep = 1;

    for (int i = 1; i < argc; i += argStep)
    {
        argStep = 1;

        char **arg = &argv[i];

        if (!strcasecmp(arg[0], MBRDEV_ARGUMENT_STRING))
        {
            argStep = MBRDEV_ARGUMENT_STRING_MINARGS;

            if (argc - i < argStep)
            {
                printf(DEBUG_STRING STR_000000);
                goto error;
            }

            MBRDevice = arg[1];
            MBRFile = arg[2];

            if (!strcasecmp(MBRFile, "null"))
                MBRFile = NULL;
        }
        else if (!strcasecmp(arg[0], VBRDEV_ARGUMENT_STRING))
        {
            argStep = VBRDEV_ARGUMENT_STRING_MINARGS;

            if (argc - i < argStep)
            {
                printf(DEBUG_STRING STR_000000);
                goto error;
            }

            VBRDevice = arg[1];
            VBRFile = arg[2];
            VBRFileSystem = arg[3];
            VBRPartitionNumber = ((uint8_t)atoi(arg[4]) - 1) % 4;
        }
        else if (!strcasecmp(arg[0], EXPORT_ARGUMENT_STRING))
        {
            InvertedFlashDirection = true;
        }
    }

    if (!MBRDevice && !MBRFile && !VBRDevice && !VBRFile)
    {
        printf(STR_000001);
        printf(STR_000013 STR_000014, *argv, *argv);
        goto error;
    }

    MbrFlash(MBRDevice, MBRFile, VBRPartitionNumber, &VBRPartitionStartSector, &VBRPartitionEndSector);
    VbrFlash(VBRDevice, VBRFile, VBRFileSystem, VBRPartitionNumber, &VBRPartitionStartSector, &VBRPartitionEndSector);

    return 0;

error:
    return 1;
}