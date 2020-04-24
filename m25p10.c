/*!
 * \file sample-static.c
 *
 * \author FTDI
 * \date 20110512
 *
 * Copyright © 2000-2014 Future Technology Devices International Limited
 *
 *
 * THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Project: libMPSSE
 * Module: SPI Sample Application - Interfacing 94LC56B SPI EEPROM
 *
 * Rivision History:
 * 0.1  - 20110512 - Initial version
 * 0.2  - 20110801 - Changed LatencyTimer to 255
 * 					 Attempt to open channel only if available
 *					 Added & modified macros
 *					 Included stdlib.h
 * 0.3  - 20111212 - Added comments
 * 0.41 - 20140903 - Fixed compilation warnings
 *					 Added testing of SPI_ReadWrite()
 */

 /******************************************************************************/
 /* 							 Include files										   */
 /******************************************************************************/
 /* Standard C libraries */
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
/* OS specific libraries */
#ifdef _WIN32
#include<windows.h>
#endif

/* Include D2XX header*/
#include "ftd2xx.h"

/* Include libMPSSE header */
#include "libMPSSE_spi.h"

/******************************************************************************/
/*								Macro and type defines							   */
/******************************************************************************/
/* Helper macros */

#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

/* Application specific macro definations */
#define SPI_DEVICE_BUFFER_SIZE		256


/******************************************************************************/
/*								Global variables							  	    */
/******************************************************************************/
static FT_HANDLE ftHandle;
static uint8 buffer[SPI_DEVICE_BUFFER_SIZE] = { 0 };


int verifyDeviceM25P10Compatibility(void)
{

	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command + dummy address
	sizeToTransfer = 4;
	sizeTransfered = 0;
	buffer[0] = 0xAB; //Command: Read Electronic Signature
	buffer[1] = 0; //Dummy address
	buffer[2] = 0; //Dummy address
	buffer[3] = 0; //Dummy address
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	APP_CHECK_STATUS(status);

	// Read device type 3 times
	sizeToTransfer = 3;
	sizeTransfered = 0;
	status = SPI_Read(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);

	printf("Received version: 0x%02X 0x%02X 0x%02X\n", buffer[0], buffer[1], buffer[2]);
	return (buffer[0] == 0x10 && buffer[1] == 0x10 && buffer[2] == 0x10) ? 1 : 0;
}

uint8_t getStatusRegister(void)
{

	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command
	sizeToTransfer = 1;
	sizeTransfered = 0;
	buffer[0] = 0x05; //Command: Read status register
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	APP_CHECK_STATUS(status);

	// Read status register
	sizeToTransfer = 1;
	sizeTransfered = 0;
	status = SPI_Read(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);

	return buffer[0];
}

void dumpStatusRegister(void)
{
	uint8_t statusRegister = getStatusRegister();
	printf("Status register: %02X\n", statusRegister);
	printf("Status Register Write Protect (SRWP): %d\n", statusRegister >> 7);
	printf("Block Protect Bit 2            (BP2): %d\n", (statusRegister >> 4) & 1);
	printf("Block Protect Bit 1            (BP1): %d\n", (statusRegister >> 3) & 1);
	printf("Block Protect Bit 0            (BP0): %d\n", (statusRegister >> 2) & 1);
	printf("Write Enable Latch Bit         (WEL): %d\n", (statusRegister >> 1) & 1);
	printf("Write In Progress Bit          (WIP): %d\n", (statusRegister >> 0) & 1);
}



void writeEnable(void)
{

	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command
	sizeToTransfer = 1;
	sizeTransfered = 0;
	buffer[0] = 0x06; //Command: Write enable
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);
}

void writeDisable(void)
{

	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command
	sizeToTransfer = 1;
	sizeTransfered = 0;
	buffer[0] = 0x04; //Command: Write disable
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);
}

