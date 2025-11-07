#ifndef _STORAGE_CARD_H
#define _STORAGE_CARD_H

#include <SD.h>
#include "IO.h"
#include "constants.h"

/*                                REGISTERS                           */
/* ------------------------------------------------------------------ */
/* | # |            READ             |             WRITE            | */
/* ------------------------------------------------------------------ */
/* | 0 |                        DATA REGISTER                       | */
/* ------------------------------------------------------------------ */
/* | 1 |       ERROR REGISTER        |        FEATURE REGISTER      | */
/* ------------------------------------------------------------------ */
/* | 2 |                   SECTOR COUNT REGISTER                    | */
/* ------------------------------------------------------------------ */
/* | 3 |                        LBA 0 [0:7]                         | */
/* ------------------------------------------------------------------ */
/* | 4 |                        LBA 1 [8:15]                        | */
/* ------------------------------------------------------------------ */
/* | 5 |                        LBA 2 [16:23]                       | */
/* ------------------------------------------------------------------ */
/* | 6 |                     LBA 3 [24:27 (LSB)]                    | */
/* ------------------------------------------------------------------ */
/* | 7 |      STATUS REGISTER        |        COMMAND REGISTER      | */
/* ------------------------------------------------------------------ */
/*                                                                    */
/* ERROR REGISTER                                                     */
/* | 7   | 6   | 5  | 4    | 3 | 2    | 1 | 0    |                    */
/* | BBK | UNC | X  | IDNF | X | ABRT | X | AMNF |                    */
/* | 0   | 0   | 0  | 0    | 0 | 0    | 0 |  0   |  <- Default Values */
/*                                                                    */
/* BBK  - Bad Block Detected                                          */
/* UNC  - Uncorrectable Error                                         */
/* IDNF - Sector ID Not Found                                         */
/* ABRT - Aborted Command                                             */
/* AMNF - Address Mark Not Found (General Error)                      */
/*                                                                    */
/* LBA 3 REGISTER                                                     */
/* | 7 | 6   | 5 | 4   | 3 | 2 | 1 | 0 |                              */
/* | X | LBA | X | DRV |   LBA[24:27]  |                              */
/* | 1 | 1   | 1 | 0   | 0 | 0 | 0 | 0 |  <- Default Values           */
/*                                                                    */
/* Only True IDE mode with LBA is supported so we ignore bits 4-7     */
/*                                                                    */
/*                                                                    */
/* STATUS REGISTER                                                    */
/* | 7   | 6   | 5   | 4   | 3   | 2    | 1   | 0   |                 */
/* | BSY | RDY | DWF | DSC | DRQ | CORR | IDX | ERR |                 */
/* | 0   | 1   | 0   | 0   | 0   | 0    | 0   |  0  |  <- Default Val */
/*                                                                    */
/* Note: The emulated CF card is always RDY and never BSY             */
/*                                                                    */
/* BSY  - Drive Busy                                                  */
/* RDY  - Drive Ready                                                 */
/* DWF  - Drive Write Full                                            */
/* DSC  - Drive Seek Complete                                         */
/* DRQ  - Data Request (Data can be transferred)                      */
/* CORR - Data has been corrected                                     */
/* DSC  - Drive Seek Complete                                         */
/* ERR  - Error occurred                                              */
/*                                                                    */

#define ST_ERR_AMNF       0b00000001
#define ST_ERR_ABRT       0b00000100
#define ST_ERR_IDNF       0b00010000
#define ST_ERR_UNC        0b01000000
#define ST_ERR_BBK        0b10000000

#define ST_STATUS_ERR     0b00000001
#define ST_STATUS_IDX     0b00000010
#define ST_STATUS_CORR    0b00000100
#define ST_STATUS_DRQ     0b00001000
#define ST_STATUS_DSC     0b00010000
#define ST_STATUS_DWF     0b00100000
#define ST_STATUS_RDY     0b01000000
#define ST_STATUS_BSY     0b10000000

#define ST_STORAGE_SIZE   0x8000000     // 128MB (134217728 bytes)
#define ST_SECTOR_SIZE    0x200         // 512 bytes
#define ST_SECTOR_COUNT   0x40000       // 262144 512 byte sectors
#define ST_IDENTITY_SIZE  0x100         // 256 bytes 

#define ST_STORAGE_FILE_NAME    "Storage.bin"
#define ST_IDENTITY_FILE_NAME   "Identity.bin"

class StorageCard: public IO {
  private:
    uint8_t buffer[0x200]; // 512 bytes
    uint16_t bufferIndex;
    uint16_t commandDataSize;
    uint32_t sectorOffset;
    
    uint8_t error;
    uint8_t feature;
    uint8_t sectorCount;
    uint8_t lba0;
    uint8_t lba1;
    uint8_t lba2;
    uint8_t lba3;
    uint8_t status;
    uint8_t command;

    bool isIdentifying;
    bool isTransferring;

    void executeCommand();
    uint8_t readBuffer();
    void writeBuffer(uint8_t value);
    uint32_t sectorIndex();
    bool sectorValid();
    void generateIdentity(uint8_t *identity);

  public:
    StorageCard();
    ~StorageCard() {}

    void begin();

    uint8_t id() { return IO_STORAGE_CARD; }
    String  description() override { return "Storage Card"; }
    bool    passthrough() override { return false; }
    uint8_t read(uint16_t address) override;
    void    write(uint16_t address, uint8_t value) override;
    uint8_t tick() { return 0x00; }
    void    reset() override;
};

#endif