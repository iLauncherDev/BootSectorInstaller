/*
 * PROJECT:     BootSectorInstaller
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Helper code to interact with VBR
 * COPYRIGHT:   Copyright 2024 Daniel Victor <ilauncherdeveloper@gmail.com>
 */

#include <vbr.h>

bool BtrfsInstall(uint8_t PartitionNumber,
                  uint64_t *PartitionStartSector, uint64_t *PartitionEndSector,
                  void *Dest, void *Src);
bool ExtInstall(uint8_t PartitionNumber,
                uint64_t *PartitionStartSector, uint64_t *PartitionEndSector,
                void *Dest, void *Src);
bool FatInstall(uint8_t PartitionNumber,
                uint64_t *PartitionStartSector, uint64_t *PartitionEndSector,
                void *Dest, void *Src);
bool NtfsInstall(uint8_t PartitionNumber,
                 uint64_t *PartitionStartSector, uint64_t *PartitionEndSector,
                 void *Dest, void *Src);

VBR_INSTALLER VBR_Installers[] = {
    {
        .VBRSize = VBR_FILESYSTEM_BTRFS_SIZE,
        .FileSystem = VBR_FILESYSTEM_BTRFS_STRING,
        .Install = BtrfsInstall,
    },
    {
        .VBRSize = VBR_FILESYSTEM_EXT_SIZE,
        .FileSystem = VBR_FILESYSTEM_EXT_STRING,
        .Install = ExtInstall,
    },
    {
        .VBRSize = VBR_FILESYSTEM_FAT_SIZE,
        .FileSystem = VBR_FILESYSTEM_FAT_STRING,
        .Install = FatInstall,
    },
    {
        .VBRSize = VBR_FILESYSTEM_NTFS_SIZE,
        .FileSystem = VBR_FILESYSTEM_NTFS_STRING,
        .Install = NtfsInstall,
    },
};

void *VbrReadFile(char *File, uint16_t FileSize)
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
    if (size == -1)
    {
        printf(DEBUG_STRING STR_000004);
        fclose(Bin);
        return false;
    }

    void *Buffer = malloc(FileSize);

    if (!Buffer)
    {
        printf(DEBUG_STRING STR_000004);
        fclose(Bin);
        return NULL;
    }

    memset(Buffer, 0, FileSize);

    size = size > FileSize ? FileSize : size;

    size_t count = fread(Buffer, 1, (size_t)size, Bin);

    if (count < (size_t)size)
    {
        printf(DEBUG_STRING STR_000004);
        free(Buffer);
        fclose(Bin);
        return NULL;
    }

    fclose(Bin);

    return Buffer;
}

bool VbrWriteFile(char *File, void *Buffer, uint16_t FileSize)
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
        size = FileSize;

    if (size == -1)
    {
        printf(DEBUG_STRING STR_000004);
        fclose(Bin);
        return false;
    }

    size = size > FileSize ? FileSize : size;

    size_t count = fwrite(Buffer, 1, (size_t)size, Bin);

    if (count < (size_t)size)
    {
        printf(DEBUG_STRING STR_000011);
        fclose(Bin);
        return false;
    }

    fclose(Bin);

    return true;
}

bool VbrFlash(char *Device, char *File, char *FileSystem,
              uint8_t PartitionNumber,
              uint64_t *PartitionStartSector, uint64_t *PartitionEndSector)
{
    uint16_t VBRSize = VBR_SIZE_LIMIT;
    bool Result = false;
    void *Dest = VbrReadFile(Device, VBRSize), *Src = VbrReadFile(File, VBRSize);

    if (!Dest || (!Src && !InvertedFlashDirection))
    {
        printf(DEBUG_STRING STR_000007);
        free(Dest);
        free(Src);
        return Result;
    }

    if (!FileSystem)
        goto ErrInvalidFS;

    for (size_t i = 0; i < sizeof(VBR_Installers) / sizeof(VBR_INSTALLER); i++)
    {
        VBR_INSTALLER *VBR_Installer = &VBR_Installers[i];

        if (!strcasecmp(VBR_Installer->FileSystem, FileSystem))
        {
            VBRSize = VBR_Installer->VBRSize;

            if (InvertedFlashDirection)
            {
                Result = true;
                break;
            }

            if (VBR_Installer->Install)
                Result = VBR_Installer->Install(PartitionNumber + 1,
                                                PartitionStartSector, PartitionEndSector,
                                                Dest, Src);

            break;
        }
    }

    if (Result)
        VbrWriteFile(InvertedFlashDirection ? File : Device, Dest, VBRSize);

    free(Dest);
    free(Src);
    return Result;

ErrInvalidFS:
    printf(DEBUG_STRING STR_000012);
    free(Dest);
    free(Src);
    return Result;
}

bool IsFat32(void *Buffer)
{
    FAT32_VBR *Fat32VBR = Buffer;

    if (strncasecmp((char *)Fat32VBR->FileSystemType,
                    VBR_FILESYSTEM_TYPE_FAT32_STRING,
                    sizeof(Fat32VBR->FileSystemType)))
        return false;

    return true;
}

