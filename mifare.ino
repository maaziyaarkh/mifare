

#include <SPI.h>
#include <PN532_SPI.h>
#include "PN532.h"
SPIClass PNSPI(SPI);
PN532_SPI pn532spi(PNSPI, D1);
PN532 nfc(pn532spi);

uint8_t defaultKey1[] = {0x57, 0x9B, 0x07, 0xB3, 0xA4, 0xE9};
uint8_t defaultKey2[] = {0x8D, 0x5F, 0x30, 0x7C, 0xF0, 0x5E};
uint8_t defaultKey3[] = {0xE9, 0xC1, 0x25, 0x4F, 0xA0, 0x7D};
uint8_t defaultKey4[] = {0xE3, 0x2F, 0x50, 0xF2, 0xE3, 0xD0};

uint8_t datasector3[] = {0x89, 0x51, 0x71, 0xF2, 0x1D, 0xCB, 0x78, 0x77, 0x88, 0xFF, 0xE9, 0xC1, 0x25, 0x4F, 0xA0, 0x7D};
uint8_t datasector6[] = {0xB2, 0x2C, 0xE4, 0x24, 0x7D, 0x90, 0x78, 0x77, 0x88, 0xFF, 0x57, 0x9B, 0x07, 0xB3, 0xA4, 0xE9};
uint8_t datasector4[] = {0x8F, 0x2D, 0x09, 0x3E, 0x7A, 0x6B, 0x48, 0x77, 0x8B, 0xFF, 0xE3, 0x2F, 0x50, 0xF2, 0xE3, 0xD0};
uint8_t datasector5[] = {0xA7, 0x2F, 0x60, 0xB9, 0x7C, 0x9F, 0x48, 0x77, 0x8B, 0xFF, 0x8D, 0x5F, 0x30, 0x7C, 0xF0, 0x5E};
uint8_t datasector65[] = {0xB2, 0x2C, 0xE4, 0x24, 0x7D, 0x90, 0x68, 0x77, 0x89, 0xFF, 0x57, 0x9B, 0x07, 0xB3, 0xA4, 0xE9};

uint8_t defaultKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t sampleData1[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint8_t datablock1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t datablock0[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t datablockFF[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t datablockF0[] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF};


uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void sectorlookUpTable(int sectornumber, uint8_t *keydatablock)
{
  if (sectornumber == 0)
  {
    memcpy(keydatablock, datasector3, 16);
  } else if (sectornumber == 1 || sectornumber == 2  || sectornumber == 3)
  {
    memcpy(keydatablock, datasector6, 16);
  }
  else if (sectornumber == 4 )
  {
    memcpy(keydatablock, datasector4, 16);
  } else if (sectornumber == 5 )
  {
    memcpy(keydatablock, datasector5, 16);
  } else if (sectornumber == 6 )
  {
    memcpy(keydatablock, datasector65, 16);
  }
  else if (sectornumber > 6  ) {
    memcpy(keydatablock, datasector6, 16);
  }

}

void setup(void) {


  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1);
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop(void) {

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    if (uidLength != 4) return;



    for (int j = 1; j < 64; j++)
    {
      if (j == 1) {
        nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 1, 1, defaultKey);
        nfc.mifareclassic_WriteDataBlock (1, datablock1 );
      }

      else if (j == 4 || j == 5 || j == 6 || j == 8 || j == 9 || j == 10 || j == 12 || j == 13 || j == 14 || j == 29 || j == 30
               || j == 33 || j == 34 || j == 37 || j == 38 || j == 41 || j == 42 || j == 45 || j == 46 || j == 49 || j == 50 || j == 53
               || j == 54 || j == 57 || j == 58 || j == 61 || j == 62 ) {
        nfc.mifareclassic_AuthenticateBlock(uid, uidLength, j, 1, defaultKey);
        nfc.mifareclassic_WriteDataBlock (j, datablock0 );
      }
      else if (j == 16 || j == 17) {
        nfc.mifareclassic_AuthenticateBlock(uid, uidLength, j, 1, defaultKey);
        nfc.mifareclassic_WriteDataBlock (j, datablockF0 );
      }
      else if (j == 20 || j == 21) {
        nfc.mifareclassic_AuthenticateBlock(uid, uidLength, j, 1, defaultKey);
        nfc.mifareclassic_WriteDataBlock (j, datablockF0 );
      }
      else if (j == 24 || j == 25 || j == 26) {
        nfc.mifareclassic_AuthenticateBlock(uid, uidLength, j, 1, defaultKey);
        nfc.mifareclassic_WriteDataBlock (j, datablockF0 );
      }
      else if (j == 2) {
        nfc.mifareclassic_AuthenticateBlock(uid, uidLength, j, 1, defaultKey);
        nfc.mifareclassic_WriteDataBlock (j, datablockFF );
      }
      else if (j == 28 || j == 32   || j == 36   || j == 40   || j == 44   || j == 48   || j == 52   || j == 56   || j == 60 ) {
        nfc.mifareclassic_AuthenticateBlock(uid, uidLength, j, 1, defaultKey);
        nfc.mifareclassic_WriteDataBlock (j, datablockFF );
      }
    }

delay(2000);

    for (int i = 0; i < 16; i++)
    {
      uint8_t dat[16];
      sectorlookUpTable(i, dat);
      nfc.PrintHex(dat, 16);

      if (!nfc.mifareclassic_AuthenticateBlock(uid, uidLength, i * 4 + 3, 1, defaultKey))
      {
        Serial.println("Authenticate failed");
        delay(1000);

      }

      Serial.println("Authenticate " + i);
      sectorlookUpTable(i, dat);
      if (!nfc.mifareclassic_WriteDataBlock(i * 4 + 3, dat))
      {
        Serial.println("error in write block 0");
        delay(1000);

      }
      Serial.println("write " + i);

    }

  }

}
