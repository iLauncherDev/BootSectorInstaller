/*
 * PROJECT:     BootSectorInstaller
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Add public MBR macros, structures and functions declarations
 * COPYRIGHT:   Copyright 2024 Daniel Victor <ilauncherdeveloper@gmail.com>
 */

#pragma once
#include <lib/default.h>

#define MBR_SIGNATURE_LOW (0x55)
#define MBR_SIGNATURE_HIGH (0xAA)
#define MBR_SIZE (sizeof(MBR))

#pragma pack(push, 1)

typedef struct _MBR_PTE
{
    uint8_t Attributes;
    uint8_t CHSAddressStart[3];
    uint8_t PartitionType;
    uint8_t CHSAddressEnd[3];
    uint32_t LBAStartAddress;
    uint32_t PartitionSectors;
} MBR_PTE;

typedef struct _MBR
{
    uint8_t MBRCode[442];
    uint32_t DiskSignature;
    MBR_PTE PTE[4];
    uint8_t MBRSignatureLow;
    uint8_t MBRSignatureHigh;
} MBR;

#pragma pack(pop)

bool MbrFlash(char *Device, char *File, uint8_t PartitionNumber, uint64_t *PartitionStartSector, uint64_t *PartitionEndSector);
