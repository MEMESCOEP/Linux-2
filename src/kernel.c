// Includes
#include "kernel.h"
#include "console.h"
#include "gdt.h"
#include "ide.h"
#include "idt.h"
#include "keyboard.h"
#include "string.h"
#include "ff.h"

// Variables
char *command;
char *Username = "root";
char *OS_Name = "TEST_OS";
char *OS_Version = "0.0.2";
char *cwd = "NoFS";

dword *SMI_CMD;
byte ACPI_ENABLE;
byte ACPI_DISABLE;
dword *PM1a_CNT;
dword *PM1b_CNT;
word SLP_TYPa;
word SLP_TYPb;
word SLP_EN;
word SCI_EN;
byte PM1_CNT_LEN;

struct RSDPtr
{
   byte Signature[8];
   byte CheckSum;
   byte OemID[6];
   byte Revision;
   dword *RsdtAddress;
};

struct FACP
{
   byte Signature[4];
   dword Length;
   byte unneded1[40 - 8];
   dword *DSDT;
   byte unneded2[48 - 44];
   dword *SMI_CMD;
   byte ACPI_ENABLE;
   byte ACPI_DISABLE;
   byte unneded3[64 - 54];
   dword *PM1a_CNT_BLK;
   dword *PM1b_CNT_BLK;
   byte unneded4[89 - 72];
   byte PM1_CNT_LEN;
};

FATFS FatFs;		/* FatFs work area needed for each volume */
FIL Fil;			/* File object needed for each open file */

// Functions
void kmain() {
   gdt_init();
   idt_init();
   
   UINT bw;
   FRESULT fr;


   /*f_mount(&FatFs, "", 0);		// Give a work area to the default drive 

   fr = f_open(&Fil, "newfile.txt", FA_WRITE | FA_CREATE_ALWAYS);	// Create a file
   if (fr == FR_OK) {
      f_write(&Fil, "It works!\r\n", 11, &bw);	// Write data to the file
      fr = f_close(&Fil);							// Close the file
      if (fr == FR_OK && bw == 11) {		// Lights green LED if data written well
         //DDRB |= 0x10; PORTB |= 0x10;	// Set PB4 high
      }
   }*/

   console_init(COLOR_WHITE, COLOR_BLACK);
   keyboard_init();
   ata_init();

   console_clear(COLOR_WHITE, COLOR_BLACK);

   command[0] = 0;

   printf("%s (%s)\nType \"help\" for a list of commands.\n\n", OS_Name, OS_Version);
   printf("%s@%s[%s] >> ", Username, OS_Name, cwd);

   while(1){
      char ch = kb_getchar();
      if(ch != '\n' && ch != 0 && ch != '\0'){
               append(command, ch);
               printf("%c", ch);
      }
      else{		
         printf("\n");
         if(strcmp(command, "") != 0){
            interpret(command);
            printf("\n");
         }

         printf("%s@%s[%s] >> ", Username, OS_Name, cwd);
      }
   }   
}

void append(char* s, char c) {
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}


void interpret(char* cmd){

	if(strcmp(cmd, "bruh") == 0){
		printf("bruh amogus lol\n");
	}

   else if(strcmp(cmd, "") == 0){

   }
   
	else if(strcmp(cmd, "shutdown") == 0){
		printf("TEST_OS Shutting down...\n");
        initAcpi();
        acpiPowerOff();
		printf("Shutdown failed.\n");
	}

    else if(strcmp(cmd, "reboot") == 0){
		printf("TEST_OS Rebooting...\n");
        reboot();
		printf("Reboot failed.\n");
	}

   else if(strcmp(cmd, "clear") == 0){
      console_clear(COLOR_WHITE, COLOR_BLACK);
   }

   else if(strcmp(cmd, "fstest") == 0){
      char* DiskName = "VBOX HARDDISK";
      printf("\nFilesystem Example\n");
      const int DRIVE = ata_get_drive_by_model(DiskName);

      if(DRIVE != -1){
         const uint32 LBA = 0;
         const uint8 NO_OF_SECTORS = 1;
         char buf[ATA_SECTOR_SIZE] = {0};

         struct example {
            int id;
            char name[32];
         };

         struct example e;
         e.id = 10012;
         strcpy(e.name, "Iron Man");

         // write message to drive
         strcpy(buf, "Hello World");
         ide_write_sectors(DRIVE, NO_OF_SECTORS, LBA, (uint32)buf);

         memset(buf, 0, sizeof(buf));
         memcpy(buf, &e, sizeof(e));
         ide_write_sectors(DRIVE, NO_OF_SECTORS, LBA + 1, (uint32)buf);
         printf("data written\n");

         // read message from drive
         memset(buf, 0, sizeof(buf));
         ide_read_sectors(DRIVE, NO_OF_SECTORS, LBA, (uint32)buf);
         printf("read data: %s\n", buf);

         memset(buf, 0, sizeof(buf));
         ide_read_sectors(DRIVE, NO_OF_SECTORS, LBA + 1, (uint32)buf);
         memcpy(&e, buf, sizeof(e));
         printf("id: %d, name: %s\n", e.id, e.name);
      }else{
         printf("ERROR: No drive with name \"%s\" was found.", DiskName);
      }
   }

   else if(strcmp(cmd, "dsk") == 0){
      IDE_DEVICE *drives = ata_get_drives();
      for(int i = 0; i < MAXIMUM_IDE_DEVICES; i++){
         if(drives[i].type == 0){
            if(strcmp(drives[i].model, "") != 0){
               printf("Hard Drive: \"%s\"\n", drives[i].model);
               printf("Size: %d bytes\n\n", (drives[i].size));
            }
         }
         else if(drives[i].type == 1){
            if(strcmp(drives[i].model, "") != 0){
               printf("DVD-ROM Drive: \"%s\"\n", drives[i].model);
            }
         }
         else{
            printf("Unknown: %s (Type=%d)\n", drives[i].model, drives[i].type);
         }         
      }
   }

	else {
		printf("Invalid command: ");
		printf(cmd);
		printf("\n");
	}
		
	for(int i = 0; i < strlen(command); i++){
		command[i] = 0;
	}

   //console_scroll(SCROLL_DOWN);
}

