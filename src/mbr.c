/*
 * PROJECT:     BootSectorInstaller
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Helper code to interact with MBR
 * COPYRIGHT:   Copyright 2024 Daniel Victor <ilauncherdeveloper@gmail.com>
 */

#include <mbr.h>

void *MbrReadFile(char *File)
{
    if (!File)
    {
        printf(DEBUG_STRING STR_000006);
        return NULL;
    }

    FILE *Bin = fopen(File, "rb");
    if (!Bin)
    {
        printf(DEBUG_STRING STR_000002, File);
        return NULL;
    }

    int64_t size = GetFileSize(File);

    if (size < MBR_SIZE)
    {
        printf(DEBUG_STRING STR_000004);
        fclose(Bin);
        return NULL;
    }

    void *Buffer = malloc(MBR_SIZE);

    if (!Buffer)
    {
        printf(DEBUG_STRING STR_000004);
        fclose(Bin);
        return NULL;
    }

    size_t count = fread(Buffer, 1, MBR_SIZE, Bin);

    if (count != MBR_SIZE)
    {
        printf(DEBUG_STRING STR_000004);
        free(Buffer);
        fclose(Bin);
        return NULL;
    }

    fclose(Bin);

    return Buffer;
}

bool MbrWriteFile(char *File, void *Buffer)
{
    if (!File)
    {
        printf(DEBUG_STRING STR_000006);
        return false;
    }

    if (!Buffer)
    {
        printf(DEBUG_STRING STR_000005);
        return false;
    }

    FILE *Bin = fopen(File, "wb");
    if (!Bin)
    {
        printf(DEBUG_STRING STR_000002, File);
        return false;
    }

    int64_t size = GetFileSize(File);

    if (!size)
        size = MBR_SIZE;

    if (size < MBR_SIZE)
    {
        printf(DEBUG_STRING STR_000010);
        fclose(Bin);
        return false;
    }

    size_t count = fwrite(Buffer, 1, MBR_SIZE, Bin);

    if (count < MBR_SIZE)
    {
        printf(DEBUG_STRING STR_000010);
        fclose(Bin);
        return false;
    }

    fclose(Bin);

    return true;
}

bool MbrFlash(char *Device, char *File,
              uint8_t PartitionNumber,
              uint64_t *PartitionStartSector, uint64_t *PartitionEndSector)
{
    MBR *Dest = MbrReadFile(Device), *Src = MbrReadFile(File);

    if (Dest)
    {
        MBR_PTE *PTE = &Dest->PTE[PartitionNumber];

        *PartitionStartSector = PTE->LBAStartAddress;
        *PartitionEndSector = *PartitionStartSector + PTE->PartitionSectors;
    }

    if (!Dest || (!Src && !InvertedFlashDirection))
    {
        printf(DEBUG_STRING STR_000007);
        free(Dest);
        free(Src);
        return false;
    }

    if (!InvertedFlashDirection)
    {
        if (Src->MBRSignatureLow != MBR_SIGNATURE_LOW || Src->MBRSignatureHigh != MBR_SIGNATURE_HIGH)
        {
            printf(DEBUG_STRING STR_000008);
            free(Dest);
            free(Src);
            return false;
        }

        memcpy(Dest->MBRCode, Src->MBRCode, sizeof(Src->MBRCode));
        Dest->MBRSignatureLow = Src->MBRSignatureLow;
        Dest->MBRSignatureHigh = Src->MBRSignatureHigh;
    }

    bool FlashStatus = MbrWriteFile(InvertedFlashDirection ? File : Device, Dest);
    if (!FlashStatus)
    {
        printf(DEBUG_STRING STR_000010);
        free(Dest);
        free(Src);
        return false;
    }

    free(Dest);
    free(Src);
    return true;
}
