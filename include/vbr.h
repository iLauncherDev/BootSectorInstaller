/*
 * PROJECT:     BootSectorInstaller
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Add public VBR macros, structures and functions declarations
 * COPYRIGHT:   Copyright 2024 Daniel Victor <ilauncherdeveloper@gmail.com>
 */

#pragma once
#include <lib/default.h>
#include <mbr.h>

#define VBR_SIZE_LIMIT (1024 * 8)
#define VBR_FILESYSTEM_BTRFS_STRING "BTRFS"
#define VBR_FILESYSTEM_EXT_STRING "EXT"
#define VBR_FILESYSTEM_FAT_STRING "FAT"
#define VBR_FILESYSTEM_NTFS_STRING "NTFS"
#define VBR_FILESYSTEM_BTRFS_SIZE (SECTOR_SIZE * 3)
#define VBR_FILESYSTEM_FAT_SIZE (SECTOR_SIZE)
#define VBR_FILESYSTEM_FAT32_SIZE (SECTOR_SIZE * 2)
#define VBR_FILESYSTEM_EXT_SIZE (SECTOR_SIZE * 4)
#define VBR_FILESYSTEM_NTFS_SIZE (SECTOR_SIZE * 4)
#define VBR_FILESYSTEM_TYPE_FAT32_STRING "FAT32   "
#define VBR_FILESYSTEM_SIGNATURE_EXT (0xEF53)
#define EXT_DEFAULT_INODE_SIZE (128)
#define EXT_DEFAULT_DESC_SIZE (32)
#define EXT3_FEATURE_INCOMPAT_RECOVER (0x0004)
#define EXT3_FEATURE_INCOMPAT_JOURNAL_DEV (0x0008)
#define EXT4_FEATURE_INCOMPAT_EXTENTS (0x0040)
#define EXT4_FEATURE_INCOMPAT_64BIT (0x0080)

typedef struct _VBR_INSTALLER
{
    uint16_t VBRSize;
    char *FileSystem;
    bool (*Install)(uint8_t PartitionNumber,
                    uint64_t *PartitionStartSector, uint64_t *PartitionEndSector,
                    void *Dest, void *Src);
} VBR_INSTALLER;

#pragma pack(push, 1)

typedef struct _BTRFS_VBR
{
    uint8_t JumpOpcode[3];
    uint8_t ChunkMapSize;
    uint8_t BootDrive;
    uint64_t PartitionStartLBA;
    uint8_t VBRCode[497];
    uint8_t VBRSignatureLow;
    uint8_t VBRSignatureHigh;
    uint8_t VBRCode2[1021];
    uint8_t BootPartition;
    uint8_t VBRSignatureLow2;
    uint8_t VBRSignatureHigh2;
} BTRFS_VBR;

static_assert(sizeof(BTRFS_VBR) == VBR_FILESYSTEM_BTRFS_SIZE, "The BTRFS VBR structure must be 1536 bytes");

typedef struct _EXT_VBR
{
    /* BootSector Information Part 1 */
    uint8_t JumpOpcode[3];
    uint8_t BootDrive;
    uint32_t ExtVolumeStartSector;
    uint32_t ExtBlockSize;
    uint32_t ExtBlockSizeInBytes;
    uint32_t ExtPointersPerBlock;
    uint32_t ExtGroupDescSize;
    uint32_t ExtFirstDataBlock;
    uint32_t ExtInodeSize;
    uint32_t ExtInodesPerGroup;
    uint8_t VBRCode[473];
    uint8_t BootPartition;
    uint8_t VBRSignatureLow;
    uint8_t VBRSignatureHigh;
    uint8_t VBRCode2[512];

    /* SuperBlock Information Ext2 */
    uint32_t InodesCount;
    uint32_t BlocksCountLo;
    uint32_t RBlocksCountLo;
    uint32_t FreeBlocksCountLo;
    uint32_t FreeInodesCount;
    uint32_t FirstDataBlock;
    uint32_t LogBlockSize;
    uint32_t LogFragSize;
    uint32_t BlocksPerGroup;
    uint32_t FragsPerGroup;
    uint32_t InodesPerGroup;
    uint32_t MTime;
    uint32_t WTime;
    uint16_t MntCount;
    uint16_t MaxMntCount;
    uint16_t Magic;
    uint16_t State;
    uint16_t Errors;
    uint16_t MinorRevLevel;
    uint32_t LastCheck;
    uint32_t CheckInterval;
    uint32_t CreatorOS;
    uint32_t RevLevel;
    uint16_t DefResUID;
    uint16_t DefResGID;

    /* SuperBlock Information Ext3 */
    uint32_t FirstIno;
    uint16_t InodeSize;
    uint16_t BlockGroupNr;
    uint32_t FeatureCompat;
    uint32_t FeatureIncompat;
    uint32_t FeatureROCompat;
    uint8_t UUID[16];
    uint8_t VolumeName[16];
    uint8_t LastMounted[64];
    uint32_t AlgorithmUsageBitmap;

    /* SuperBlock Information Ext4 */
    uint8_t PreallocBlocks;
    uint8_t PreallocDirBlocks;
    uint16_t ReservedGdtBlocks;
    uint8_t JournalUuid[16];
    uint32_t JournalInum;
    uint32_t JournalDev;
    uint32_t LastOrphan;
    uint32_t HashSeed[4];
    uint8_t DefHashVersion;
    uint8_t JournalBackupType;
    uint16_t GroupDescSize;
    uint8_t Reserved[768];
} EXT_VBR;