void reboot()
{
    uint8 good = 0x02;
    while (good & 0x02)
        good = inportb(0x64);
    outportb(0x64, 0xFE);
}

dword *SMI_CMD;
byte ACPI_ENABLE;
byte ACPI_DISABLE;
dword *PM1a_CNT;
dword *PM1b_CNT;
word SLP_TYPa;
word SLP_TYPb;
word SLP_EN;
word SCI_EN;
byte PM1_CNT_LEN;



// check if the given address has a valid header
unsigned int *acpiCheckRSDPtr(unsigned int *ptr)
{
   char *sig = "RSD PTR ";
   struct RSDPtr *rsdp = (struct RSDPtr *) ptr;
   byte *bptr;
   byte check = 0;
   int i;

   if (memcmp(sig, rsdp, 8) == 0)
   {
      // check checksum rsdpd
      bptr = (byte *) ptr;
      for (i=0; i<sizeof(struct RSDPtr); i++)
      {
         check += *bptr;
         bptr++;
      }

      // found valid rsdpd   
      if (check == 0) {
         /*
          if (desc->Revision == 0)
            printf("acpi 1");
         else
            printf("acpi 2");
         */
         return (unsigned int *) rsdp->RsdtAddress;
      }
   }

   return NULL;
}



// finds the acpi header and returns the address of the rsdt
unsigned int *acpiGetRSDPtr(void)
{
   unsigned int *addr;
   unsigned int *rsdp;

   // search below the 1mb mark for RSDP signature
   for (addr = (unsigned int *) 0x000E0000; (int) addr<0x00100000; addr += 0x10/sizeof(addr))
   {
      rsdp = acpiCheckRSDPtr(addr);
      if (rsdp != NULL)
         return rsdp;
   }


   // at address 0x40:0x0E is the RM segment of the ebda
   int ebda = *((short *) 0x40E);   // get pointer
   ebda = ebda*0x10 &0x000FFFFF;   // transform segment into linear address

   // search Extended BIOS Data Area for the Root System Description Pointer signature
   for (addr = (unsigned int *) ebda; (int) addr<ebda+1024; addr+= 0x10/sizeof(addr))
   {
      rsdp = acpiCheckRSDPtr(addr);
      if (rsdp != NULL)
         return rsdp;
   }

   return NULL;
}



// checks for a given header and validates checksum
int acpiCheckHeader(unsigned int *ptr, char *sig)
{
   if (memcmp(ptr, sig, 4) == 0)
   {
      char *checkPtr = (char *) ptr;
      int len = *(ptr + 1);
      char check = 0;
      while (0<len--)
      {
         check += *checkPtr;
         checkPtr++;
      }
      if (check == 0)
         return 0;
   }
   return -1;
}



