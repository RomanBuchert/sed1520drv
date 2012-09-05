/*
 * main.c
 *
 *  Created on: 01.03.2011
 *      Author: rb
 */
#include <linux/ppdev.h>
#include <linux/parport.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <mytypes.h>
#include <sed1520.h>
#include <time.h>
#include "clips.h"

TSed1520 *pSed1520;
int fd, mode;
struct ppdev_frob_struct frob;
__u8 empty[32][122];
void WriteDaten(__u8 u8Daten)
{
	ioctl(fd, PPWDATA, &u8Daten);
}

void WriteA0(__u8 u8Daten)
{
	frob.mask = PARPORT_CONTROL_INIT;
	frob.val = PARPORT_CONTROL_INIT & ((u8Daten&0x01)<<2);
	ioctl(fd, PPFCONTROL, &frob);
}

void WriteE0(__u8 u8Daten)
{
	u8Daten ^= 0xFF;
	frob.mask = PARPORT_CONTROL_STROBE;
	frob.val = ((u8Daten&0x01)<<0);
	ioctl(fd, PPFCONTROL, &frob);
}

void WriteE1(__u8 u8Daten)
{
	u8Daten ^= 0xFF;
	frob.mask = PARPORT_CONTROL_AUTOFD;
	frob.val =  ((u8Daten&0x01)<<1);
	ioctl(fd, PPFCONTROL, &frob);
}

int main(int argc, char **argv)
{
   int retval;

   Sed1520Init();
   Sed1520GetDisplayStruct(&pSed1520);
   pSed1520->WriteData = WriteDaten;
   pSed1520->WriteA0 = WriteA0;
   pSed1520->WriteE0 = WriteE0;
   pSed1520->WriteE1 = WriteE1;
   pSed1520->Pixelschwelle = 0x20;

   //Parallelport oeffnen.
   fd = open("/dev/parport0",O_RDWR);
   if (fd == 0)
   {
      printf("Fehler: /dev/parport0 konnte nicht geoeffnet werden!\n");
      return -1;
   }

   //Exclusiver Zugriff auf Parallelport versuchen
   retval = ioctl(fd, PPCLAIM);
   if (retval)
   {
      printf("Fehler: Konnte keine Zugriff auf den Parallelport bekommen\n");
      close(fd);
      return -2;
   }

   //Modus des Parallelports einstellen
   mode = IEEE1284_MODE_COMPAT;
   retval = ioctl (fd, PPNEGOT, &mode);
   if (retval)
   {
      printf("Fehler: Konnte Modus der Schnittstelle nicht einstellen\n");
      close(fd);
      return -3;
   }

   Sed1520InitDisplay();
   Sed1520DrawBmp(tstbmp_xpm);
   sleep(5);
   Sed1520DrawBmp(clip2_xpm);
   sleep(5);
   Sed1520DrawBmp(clip3_xpm);
   sleep(5);
   Sed1520DrawBmp(clip4_xpm);
   sleep(5);
   Sed1520DrawBmp(clip5_xpm);
   sleep(5);
   Sed1520DrawBmp(empty);
   ioctl(fd, PPRELEASE);
   close(fd);
   return 0;
}