bool IsExt3Plus(void *Buffer)
{
    EXT_VBR *ExtVBR = Buffer;

    if (ExtVBR->FeatureIncompat & EXT3_FEATURE_INCOMPAT_RECOVER ||
        ExtVBR->FeatureIncompat & EXT3_FEATURE_INCOMPAT_JOURNAL_DEV)
        return true;

    if (ExtVBR->FeatureIncompat & EXT4_FEATURE_INCOMPAT_EXTENTS ||
        ExtVBR->FeatureIncompat & EXT4_FEATURE_INCOMPAT_64BIT)
        return true;

    if (ExtVBR->JournalUuid[0] != 0)
        return true;

    if (ExtVBR->RevLevel > 0)
        return true;

    return false;
}

bool BtrfsInstall(uint8_t PartitionNumber,
                  uint64_t *PartitionStartSector, uint64_t *PartitionEndSector,
                  void *Dest, void *Src)
{
    BTRFS_VBR *DestVBR = Dest, *SrcVBR = Src;

    if (SrcVBR->VBRSignatureLow != MBR_SIGNATURE_LOW ||
        SrcVBR->VBRSignatureHigh != MBR_SIGNATURE_HIGH)
    {
        printf(DEBUG_STRING STR_000009);
        return false;
    }

    memcpy(DestVBR->JumpOpcode, SrcVBR->JumpOpcode, sizeof(SrcVBR->JumpOpcode));
    memcpy(DestVBR->VBRCode, SrcVBR->VBRCode, sizeof(SrcVBR->VBRCode));
    memcpy(DestVBR->VBRCode2, SrcVBR->VBRCode2, sizeof(SrcVBR->VBRCode2));

    DestVBR->BootDrive = 0xFF;
    DestVBR->BootPartition = PartitionNumber;

    DestVBR->VBRSignatureLow = SrcVBR->VBRSignatureLow;
    DestVBR->VBRSignatureLow2 = SrcVBR->VBRSignatureLow2;
    DestVBR->VBRSignatureHigh = SrcVBR->VBRSignatureHigh;
    DestVBR->VBRSignatureHigh2 = SrcVBR->VBRSignatureHigh2;

    return true;
}

bool ExtInstall(uint8_t PartitionNumber,
                uint64_t *PartitionStartSector, uint64_t *PartitionEndSector,
                void *Dest, void *Src)
{
    EXT_VBR *DestVBR = Dest, *SrcVBR = Src;

    bool IsDestExt3Plus = IsExt3Plus(Dest);

    uint32_t ExtInodeSize = IsDestExt3Plus ? DestVBR->InodeSize : EXT_DEFAULT_INODE_SIZE;
    uint32_t ExtGroupDescSize = (IsDestExt3Plus && DestVBR->GroupDescSize)
                                    ? DestVBR->GroupDescSize
                                    : EXT_DEFAULT_DESC_SIZE;

    uint32_t ExtVolumeStartSector = (uint32_t)(*PartitionStartSector);
    uint32_t ExtBlockSize = (1024 << DestVBR->LogBlockSize) / SECTOR_SIZE;
    uint32_t ExtBlockSizeInBytes = ExtBlockSize * SECTOR_SIZE;
    uint32_t ExtPointersPerBlock = ExtBlockSizeInBytes / sizeof(uint32_t);
    uint32_t ExtFirstDataBlock = DestVBR->FirstDataBlock + 1;
    uint32_t ExtInodesPerGroup = DestVBR->InodesPerGroup;

    if (SrcVBR->VBRSignatureLow != MBR_SIGNATURE_LOW ||
        SrcVBR->VBRSignatureHigh != MBR_SIGNATURE_HIGH)
    {
        printf(DEBUG_STRING STR_000009);
        return false;
    }

    DestVBR->ExtVolumeStartSector = ExtVolumeStartSector;
    DestVBR->ExtBlockSize = ExtBlockSize;
    DestVBR->ExtBlockSizeInBytes = ExtBlockSizeInBytes;
    DestVBR->ExtPointersPerBlock = ExtPointersPerBlock;
    DestVBR->ExtGroupDescSize = ExtGroupDescSize;
    DestVBR->ExtFirstDataBlock = ExtFirstDataBlock;
    DestVBR->ExtInodeSize = ExtInodeSize;
    DestVBR->ExtInodesPerGroup = ExtInodesPerGroup;

    printf(DEBUG_STRING "%s = 0x%x;\n", "DestVBR->ExtVolumeStartSector", DestVBR->ExtVolumeStartSector);
    printf(DEBUG_STRING "%s = 0x%x;\n", "DestVBR->ExtBlockSize", DestVBR->ExtBlockSize);
    printf(DEBUG_STRING "%s = 0x%x;\n", "DestVBR->ExtBlockSizeInBytes", DestVBR->ExtBlockSizeInBytes);
    printf(DEBUG_STRING "%s = 0x%x;\n", "DestVBR->ExtPointersPerBlock", DestVBR->ExtPointersPerBlock);
    printf(DEBUG_STRING "%s = 0x%x;\n", "DestVBR->ExtGroupDescSize", DestVBR->ExtGroupDescSize);
    printf(DEBUG_STRING "%s = 0x%x;\n", "DestVBR->ExtFirstDataBlock", DestVBR->ExtFirstDataBlock);
    printf(DEBUG_STRING "%s = 0x%x;\n", "DestVBR->ExtInodeSize", DestVBR->ExtInodeSize);
    printf(DEBUG_STRING "%s = 0x%x;\n", "DestVBR->ExtInodesPerGroup", DestVBR->ExtInodesPerGroup);

    memcpy(DestVBR->JumpOpcode, SrcVBR->JumpOpcode, sizeof(SrcVBR->JumpOpcode));
    memcpy(DestVBR->VBRCode, SrcVBR->VBRCode, sizeof(SrcVBR->VBRCode));
    memcpy(DestVBR->VBRCode2, SrcVBR->VBRCode2, sizeof(SrcVBR->VBRCode2));

    DestVBR->BootDrive = 0xFF;
    DestVBR->BootPartition = PartitionNumber;

    DestVBR->VBRSignatureLow = SrcVBR->VBRSignatureLow;
    DestVBR->VBRSignatureHigh = SrcVBR->VBRSignatureHigh;

    return true;
}

