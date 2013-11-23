/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Main.cpp
--
-- PROGRAM: Wireless-MRGV
--
-- FUNCTIONS:
-- int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lspszCmdParam, int nCmdShow)
-- BOOL Register(HINSTANCE hInst)
-- HWND Create(HINSTANCE hInst, int nCmdShow)
-- LRESULT CALLBACK WndProc (HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
-- BOOL Window_OnCreate(HWND hwnd)
-- void Window_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
-- void Window_OnDestroy (HWND);
-- BOOL CALLBACK AboutDlgProc (HWND, UINT, WPARAM, LPARAM);
-- void OpenFileInitialize(HWND )
-- BOOL FileOpenDlg(HWND, PTSTR, PTSTR)
-- BOOL FileRead(HWND, PTSTR)
-- void OkMessage(HWND, TCHAR*, TCHAR*)
--
--
-- DATE: November 12, 2013
--
-- REVISIONS:  
-- November 12, 2013 - Mat Siwoski: Added Window_OnCreate, OpenFileInitialize, FileOpenDlg & FileRead
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- NOTES:
--    
--
----------------------------------------------------------------------------------------------------------------------*/

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "Main.h"
#include "Resource.h"

static HWND MainWindow;
static char szAppName[] = "Windows Protocol";
static HINSTANCE hInstance;
static OPENFILENAME ofn;
static bool bWantToRead = false;
static HANDLE hACKWaitSemaphore = INVALID_HANDLE_VALUE;
static HANDLE hReceiveThread	= INVALID_HANDLE_VALUE;
char szFile[260];				// buffer for file name
HANDLE hf;						// file handle
HANDLE hComm;
COMMCONFIG cc;
LPSTR pszFileText;

	/*------------------------------------------------------------------------------------------------------------------
	-- FUNCTION: WinMain
	--
	-- DATE: November 12, 2013
	--
	-- REVISIONS: 
	--
	-- DESIGNER: Mat Siwoski
	--
	-- PROGRAMMER: Mat Siwoski
	--
	-- INTERFACE: int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lspszCmdParam, int nCmdShow)
	--				HINSTANCE hInst: Handle to the current instance of the program.
	--				HINSTANCE hPrevInstance: Handle to the previous instance of the program.
	--				LPSTR lspszCmdParam: Command line for the application.
	--				int nCmdShow: Control for how the window should be shown.
	--
	-- RETURNS: Returns the exit value upon exit.
	--
	-- NOTES:
	-- This function is the entry point for a graphical Windows-based application.
	------------------------------------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;
	DWORD dwReceiveThreadID;

	if (!hPrevInstance){
		if (!Register(hInst)){
			return FALSE;
		}
	}
	MainWindow = Create (hInst, nCmdShow);
	if (!MainWindow){
		return FALSE;
	}
	cc.dwSize = sizeof(COMMCONFIG);
	cc.wVersion = 0x100;
	
	// Non-Window related inits
	hComm = 0;
	hACKWaitSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
	hReceiveThread	  = CreateThread(NULL, 0, ReceiveThread, &MainWindow, 0, &dwReceiveThreadID);

	if(hACKWaitSemaphore == NULL || hACKWaitSemaphore == INVALID_HANDLE_VALUE)
	{
		MessageBox(MainWindow, TEXT("Couldn't acquire semaphores"), TEXT("Creation error"), MB_OK);
		return EXIT_FAILURE;
	}

	while (GetMessage (&Msg, NULL, 0, 0))
	{
		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}
	return Msg.wParam;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Register
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: BOOL Register(HINSTANCE hInst)
--				HINSTANCE hInst: Handle to the window
--
-- RETURNS: Returns true if the registration has passed, otherwise, false
--
-- NOTES:
-- This function registers the parameters for the intial window of the main program. 
------------------------------------------------------------------------------------------------------------------*/
BOOL Register(HINSTANCE hInst)
{
	WNDCLASS W;

	memset (&W, 0, sizeof(WNDCLASS));
	W.style			= CS_HREDRAW | CS_VREDRAW;
	W.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	W.hCursor		= LoadCursor (NULL, IDC_ARROW);
	W.lpfnWndProc	= WndProc;
	W.hInstance		= hInst;
	W.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	W.lpszClassName = szAppName;
	W.cbClsExtra	= 0;
	W.cbWndExtra	= 0;
	W.lpszMenuName	= TEXT("MYMENU");

	return (RegisterClass (&W) != 0);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Create
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: HWND Create(HINSTANCE hInst, int nCmdShow)
--				HINSTANCE hInst: A handle to the window.
--				int nCmdShow: Control how the window should be shown.
--
-- RETURNS: Returns the handle to the window.
--
-- NOTES:
-- This function creates the main window of the application.
------------------------------------------------------------------------------------------------------------------*/
HWND Create(HINSTANCE hInst, int nCmdShow)
{
	hInstance = hInst;

	HWND hwnd = CreateWindow (szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInst, NULL);

	if (hwnd == NULL)
		return hwnd;
	nCmdShow = SW_SHOW;

	// Disalbes the Disconnect button at start of window
	EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_DISABLED);

	ShowWindow (hwnd, nCmdShow);
	UpdateWindow (hwnd);

	return hwnd;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WndProc
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: LRESULT CALLBACK WndProc (HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
--				HWND hwnd: Handle to the window
--				UINT Message: The message
--				WPARAM wParam: Parameter
--				LPARAM lParam: Parameter
--
-- RETURNS: 
--
-- NOTES:
-- This function that handles the different type of window messages.
------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
	switch (Message){
		HANDLE_MSG (hwnd, WM_COMMAND, Window_OnCommand);
		HANDLE_MSG (hwnd, WM_DESTROY, Window_OnDestroy);
	default:
		return DefWindowProc (hwnd, Message, wParam, lParam);
	}
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Window_OnCreate
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: BOOL Window_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
--				HWND hwnd: Handle to the window
--				
-- RETURNS: -
--
-- NOTES:
-- This function deals with the selection in the menu on the main window. 
------------------------------------------------------------------------------------------------------------------*/
BOOL Window_OnCreate(HWND hwnd){
	OpenFileInitialize(hwnd);
	
	return TRUE;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Window_OnCommand
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
-- November 13, 2013 - Mat Siwoski: Completed implementation of the SendFile
-- November 13, 2013 - Mat Siwoski: Implementation of the Config option.
-- November 18, 2013 - Robin Hsieh: Added the enabling and disabling menu items.
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: void Window_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
--				HWND hwnd: Handle to the window
--				int id: The id of the selection.
--				HWND hwndCtl: Handle to the control.
--				UINT codeNotify: Int for the Notification
--
-- RETURNS: -
--
-- NOTES:
-- This function deals with the selection in the menu on the main window. 
------------------------------------------------------------------------------------------------------------------*/
void Window_OnCommand (HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH] ;
	static HWND hwndEdit ;
	switch(id){
		case IDM_CONNECT:
				EnableMenuItem(GetMenu(hwnd), IDM_CONNECT, MF_DISABLED);
				EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_ENABLED);
				DrawMenuBar(hwnd);
			break;
		case IDM_SENDFILE:
			if (FileOpenDlg(hwnd, szFileName, szTitleName)){
				
				if (!FileRead(hwndEdit, szFileName))
                    {
                         OkMessage (hwnd, TEXT ("Could not read file %s!"),
                                    szTitleName) ;
                         szFileName[0]  = '\0' ;
                         szTitleName[0] = '\0' ;
                    }
			}
			break;
		case IDM_DISCONNECT:
			EnableMenuItem(GetMenu(hwnd), IDM_CONNECT, MF_ENABLED);
			EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_DISABLED);
			DrawMenuBar(hwnd);
			break;
		case IDM_CONFIG:
			
			if(SetupPort(lpszCommName))
			{
				if (ConfPort(&MainWindow, lpszCommName))
				{
					// Set Read flag true
					bWantToRead = true;
					break;
				}
				
			}
			break;
			/*GetCommConfig(hComm, &cc, &cc.dwSize);
			if (!CommConfigDialog (lpszCommName, hwnd, &cc)) 
				break;*/ 
		case IDM_ABOUT:
			DialogBox (hInstance, TEXT ("AboutBox"), hwnd, AboutDlgProc) ;
			break;
		case IDM_EXIT:	
			Window_OnDestroy(hwnd);
			break;
	}
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Window_OnDestroy
--
-- DATE: November 12, 2013
--
-- REVISIONS: 2013/11/21 - Vincent - Added cleanup
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: void Window_OnDestroy (HWND hwnd)
--				HWND hwnd: Handle for the window
--				
-- RETURNS: -
--
-- NOTES:
-- This function cleans up handles and shuts down the program.
------------------------------------------------------------------------------------------------------------------*/
void Window_OnDestroy (HWND hwnd){
	CloseHandle(hACKWaitSemaphore);
	CloseHandle(hComm);
	PostQuitMessage(0);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: AboutDlgProc
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
--				HWND hDlg: Handle for the dialog box
--				UINT message: Message. 
--				WPARAM wParam: Parameter.
--				LPARAM lParam: Parameter.
--
-- RETURNS: Returns True upon closing the dialog box, else false
--
-- NOTES:
-- This function creates an About Dialog Box displaying information about the program. 
------------------------------------------------------------------------------------------------------------------*/
BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD (wParam))
			{
				case ID_OK:{
					EndDialog (hDlg, 0) ;
					return TRUE ;
				}
			}
			break;
	}
	return FALSE ;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: OpenFileInitialize
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: void OpenFileInitialize(HWND hwnd)
--				HWND hwnd: Handle to the window
--
-- RETURNS: -
--
-- NOTES:
-- This function initializes the parameters that are going to be used to for opening a file.
------------------------------------------------------------------------------------------------------------------*/
void OpenFileInitialize(HWND hwnd){
	static TCHAR szFilter[] = TEXT ("Text Files (*.TXT)\0*.txt\0");
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
    ofn.hwndOwner         = hwnd ;
    ofn.hInstance         = NULL ;
    ofn.lpstrFilter       = szFilter ;
    ofn.lpstrCustomFilter = NULL ;
    ofn.nMaxCustFilter    = 0 ;
    ofn.nFilterIndex      = 0 ;
    ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
    ofn.nMaxFile          = MAX_PATH ;
    ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
    ofn.nMaxFileTitle     = MAX_PATH ;
    ofn.lpstrInitialDir   = NULL ;
    ofn.lpstrTitle        = NULL ;
    ofn.Flags             = 0 ;             // Set in Open and Close functions
    ofn.nFileOffset       = 0 ;
    ofn.nFileExtension    = 0 ;
    ofn.lpstrDefExt       = TEXT ("txt") ;
    ofn.lCustData         = 0L ;
    ofn.lpfnHook          = NULL ;
    ofn.lpTemplateName    = NULL ;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: FileOpenDlg
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: BOOL PopFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
--				HWND hwnd: Handle to the window
--				PTSTR pstrFileName: Pointer to the name of the file
--				PTSTR pstrTitleName: Pointer to the title.
--
-- RETURNS: Returns true if able to get a file back after selecting.
--
-- NOTES:
-- This function initializes the parameters that are going to be used to for opening a file.
------------------------------------------------------------------------------------------------------------------*/
BOOL FileOpenDlg(HWND hwnd, PTSTR pstrFileName, LPCSTR pstrTitleName){
	OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = pstrFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    return(GetOpenFileName(&ofn));
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: OkMessage
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: void OkMessage(HWND hwnd, TCHAR* szMessage, TCHAR* szTitleName)
--				HWND hwnd: Handle to the window
--				TCHAR* szMessage: The name of the message
--				TCHAR* szTitleName: The title of the file.
--
-- RETURNS: -
--
-- NOTES:
-- This function displays an error window if the read file does not work
------------------------------------------------------------------------------------------------------------------*/
void OkMessage(HWND hwnd, TCHAR* szMessage, TCHAR* szTitleName)
{
     TCHAR szBuffer[64 + MAX_PATH] ;
     
     wsprintf (szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED) ;
     
     MessageBox (hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION) ;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: FileRead
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: BOOL FileRead(HWND hwndEdit, LPCSTR pstrFileName)
--				HWND hwndEdit: Handle to the file selection
--				LPCSTR pstrFileName: Pointer to the file name
--
-- RETURNS: Returns true if able to successfully read the file.
--
-- NOTES:
-- This function proceeds the FileOpen function and is used to read in the file.
------------------------------------------------------------------------------------------------------------------*/
BOOL FileRead(HWND hwndEdit, LPCSTR pstrFileName){

	HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwFileSize;
        dwFileSize = GetFileSize(hFile, NULL);
		
        if(dwFileSize  != 0xFFFFFFFF)
        {
            
            pszFileText = (LPSTR) malloc(dwFileSize + 2);
            if(pszFileText != NULL)
            {
                DWORD dwRead;
                if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
                {
					pszFileText[dwFileSize] = '\0' ;
					pszFileText[dwFileSize + 1] = '\0' ;
					bSuccess = TRUE;
                }
               //free(pszFileText); //not sure if this is needed here or not as i think this frees the memory (i.e. the stuff we read)
            }
        }
       CloseHandle(hFile);
    }
    return bSuccess;
}