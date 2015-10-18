#include "menu.h"

FileEntry FileList[1024];
u32 fileCounter = 0;
volatile bool isMenu = false;

void printText(int col, int x, int y, const char* fmt, ...)
{
	char temp[256];
	u16* mapBase = (u16*)BG_MAP_RAM_SUB(31);
		
	va_list marker;
	va_start(marker,fmt);
	vsprintf(temp,fmt,marker);
	va_end(marker);
		
	int len = strlen(temp);
	int posX = x;
	int posY = y;
	int textPos = 0;
		
	while(len--) {
		if(temp[textPos] == '\n') {
			++posY;
			posX = x;
		} else {	
			mapBase[posX + posY*32] = (temp[textPos] - 32) | col << 12;
			++posX;
		}
		++textPos;
	}
}

void fillList()
{
	textClear();
	int curFile = 0;
		
	FileList[curFile].fType = FAT_FindFirstFileLFN(FileList[curFile].fName);
		
	while(FileList[curFile].fType != FT_NONE) {
		++curFile;
		FileList[curFile].fType  = FAT_FindNextFileLFN(FileList[curFile].fName);
		FileList[curFile].isRunable = !strcasestr(FileList[curFile].fName, ".wsc") || !strcasestr(FileList[curFile].fName, ".ws");
	}
		
	fileCounter = curFile - 1;
}

void printList(u32 startPos)
{
	u32 i = startPos;
		
	textClear();
	printText(0, 1, 1, "dualSwan v1.2 by LiraNuna\nDetected: %s", (const char*)&active_interface->ul_ioType);
		
	for(; i < startPos + 18; ++i) {
		if(FileList[i].fType == FT_DIR)
			printText(0, 5, 4 + (i-startPos), "<%s>", FileList[i].fName);
		else if(FileList[i].fType == FT_FILE) {
			if(FileList[i].isRunable)
				printText(1, 5, 4 + (i-startPos), "%s", FileList[i].fName);
			else
				printText(0, 5, 4 + (i-startPos), "%s", FileList[i].fName);
		} else
			break;
	}
}

void handleFileMenu()
{
	bool isSelected = false;
	s32 curFile = 0;
	int virtualFile = 0;
		
	fillList();
	while(!isSelected) {
		scanKeys();
		u16 KEYS = keysDown();
		printList(curFile);
		printText(0, 2, virtualFile + 4, "->");
			
		if(curFile)
			printText(1, 1, 3, "^\n|");
			
		if((curFile + 18 != fileCounter) && fileCounter > 19)
			printText(1, 1, 20, "|\nv");
			
		if(KEYS & KEY_RIGHT) {
			virtualFile += 5;
			if(virtualFile > 18)
				curFile += virtualFile - 18;
		}
			
		if(KEYS & KEY_LEFT) {
			virtualFile -= 5;
			if(virtualFile < 0)
				curFile += virtualFile;
		}
			
		if(KEYS & KEY_DOWN)
			++virtualFile;
			
		if(KEYS & KEY_UP)
			--virtualFile;
			
		if(KEYS & KEY_A) {
			if(FileList[curFile + virtualFile].fType == FT_DIR) {
				FAT_chdir(FileList[curFile + virtualFile].fName);
				virtualFile = curFile = 0;
				fillList();
			} else if(FileList[curFile + virtualFile].isRunable) {
				textClear();
				printText(0, 1, 1, "Loading...\n\n%s", FileList[curFile + virtualFile].fName);
				wsInit(FileList[curFile + virtualFile].fName);
				textClear();
				break;
			}
		}
		
		if(virtualFile < 0) {
			virtualFile = 0;
			--curFile;
			if(curFile < 0)
				curFile = 0;
		}
			
		if(fileCounter < 19) {
			if(virtualFile > fileCounter)
				virtualFile = fileCounter;
		} else {
			if(virtualFile > 18) {
				virtualFile = 18;
				++curFile;
				if(curFile + 18 > fileCounter)
					curFile = fileCounter - 18;
			}
		}
			
		swiWaitForVBlank();
	}
}

void handleInGameMenu()
{
	// To Do: Fill ingame manu
}