void pageProgram(int address, uint8_t* data, uint32 count)
{
	uint8_t* p = data;

	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command + address
	sizeToTransfer = 4;
	sizeTransfered = 0;
	buffer[0] = 0x02; //Command: Page program
	buffer[1] = (address >> 16) & 0xFF; //Address H
	buffer[2] = (address >> 8) & 0xFF; //Address
	buffer[3] = (address >> 0) & 0xFF; //Address L
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	APP_CHECK_STATUS(status);

	while (count)
	{
		// Do partly transfers of 128 bytes due to buffer limitations
		sizeToTransfer = 128;
		sizeTransfered = 0;

		if (sizeToTransfer > count) sizeToTransfer = count;

		uint32_t options = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES;

		count -= sizeToTransfer;

		if (count == 0) options |= SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;

		status = SPI_Write(ftHandle, p, sizeToTransfer, &sizeTransfered, options); //Note: Reading from "data" buffer instead of the global buffer
		p += sizeToTransfer;
		APP_CHECK_STATUS(status);
	}

}


void waitWhileWriteInProgress()
{
	uint8_t status;
	do {
		status = getStatusRegister();
	} while ((status & 1) == 1); //Wait until write in progress bit (WIP) is 0
}


void programFromFile(char* filename, int address)
{
	uint8_t buf[256];

	printf("Writing from file:\n");

	uint32_t pos = 0;
	int bufPos = 0;

	FILE* fData = fopen(filename, "rb");
	if (fData == NULL)
	{
		printf("ERROR: Failed to open source file\n");
		return;
	}

	while (!feof(fData))
	{
		fread(buf + bufPos, 1, 1, fData);
		bufPos++;
		if (bufPos == 256)
		{
			bufPos = 0;
			writeEnable();
			pageProgram(address, buf, 256);
			waitWhileWriteInProgress();
			address += 256;
		}
	}

	if (bufPos) {
		writeEnable();
		pageProgram(address, buf, bufPos);
		waitWhileWriteInProgress();
	}

	fclose(fData);
}


void eraseSector(int address)
{
	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command
	sizeToTransfer = 4;
	sizeTransfered = 0;
	buffer[0] = 0xD8; //Command: Sector erase
	buffer[1] = (address >> 16) & 0xFF; //Address H
	buffer[2] = (address >> 8) & 0xFF; //Address
	buffer[3] = (address >> 0) & 0xFF; //Address L
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);

	waitWhileWriteInProgress();
}


void bulkErase(void)
{
	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command
	sizeToTransfer = 1;
	sizeTransfered = 0;
	buffer[0] = 0xC7; //Command: Bulk erase
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);

	waitWhileWriteInProgress();
}



void readDeviceIdentification(void)
{
	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command + dummy address
	sizeToTransfer = 1;
	sizeTransfered = 0;
	buffer[0] = 0x9F; //Command: Read Identification
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	APP_CHECK_STATUS(status);

	// Read manufacturer + device identification
	sizeToTransfer = 20;
	sizeTransfered = 0;
	status = SPI_Read(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);

	printf("Manufacturer: %02X\n", buffer[0]);
	printf("Device identification: %02X %02X\n", buffer[1], buffer[2]);
	printf("UID:");
	for (int i = 3; i < 20; i++)
	{
		printf(" %02X", buffer[i]);
	}
	printf("\n");
}



void debugDeviceType(void)
{

	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command + dummy address
	sizeToTransfer = 4;
	sizeTransfered = 0;
	buffer[0] = 0xAB; //Command: Read Electronic Signature
	buffer[1] = 0; //Dummy address
	buffer[2] = 0; //Dummy address
	buffer[3] = 0; //Dummy address
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	APP_CHECK_STATUS(status);

	// Read device type 3 times
	sizeToTransfer = 3;
	sizeTransfered = 0;
	status = SPI_Read(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);

	printf("Received version: %u %u %u\n", buffer[0], buffer[1], buffer[2]);

}