static_assert(sizeof(EXT_VBR) == VBR_FILESYSTEM_EXT_SIZE, "The EXT VBR structure must be 2048 bytes");

typedef struct _FAT_VBR
{
    uint8_t JumpOpcode[3];
    uint8_t OemName[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t NumberOfFats;
    uint16_t RootDirEntries;
    uint16_t TotalSectors;
    uint8_t MediaDescriptor;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t NumberOfHeads;
    uint32_t HiddenSectors;
    uint32_t TotalSectorsBig;
} FAT_VBR;

typedef struct _FAT16_VBR
{
    FAT_VBR Header;

    uint8_t BootDrive;
    uint8_t Reserved;
    uint8_t BootSignature;
    uint32_t VolumeSerialNumber;
    uint8_t VolumeLabel[11];
    uint8_t FileSystemType[8];
    uint8_t VBRCode[447];
    uint8_t BootPartition;
    uint8_t VBRSignatureLow;
    uint8_t VBRSignatureHigh;
} FAT16_VBR;

static_assert(sizeof(FAT16_VBR) == VBR_FILESYSTEM_FAT_SIZE, "The FAT16 VBR structure must be 512 bytes");

typedef struct _FAT32_VBR
{
    FAT_VBR Header;

    uint32_t SectorsPerFatBig;
    uint16_t ExtendedFlags;
    uint16_t FileSystemVersion;
    uint32_t RootDirStartCluster;
    uint16_t FsInfo;
    uint16_t BackupBootSector;
    uint8_t Reserved1[12];
    uint8_t BootDrive;
    uint8_t Reserved;
    uint8_t BootSignature;
    uint32_t VolumeSerialNumber;
    uint8_t VolumeLabel[11];
    uint8_t FileSystemType[8];
    uint8_t VBRCode[419];
    uint8_t BootPartition;
    uint8_t VBRSignatureLow;
    uint8_t VBRSignatureHigh;
    uint8_t VBRCode2[512];
} FAT32_VBR;

static_assert(sizeof(FAT32_VBR) == VBR_FILESYSTEM_FAT32_SIZE, "The FAT32 VBR structure must be 1024 bytes");

typedef struct _NTFS_VBR
{
    uint8_t JumpOpcode[3];
    uint8_t OemId[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t NumberOfFats;
    uint16_t MaxRootEntries;
    uint16_t TotalSectors;
    uint8_t MediaDescriptor;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t NumberOfHeads;
    uint32_t HiddenSectors;
    uint32_t TotalSectorsBig;
    uint8_t BootDrive;
    uint8_t CurrentHead;
    uint8_t BootSignature;
    uint8_t Unused;
    uint64_t VolumeSectorCount;
    uint64_t MftLocation;
    uint64_t MftMirrorLocation;
    uint32_t ClustersPerMftRecord;
    uint32_t ClustersPerIndexRecord;
    uint64_t VolumeSerialNumber;
    uint32_t Checksum;
    uint8_t VBRCode[425];
    uint8_t BootPartition;
    uint8_t VBRSignatureLow;
    uint8_t VBRSignatureHigh;
    uint8_t VBRCode2[1536];
} NTFS_VBR;

static_assert(sizeof(NTFS_VBR) == VBR_FILESYSTEM_NTFS_SIZE, "The NTFS VBR structure must be 2048 bytes");

#pragma pack(pop)

bool VbrFlash(char *Device, char *File, char *FileSystem,
              uint8_t PartitionNumber,
              uint64_t *PartitionStartSector, uint64_t *PartitionEndSector);