bool FatInstall(uint8_t PartitionNumber,
                uint64_t *PartitionStartSector, uint64_t *PartitionEndSector,
                void *Dest, void *Src)
{
    FAT_VBR *DestVBRHeader = Dest, *SrcVBRHeader = Src;
    bool IsDest32 = IsFat32(Dest), IsSrc32 = IsFat32(Src);

    if (IsDest32 != IsSrc32)
    {
        printf(DEBUG_STRING STR_000015);
        return false;
    }

    if (*(uint8_t *)(Src + (VBR_FILESYSTEM_FAT_SIZE - 2)) != MBR_SIGNATURE_LOW ||
        *(uint8_t *)(Src + (VBR_FILESYSTEM_FAT_SIZE - 1)) != MBR_SIGNATURE_HIGH)
    {
        printf(DEBUG_STRING STR_000009);
        return false;
    }

    memcpy(DestVBRHeader->JumpOpcode, SrcVBRHeader->JumpOpcode, sizeof(SrcVBRHeader->JumpOpcode));
    memcpy(DestVBRHeader->OemName, SrcVBRHeader->OemName, sizeof(SrcVBRHeader->OemName));

    if (IsDest32)
    {
        FAT32_VBR *DestVBR = Dest, *SrcVBR = Src;

        memcpy(DestVBR->VBRCode, SrcVBR->VBRCode, sizeof(SrcVBR->VBRCode));
        memcpy(DestVBR->VBRCode2, SrcVBR->VBRCode2, sizeof(SrcVBR->VBRCode2));

        DestVBR->BootDrive = 0xFF;
        DestVBR->BootPartition = PartitionNumber;
        DestVBR->VBRSignatureLow = SrcVBR->VBRSignatureLow;
        DestVBR->VBRSignatureHigh = SrcVBR->VBRSignatureHigh;
    }
    else
    {
        FAT16_VBR *DestVBR = Dest, *SrcVBR = Src;

        memcpy(DestVBR->VBRCode, SrcVBR->VBRCode, sizeof(SrcVBR->VBRCode));

        DestVBR->BootDrive = 0xFF;
        DestVBR->BootPartition = PartitionNumber;
        DestVBR->VBRSignatureLow = SrcVBR->VBRSignatureLow;
        DestVBR->VBRSignatureHigh = SrcVBR->VBRSignatureHigh;
    }

    return true;
}

bool NtfsInstall(uint8_t PartitionNumber,
                 uint64_t *PartitionStartSector, uint64_t *PartitionEndSector,
                 void *Dest, void *Src)
{
    NTFS_VBR *DestVBR = Dest, *SrcVBR = Src;

    if (SrcVBR->VBRSignatureLow != MBR_SIGNATURE_LOW ||
        SrcVBR->VBRSignatureHigh != MBR_SIGNATURE_HIGH)
    {
        printf(DEBUG_STRING STR_000009);
        return false;
    }

    memcpy(DestVBR->JumpOpcode, SrcVBR->JumpOpcode, sizeof(SrcVBR->JumpOpcode));
    memcpy(DestVBR->VBRCode, SrcVBR->VBRCode, sizeof(SrcVBR->VBRCode));
    memcpy(DestVBR->VBRCode2, SrcVBR->VBRCode2, sizeof(SrcVBR->VBRCode2));

    DestVBR->BootDrive = 0xFF;
    DestVBR->BootPartition = PartitionNumber;

    DestVBR->VBRSignatureLow = SrcVBR->VBRSignatureLow;
    DestVBR->VBRSignatureHigh = SrcVBR->VBRSignatureHigh;

    return true;
}