void dump10bytes(void)
{

	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command + address
	sizeToTransfer = 4;
	sizeTransfered = 0;
	buffer[0] = 0x03; //Command: Read
	buffer[1] = 0; //Address H
	buffer[2] = 0; //Address
	buffer[3] = 0; //Address L
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	APP_CHECK_STATUS(status);

	// Read 10 bytes
	sizeToTransfer = 10;
	sizeTransfered = 0;
	status = SPI_Read(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	APP_CHECK_STATUS(status);

	printf("Data readout:\n");
	for (int i = 0; i < (int)sizeToTransfer; ++i)
	{
		printf(" %X", buffer[i]);
	}
	printf("\n");

}

void dumpData(uint32_t address, uint32_t count)
{
	//Max 25 MHz ("Read data at higher speed-command" (0x0B) supports 50 MHz)

	FILE* fOut = fopen("output.bin", "wb");
	if (fOut == NULL)
	{
		printf("ERROR: Failed to open file for output\n");
		return;
	}

	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command + address
	sizeToTransfer = 4;
	sizeTransfered = 0;
	buffer[0] = 0x03; //Command: Read
	buffer[1] = (address >> 16) & 0xFF; //Address H
	buffer[2] = (address >> 8) & 0xFF; //Address
	buffer[3] = (address >> 0) & 0xFF; //Address L
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	APP_CHECK_STATUS(status);

	printf("Data readout:\n");

	uint32_t pos = 0;

	while (count)
	{
		// Read chunks of 16 bytes
		sizeToTransfer = 16;
		sizeTransfered = 0;

		if (sizeToTransfer > count) sizeToTransfer = count;

		uint32_t options = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES;

		count -= sizeToTransfer;

		if (count == 0) options |= SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;

		status = SPI_Read(ftHandle, buffer, sizeToTransfer, &sizeTransfered, options);
		APP_CHECK_STATUS(status);

		fwrite(buffer, 1, sizeToTransfer, fOut);

		printf("%06X:", pos);


		for (uint32 i = 0; i < sizeToTransfer; ++i)
		{
			printf(" %02X", buffer[i]);
		}
		printf("\n");

		pos += 16;

	}

	fclose(fOut);

}

int verifyChunk(uint8_t* buf, uint32_t len, uint32_t offset)
{
	uint32 sizeToTransfer = len;
	uint32 sizeTransfered = 0;
	FT_STATUS status;

	status = SPI_Read(ftHandle, buffer, sizeToTransfer, &sizeTransfered, SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES);
	APP_CHECK_STATUS(status);

	for (uint32_t i = 0; i < len; ++i)
	{
		if (buf[i] != buffer[i])
		{
			printf("ERROR: Difference found, offset 0x%06X (file: 0x%02X, flash: 0x%02X\n", offset + i, buf[i], buffer[i]);
			return 1;
		}
	}
	return 0;
}


void verifyFile(char* filename, uint32_t address)
{
	//Max 25 MHz ("Read data at higher speed-command" (0x0B) supports 50 MHz)

	uint32_t addressStart = address;

	printf("Verify against file...\n");


	uint8_t buf[16];
	uint32_t bufPos = 0;

	FILE* fData = fopen(filename, "rb");
	if (fData == NULL)
	{
		printf("ERROR: Failed to open source file\n");
		return;
	}

	uint32 sizeToTransfer = 0;
	uint32 sizeTransfered;
	uint8 writeComplete = 0;
	uint32 retry = 0;
	FT_STATUS status;

	// Command + address
	sizeToTransfer = 4;
	sizeTransfered = 0;
	buffer[0] = 0x03; //Command: Read
	buffer[1] = (address >> 16) & 0xFF; //Address H
	buffer[2] = (address >> 8) & 0xFF; //Address
	buffer[3] = (address >> 0) & 0xFF; //Address L
	status = SPI_Write(ftHandle, buffer, sizeToTransfer, &sizeTransfered,
		SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES |
		SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE);
	APP_CHECK_STATUS(status);

	while (!feof(fData))
	{
		fread(buf + bufPos, 1, 1, fData);
		bufPos++;
		if (bufPos == 16)
		{
			bufPos = 0;


			if (verifyChunk(buf, 16, address) != 0)
			{
				fclose(fData);
				status = SPI_ToggleCS(ftHandle, FALSE);
				APP_CHECK_STATUS(status);
				return;
			}

			address += 16;
		}
	}

	if (bufPos) {
		if (verifyChunk(buf, bufPos, address) != 0)
		{
			fclose(fData);
			status = SPI_ToggleCS(ftHandle, FALSE);
			APP_CHECK_STATUS(status);
			return;
		}
		address += bufPos;
	}

	fclose(fData);
	status = SPI_ToggleCS(ftHandle, FALSE);
	APP_CHECK_STATUS(status);


	printf("Verification OK. Compared from 0x%06X to 0x%06X, %u bytes\n", addressStart, address - 1, address - addressStart);



}



void advancedMenu(void)
{

	int running = 1;
	while (running)
	{
		printf("\n\nAdvanced menu:\n");
		printf("I: Read identification\n");
		printf("S: Read status register\n");
		printf("R: Read entire flash to \"output.bin\"\n");
		printf("E: Write enable\n");
		printf("D: Write disable\n");

		printf("W: Write to flash from \"input.bin\"\n");
		printf("C: Erase sector (write must be enabled first)\n");
		printf("B: Bulk erase (write must be enabled first)\n");
		printf("V: Verify flash against \"input.bin\"\n");
		printf("M: Print advanced menu\n");
		printf("Q: Quit to main menu\n\n");

		scanf("%s", buffer);

		char cmd = buffer[0];
		if (cmd >= 'a' && cmd <= 'z') cmd = cmd - 'a' + 'A'; //UPPERCASE

		switch (cmd) {
		case 'I':
			readDeviceIdentification();
			break;

		case 'S':
			dumpStatusRegister();
			break;

		case 'R':
			dumpData(0, 0x20000);
			break;

		case 'E':
			writeEnable();
			break;

		case 'D':
			writeDisable();
			break;

		case 'W':
			//programFromFile("input.bin", 0x10000);
			programFromFile("input.bin", 0);
			break;

		case 'C':
			//eraseSector(0x10000);
			eraseSector(0);
			break;

		case 'B':
			bulkErase();
			break;

		case 'V':
			verifyFile("input.bin", 0);
			break;

		case 'M':
			break;

		case 'Q':
			running = 0;
			break;

		default:
			printf("Unrecognized command\n");


		}

	}


}




/*
	M25P10A:

	M25P10A have 1 Mb flash memory, 131.072 (0x20000) bytes.
	Flash memory is divided in 4 sectors, each 0x8000 bytes, that can be individually erased (setting all bits to 1).
	Page size is 256 bytes. Only one page can be programmed in each iteration (setting individual bits to 0).
	Addresses is given with 3 bytes.


*/

void processM25P10(FT_HANDLE ftHandleInput)
{
	ftHandle = ftHandleInput;

	//debugDeviceType();

	printf("Verifying device M25P10-compatibility...\n");
	if (!verifyDeviceM25P10Compatibility())
	{
		printf("Failure, device is not M25P10-compatible\n");
		return;
	}
	printf("Success, connected to M25P10-compatible device");

	int running = 1;
	while (running)
	{
		printf("\n\nMenu:\n");
		printf("R: Read entire flash to \"output.bin\"\n");
		printf("W: Write to flash from \"input.bin\" (Bulk erase + write + verify)\n");
		printf("A: Advanced menu\n");
		printf("M: Print menu\n");
		printf("Q: Quit\n\n");

		scanf("%s", buffer);

		char cmd = buffer[0];
		if (cmd >= 'a' && cmd <= 'z') cmd = cmd - 'a' + 'A'; //UPPERCASE

		switch (cmd) {
		case 'R':
			dumpData(0, 0x20000);
			break;

		case 'W':
			writeEnable();

			bulkErase();

			programFromFile("input.bin", 0);

			//TODO verify
			//dumpData(0, 0x20000);

			verifyFile("input.bin", 0);


			break;

		case 'A':
			advancedMenu();
			break;

		case 'M':
			break;

		case 'Q':
			running = 0;
			break;

		default:
			printf("Unrecognized command\n");


		}


	}


}



