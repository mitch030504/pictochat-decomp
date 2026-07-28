#ifndef NDS_FIRMWARE_H
#define NDS_FIRMWARE_H

#include "nds/types.h"

// Touch screen calibration point pair
typedef struct {
    u16 adc_x;
    u16 adc_y;
    u8  screen_x;
    u8  screen_y;
} TouchCalibrationPoint;

// NDS Firmware User Settings structure (512 bytes)
typedef struct {
    u16 version;                    // Always 5
    u8  favorite_color;             // 0..15
    u8  birthday_month;             // 1..12
    u8  birthday_day;               // 1..31
    u8  reserved1;
    u16 nickname[10];               // UTF-16LE nickname string
    u16 nickname_len;               // 0..10
    u16 message[26];                // UTF-16LE message string
    u16 message_len;                // 0..26
    u8  alarm_hour;                 // 0..23
    u8  alarm_minute;               // 0..59
    u16 reserved2;
    u8  alarm_enable;               // Bitflags for alarm enable
    u8  padding;
    TouchCalibrationPoint touch_cal1; // Cal point 1
    TouchCalibrationPoint touch_cal2; // Cal point 2
    u16 language_and_flags;         // Language code + console flags
    u8  year;                       // Year offset (2000..2255)
    u8  reserved3;
    u32 rtc_offset;                 // RTC time adjustment offset
    u16 update_counter;             // NVRAM update cycle counter
    u16 crc16;                      // CRC16 checksum over bytes 0x00..0x6F
} FirmwareUserSettings;

// NDS Firmware Header structure (per GBATEK "DS Firmware Header").
// All "raw" fields need their documented shift/multiplier applied to get a
// real byte offset into firmware.bin - see tools/extract_firmware.py.
typedef struct {
    u16 part3_romaddr_div8;         // 0x00: Part 3 (ARM9 GUI code, LZ/Huffman) romaddr/8
    u16 part4_romaddr_div8;         // 0x02: Part 4 (ARM7 WiFi code, LZ/Huffman) romaddr/8
    u16 part34_crc16;               // 0x04: CRC16 of Part 3/4 (ARM9/7 GUI/WiFi code)
    u16 part12_crc16;               // 0x06: CRC16 of Part 1/2 (ARM9/7 boot code)
    u8  header_id[4];               // 0x08: firmware identifier, usually "MAC",nn
    u16 part1_romaddr_raw;          // 0x0C: Part 1 (ARM9 boot, LZSS) romaddr / 2^(2+shift1)
    u16 part1_ramaddr_raw;          // 0x0E: Part 1: 0x2800000 - ramaddr, / 2^(2+shift2)
    u16 part2_romaddr_raw;          // 0x10: Part 2 (ARM7 boot, LZSS) romaddr / 2^(2+shift3)
    u16 part2_ramaddr_raw;          // 0x12: Part 2: 0x3810000 - ramaddr, / 2^(2+shift4)
    u16 shift_amounts;              // 0x14: bit0-2=shift1 bit3-5=shift2 bit6-8=shift3
                                     //       bit9-11=shift4 bit12-15=chipsize/128K
    u16 part5_romaddr_div8;         // 0x16: Part 5 (data/gfx, LZ/Huffman) romaddr/8
    u8  build_timestamp[8];         // 0x18: optional KEY1-encrypted "enPngOFF" or timestamp
    u8  console_type;               // 0x1D
    u16 unused;                     // 0x1E: FFh-filled
    u16 user_settings_offset_div8;  // 0x20: User Settings block offset / 8
} FirmwareHeader;

#endif // NDS_FIRMWARE_H