int acpiEnable(void)
{
   // check if acpi is enabled
   if ( (inportb((unsigned int) PM1a_CNT) &SCI_EN) == 0 )
   {
      // check if acpi can be enabled
      if (SMI_CMD != 0 && ACPI_ENABLE != 0)
      {
         outportb((unsigned int) SMI_CMD, ACPI_ENABLE); // send acpi enable command
         // give 3 seconds time to enable acpi
         int i;
         for (i=0; i<300; i++ )
         {
            if ( (inportb((unsigned int) PM1a_CNT) &SCI_EN) == 1 )
               break;
            //sleep(10);
         }
         if (PM1b_CNT != 0)
            for (; i<300; i++ )
            {
               if ( (inportb((unsigned int) PM1b_CNT) &SCI_EN) == 1 )
                  break;
               //sleep(10);
            }
         if (i<300) {
            printf("enabled acpi.\n");
            return 0;
         } else {
            printf("couldn't enable acpi.\n");
            return -1;
         }
      } else {
         printf("no known way to enable acpi.\n");
         return -1;
      }
   } else {
      //printf("acpi was already enabled.\n");
      return 0;
   }
}



//
// bytecode of the \_S5 object
// -----------------------------------------
//        | (optional) |    |    |    |   
// NameOP | \          | _  | S  | 5  | _
// 08     | 5A         | 5F | 53 | 35 | 5F
//
// -----------------------------------------------------------------------------------------------------------
//           |           |              | ( SLP_TYPa   ) | ( SLP_TYPb   ) | ( Reserved   ) | (Reserved    )
// PackageOP | PkgLength | NumElements  | byteprefix Num | byteprefix Num | byteprefix Num | byteprefix Num
// 12        | 0A        | 04           | 0A         05  | 0A          05 | 0A         05  | 0A         05
//
//----this-structure-was-also-seen----------------------
// PackageOP | PkgLength | NumElements |
// 12        | 06        | 04          | 00 00 00 00
//
// (Pkglength bit 6-7 encode additional PkgLength bytes [shouldn't be the case here])
//
int initAcpi(void)
{
   unsigned int *ptr = acpiGetRSDPtr();

   // check if address is correct  ( if acpi is available on this pc )
   if (ptr != NULL && acpiCheckHeader(ptr, "RSDT") == 0)
   {
      // the RSDT contains an unknown number of pointers to acpi tables
      int entrys = *(ptr + 1);
      entrys = (entrys-36) /4;
      ptr += 36/4;   // skip header information

      while (0<entrys--)
      {
         // check if the desired table is reached
         if (acpiCheckHeader((unsigned int *) *ptr, "FACP") == 0)
         {
            entrys = -2;
            struct FACP *facp = (struct FACP *) *ptr;
            if (acpiCheckHeader((unsigned int *) facp->DSDT, "DSDT") == 0)
            {
               // search the \_S5 package in the DSDT
               char *S5Addr = (char *) facp->DSDT +36; // skip header
               int dsdtLength = *(facp->DSDT+1) -36;
               while (0 < dsdtLength--)
               {
                  if ( memcmp(S5Addr, "_S5_", 4) == 0)
                     break;
                  S5Addr++;
               }
               // check if \_S5 was found
               if (dsdtLength > 0)
               {
                  // check for valid AML structure
                  if ( ( *(S5Addr-1) == 0x08 || ( *(S5Addr-2) == 0x08 && *(S5Addr-1) == '\\') ) && *(S5Addr+4) == 0x12 )
                  {
                     S5Addr += 5;
                     S5Addr += ((*S5Addr &0xC0)>>6) +2;   // calculate PkgLength size

                     if (*S5Addr == 0x0A)
                        S5Addr++;   // skip byteprefix
                     SLP_TYPa = *(S5Addr)<<10;
                     S5Addr++;

                     if (*S5Addr == 0x0A)
                        S5Addr++;   // skip byteprefix
                     SLP_TYPb = *(S5Addr)<<10;

                     SMI_CMD = facp->SMI_CMD;

                     ACPI_ENABLE = facp->ACPI_ENABLE;
                     ACPI_DISABLE = facp->ACPI_DISABLE;

                     PM1a_CNT = facp->PM1a_CNT_BLK;
                     PM1b_CNT = facp->PM1b_CNT_BLK;
                     
                     PM1_CNT_LEN = facp->PM1_CNT_LEN;

                     SLP_EN = 1<<13;
                     SCI_EN = 1;

                     return 0;
                  } else {
                     printf("\\_S5 parse error.\n");
                  }
               } else {
                  printf("\\_S5 not present.\n");
               }
            } else {
               printf("DSDT invalid.\n");
            }
         }
         ptr++;
      }
      printf("no valid FACP present.\n");
   } else {
      printf("no acpi.\n");
   }

   return -1;
}

void acpiPowerOff(void)
{
   acpiEnable();

   // send the shutdown command
   outportb((unsigned int) PM1a_CNT, SLP_TYPa | SLP_EN );
   if ( PM1b_CNT != 0 )
      outportb((unsigned int) PM1b_CNT, SLP_TYPb | SLP_EN );

   printf("acpi poweroff failed.\n");
}