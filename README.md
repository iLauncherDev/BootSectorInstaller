# BootSector Installer

This project provides an installer for MBR (Master Boot Record) and VBR (Volume Boot Record) to prepare storage devices with bootable partitions. The installer currently supports FAT/12/16/32, EXT, BTRFS, and NTFS file systems, allowing for flexible boot configuration on a wide range of systems.

## ⚠️ **WARNING**

The bootsector installation process can fail and potentially corrupt your MBR or VBR. Please ensure you have a **backup of your disk or partition** before proceeding.

<span style="color:red;">**By using this installer, you accept the risk of potential data loss.**</span>

### Recommendations:
- Test the installer in a **virtual machine** or a **non-critical environment** first to avoid data loss.
- Use tools like `dd` or disk imaging software to create a full backup of your disk or partition.
- Be cautious when running the installer, especially on production systems, and double-check the target device and partition before flashing.
- **Ensure compatibility** with your filesystem (FAT, EXT, BTRFS, NTFS) before proceeding with installation.

## Features

- **MBR Installation**: Installs the Master Boot Record to initialize the boot process for the system.
- **VBR Installation**: Installs Volume Boot Records for different file systems, including FAT, EXT, BTRFS, and NTFS.

## Supported File Systems

- **FAT** (FAT12, FAT16, FAT32)
- **EXT** (EXT2, EXT3, EXT4)
- **BTRFS**
- **NTFS**

## Installation

1. Clone or download this repository to your system.
2. Compile the bootsector installer source code using your preferred build tools.
3. Run the installer on your target device to install the MBR and VBR.

```bash
git clone https://github.com/iLauncherDev/BootSectorInstaller.git
cd bootsector-installer
./configure.sh -build
```

## Command Examples

**Note**: Flags are case-insensitive (e.g., -MBR is the same as -mbr), but paths may vary depending on the underlying operating system, file system and subsystem.

### Flashing the MBR

To flash the MBR to a specific drive:

```bash
./output/bootsector-installer -MBR /dev/[Drive] myMbr.bin
```
- `-MBR`: Specifies the MBR file to flash.
- `/dev/[Drive]`: The target drive where the MBR will be installed.
- `myMbr.bin`: The MBR binary file.

### Exporting the MBR

To export the MBR to a specific drive:

```bash
./output/bootsector-installer -EXPORT -MBR /dev/[Drive] myMbr.bin
```
- `-EXPORT`: Tells the installer to export the MBR to file without flashing it to the disk.
- `-MBR`: Specifies the MBR file to export.
- `/dev/[Drive]`: The drive from which the MBR and VBR will be exported.
- `myMbr.bin`: The output file for the exported MBR.

### Flashing the VBR

To flash the VBR to a specific partition:

```bash
./output/bootsector-installer -MBR /dev/[Drive] NULL -VBR /dev/[Drive][Partition] myVBR.bin FAT [Partition Number]
```
- `-MBR`: Specifies the MBR drive to get partition information.
- `/dev/[Drive]`: The drive from which the MBR will get partition information.
- `NULL`: Tells the installer to not flash the mbr.
- `-VBR`: Specifies the VBR file to flash.
- `/dev/[Drive][Partition]`: The target partition for the VBR installation.
- `myVBR.bin`: The VBR binary file.
- `FAT`: The filesystem type (e.g., FAT, EXT, BTRFS, NTFS).
- `[Partition Number]`: The partition number to install the VBR.

### Exporting the VBR

To export the VBR to a specific partition:

```bash
./output/bootsector-installer -EXPORT -VBR /dev/[Drive][Partition] myVBR.bin FAT [Partition Number]
```
- `-EXPORT`: Tells the installer to export the VBR to file without flashing it to the disk.
- `-VBR`: Specifies the VBR file to export.
- `/dev/[Drive][Partition]`: The partition from which the VBR will be exported.
- `myVBR.bin`: The output file for the exported VBR.
- `FAT`: The filesystem type (e.g., FAT, EXT, BTRFS, NTFS).
- `0`: The partition number for the VBR export (set to 0 because it is used to define the BootPartition field in the VBR structure during flashing).

### Flashing the MBR and VBR

To flash the MBR and VBR to a specific drive and partition:

```bash
./output/bootsector-installer -MBR /dev/[Drive] myMbr.bin -VBR /dev/[Drive][Partition] myVBR.bin FAT [Partition Number]
```
- `-MBR`: Specifies the MBR file to flash.
- `/dev/[Drive]`: The target drive where the MBR will be installed.
- `myMbr.bin`: The MBR binary file.
- `-VBR`: Specifies the VBR file to flash.
- `/dev/[Drive][Partition]`: The target partition for the VBR installation.
- `myVBR.bin`: The VBR binary file.
- `FAT`: The filesystem type (e.g., FAT, EXT, BTRFS, NTFS).
- `[Partition Number]`: The partition number to install the VBR.

### Exporting the MBR and VBR

To export the MBR and VBR to a specific drive and partition:

```bash
./output/bootsector-installer -EXPORT -MBR /dev/[Drive] myMbr.bin -VBR /dev/[Drive][Partition] myVBR.bin FAT [Partition Number]
```

- `-EXPORT`: Tells the installer to export the MBR and VBR to files without flashing them to the disk.
- `-MBR`: Specifies the MBR file to export.
- `/dev/[Drive]`: The drive from which the MBR and VBR will be exported.
- `myMbr.bin`: The output file for the exported MBR.
- `-VBR`: Specifies the VBR file to export.
- `/dev/[Drive][Partition]`: The partition from which the VBR will be exported.
- `myVBR.bin`: The output file for the exported VBR.
- `FAT`: The filesystem type (e.g., FAT, EXT, BTRFS, NTFS).
- `0`: The partition number for the VBR export (set to 0 because it is used to define the BootPartition field in the VBR structure during flashing).

## License

This project is licensed under the [MIT License](LICENSE).
