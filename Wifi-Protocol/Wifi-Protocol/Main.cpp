/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Main.cpp
--
-- PROGRAM: Wireless-GRVM
--
-- FUNCTIONS:
-- int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
-- BOOL Register(HINSTANCE)
-- HWND Create(HINSTANCE, int)
-- LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM)
-- BOOL Window_OnCreate(HWND)
-- void Window_OnCommand (HWND, int, HWND, UINT)
-- void Window_OnDestroy (HWND);
-- BOOL CALLBACK AboutDlgProc (HWND, UINT, WPARAM, LPARAM);
-- void OpenFileInitialize(HWND )
-- BOOL FileOpenDlg(HWND, PTSTR, PTSTR)
-- BOOL FileRead(HWND, PTSTR)
-- void OkMessage(HWND, TCHAR*, TCHAR*)
-- void DisplayText(HWND, LPCSTR)
-- void Window_OnVScroll(HWND, HWND, UINT, int)
--
-- DATE: November 12, 2013
--
-- REVISIONS:  
-- November 12, 2013 - Mat Siwoski: Added Window_OnCreate, OpenFileInitialize, FileOpenDlg & FileRead
-- November 23, 2013 - Mat Siwoski: Added DisplayText, Window_OnVScroll & custom Icon
-- November 24, 2013 - Mat Siwoski: Added Window_OnPaint, Window_OnSize
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- NOTES:
-- Main GUI for the program.   
--
----------------------------------------------------------------------------------------------------------------------*/

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "Main.h"
#include "Resource.h"

static HWND MainWindow;
static char szAppName[]		= "Windows Protocol";
static HINSTANCE hInstance;
static OPENFILENAME ofn;
BOOL bWantToRead			= FALSE;
HANDLE hACKWaitSemaphore	= INVALID_HANDLE_VALUE;
HANDLE hFileWaitSemaphore	= INVALID_HANDLE_VALUE;
HANDLE hReceiveThread		= INVALID_HANDLE_VALUE;
HANDLE hTransmitThread		= INVALID_HANDLE_VALUE;
char szFile[260];				// buffer for file name
HANDLE hf;						// file handle
HANDLE hComm;
COMMCONFIG cc;
LPSTR pszFileText;
SCROLLINFO  si ;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WinMain
--
-- DATE: November 12, 2013
--
-- REVISIONS:
-- November 25, 2013 - Vincent Lau: Added Semaphore creation and error checking
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
	

	if (!hPrevInstance){
		if (!Register(hInst)){
			return FALSE;
		}
	}
	MainWindow = Create (hInst, nCmdShow);
	if (!MainWindow){
		return FALSE;
	}
	ShowWindow (MainWindow, nCmdShow);
	UpdateWindow (MainWindow);
	cc.dwSize			= sizeof(COMMCONFIG);
	cc.wVersion			= 0x100;
	
	// Non-Window related inits
	hComm = 0;
	hACKWaitSemaphore	= CreateSemaphore(NULL, 0, 1, NULL);
	hFileWaitSemaphore	= CreateSemaphore(NULL, 0, 1, NULL);

	if (hACKWaitSemaphore == NULL || hACKWaitSemaphore == INVALID_HANDLE_VALUE ||
		hFileWaitSemaphore == NULL || hFileWaitSemaphore == INVALID_HANDLE_VALUE)
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
-- November 23, 2013 - Mateusz Siwoski: Added Icon to program
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
	W.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
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
-- November 23, 2013 - Mateusz Siwoski: Added scroll bars to window
-- November 25, 2013 - Mateusz Siwoski: Corrected Scroll Bars
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
	//hInstance = hInst;

	HWND hwnd = CreateWindow (szAppName, szAppName, WS_OVERLAPPEDWINDOW |  WS_VSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInst, NULL);

	if (hwnd == NULL)
		return hwnd;
	nCmdShow = SW_SHOW;

	// Disables the Disconnect button at start of window
	EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_DISABLED);

	return hwnd;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WndProc
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
-- November 23, 2013 - Mat Siwoski: Added Window_OnVScroll
-- November 24, 2013 - Mat Siwoski: Added Window_OnPaint, Window_OnSize
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
	static HINSTANCE hInst ;
	switch (Message){
		HANDLE_MSG(hwnd, WM_CREATE, Window_OnCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, Window_OnCommand);
		HANDLE_MSG(hwnd, WM_VSCROLL, Window_OnVScroll);
		HANDLE_MSG(hwnd, WM_SIZE, Window_OnSize);
		HANDLE_MSG(hwnd, WM_PAINT, Window_OnPaint);
		HANDLE_MSG(hwnd, WM_DESTROY, Window_OnDestroy);
	default:
		return DefWindowProc (hwnd, Message, wParam, lParam);
	}
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Window_OnSize
--
-- DATE: November 12, 2013
--
-- REVISIONS: 
-- November 23, 2013 - Mat Siwoski: Added Window_OnVScroll
-- November 24, 2013 - Mat Siwoski: Added Window_OnPaint
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: void Window_OnSize(HWND hwnd, UINT state, int cx, int cy)
--				HWND hwnd: Handle to the window
--				UINT state: The message
--				int cx: Parameter for lparam
--				int cy: Parameter for lparam 
--
-- RETURNS: 
--
-- NOTES:
-- This function that handles the dimensions for the window size.
------------------------------------------------------------------------------------------------------------------*/
void Window_OnSize(HWND hwnd, UINT state, int cx, int cy){
	RECT drawingArea;

	GetClientRect(hwnd, &drawingArea);

	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	//********************************************THIS IS MY ISSUE******************************************************//
	si.nMax = cy*12;
	si.nPage = 30;
	si.nPos = 0;
	
	SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Window_OnPaint
--
-- DATE: November 23, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: void Window_OnPaint(HWND hwnd)
--				HWND hwnd: Handle to the window
--
-- RETURNS: -
--
-- NOTES:
-- This function repaints the window (specifically when scrollbar is pressed)
------------------------------------------------------------------------------------------------------------------*/
void Window_OnPaint(HWND hwnd){
	PAINTSTRUCT ps;
	int iVertPos, iHorzPos;
	RECT drawingArea;
	HDC hdc = BeginPaint (hwnd, &ps) ;
	
	GetClientRect(hwnd, &drawingArea);
    si.cbSize = sizeof (si) ;
    si.fMask  = SIF_POS ;
    GetScrollInfo (hwnd, SB_VERT, &si) ;
    iVertPos = si.nPos ;
	GetScrollInfo (hwnd, SB_HORZ, &si) ;
    iHorzPos = si.nPos ;
	drawingArea.top -= iVertPos;
	if (pszFileText != NULL){
		DrawText (hdc, pszFileText, -1, &drawingArea, DT_EXPANDTABS | DT_WORDBREAK) ;
	}
	UpdateWindow(hwnd);
	EndPaint(hwnd, &ps);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Window_OnVScroll
--
-- DATE: November 23, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: void Window_OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
--				HWND hwnd: Handle to the window
--				HWND hwndCtl: Handle to the control
--				UINT code: Code
--				int pos: Position
--
-- RETURNS: -
--
-- NOTES:
-- This function handles the Scrolling for the window.
------------------------------------------------------------------------------------------------------------------*/
void Window_OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos){
	int iVertPos;
	si.cbSize = sizeof (si) ;
	si.fMask  = SIF_ALL ;
	GetScrollInfo (hwnd, SB_VERT, &si) ;
	iVertPos = si.nPos ;
	
	switch (code)
	{
		case SB_TOP:
			si.nPos = si.nMin ;
		break ;
		case SB_BOTTOM:
			si.nPos = si.nMax ;
		break ;
		case SB_LINEUP:
			si.nPos -= 1 ;
		break ;
		case SB_LINEDOWN:
			si.nPos += 1 ;
		break ;
		case SB_PAGEUP:
			si.nPos -= si.nPage ;
		break ;
		case SB_PAGEDOWN:
			si.nPos += si.nPage ;
			break ;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos ;
			break ;
		default:
			break ;         
	}
	si.fMask = SIF_POS ;
    SetScrollInfo (hwnd, SB_VERT, &si, TRUE) ;
    GetScrollInfo (hwnd, SB_VERT, &si) ;

    // If the position has changed, scroll the window and update it
	//GetClientRect(hwnd, &drawingArea);
    if (si.nPos != iVertPos)
    {          
        ScrollWindow (hwnd, 0, (iVertPos - si.nPos), 
                            NULL, NULL) ;
		//InvalidateRect(hwnd, &drawingArea, TRUE);
        UpdateWindow (hwnd) ;
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
BOOL Window_OnCreate(HWND hwnd, LPCREATESTRUCT strct){
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
-- November 25, 2013 - Vincent Lau: Added creation of the Transmit thread after successful file read operation
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
	
	switch(id){
		case IDM_CONNECT:
				EnableMenuItem(GetMenu(hwnd), IDM_CONNECT, MF_DISABLED);
				EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_ENABLED);
				DrawMenuBar(hwnd);
			break;
		case IDM_SENDFILE:
			DWORD dwTransmitThreadID;
			if (FileOpenDlg(hwnd, szFileName, szTitleName)){
				if (!FileRead(hwnd, szFileName)){ // error file could not be read
                         OkMessage (hwnd, TEXT ("Could not read file %s!"),
                                    szTitleName) ;
                         szFileName[0]  = '\0' ;
                         szTitleName[0] = '\0' ;
                }
				else // success file read
				{
					// Clean up thread
					TerminateThread(hTransmitThread, 0);
					CloseHandle(hTransmitThread);
					ReleaseSemaphore(hFileWaitSemaphore, 1, NULL);
					// create transmit thread for this file
					hTransmitThread = CreateThread(NULL, 0, TransmitThread, pszFileText, 0, &dwTransmitThreadID);
				}
			}
			break;
		case IDM_DISCONNECT:
			EnableMenuItem(GetMenu(hwnd), IDM_CONNECT, MF_ENABLED);
			EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_DISABLED);
			DrawMenuBar(hwnd);
			break;
		case IDM_CONFIG:
			DWORD dwReceiveThreadID;
			TerminateThread(hReceiveThread, 0);
			CloseHandle(hReceiveThread);
			if(SetupPort(lpszCommName)){
				if (ConfPort(&MainWindow, lpszCommName)){
					// Set Read flag true
					bWantToRead		= TRUE;
					hReceiveThread	= CreateThread(NULL, 0, ReceiveThread, &MainWindow, 0, &dwReceiveThreadID);
					break;
				}
			}
			break;
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
-- REVISIONS: 
-- November 21, 2013 - Vincent Lau: Added cleanup
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
	TerminateThread(hReceiveThread, EXIT_SUCCESS);
	CloseHandle(hReceiveThread);
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
			switch (LOWORD (wParam)){
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
	ofn.lStructSize			= sizeof (OPENFILENAME) ;
    ofn.hwndOwner			= hwnd ;
    ofn.hInstance			= NULL ;
    ofn.lpstrFilter			= szFilter ;
    ofn.lpstrCustomFilter	= NULL ;
    ofn.nMaxCustFilter		= 0 ;
    ofn.nFilterIndex		= 0 ;
    ofn.lpstrFile			= NULL ;          // Set in Open and Close functions
    ofn.nMaxFile			= MAX_PATH ;
    ofn.lpstrFileTitle		= NULL ;          // Set in Open and Close functions
    ofn.nMaxFileTitle		= MAX_PATH ;
    ofn.lpstrInitialDir		= NULL ;
    ofn.lpstrTitle			= NULL ;
    ofn.Flags				= 0 ;             // Set in Open and Close functions
    ofn.nFileOffset			= 0 ;
    ofn.nFileExtension		= 0 ;
    ofn.lpstrDefExt			= TEXT ("txt") ;
    ofn.lCustData			= 0L ;
    ofn.lpfnHook			= NULL ;
    ofn.lpTemplateName		= NULL ;
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
    ofn.hwndOwner	= hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile	= pstrFileName;
    ofn.nMaxFile	= MAX_PATH;
    ofn.Flags		= OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
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
BOOL FileRead(HWND hwnd, LPCSTR pstrFileName){

	HANDLE hFile;
    BOOL bSuccess = FALSE;
	
    hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
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
	//DISPLAY TEXT (THIS WILL NEED TO GO IN DISPLAY FUNCTION AFTER READING A FILE
	DisplayText(hwnd, pszFileText);
	
    return bSuccess;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: DisplayText
--
-- DATE: November 23, 2013
--
-- REVISIONS: 
--
-- DESIGNER: Mat Siwoski
--
-- PROGRAMMER: Mat Siwoski
--
-- INTERFACE: void DisplayText(HWND hwnd, LPCSTR text)
--				HWND hwnd: Handle to the window
--				LPCSTR text: Pointer to the file text
--
-- RETURNS: -
--
-- NOTES:
-- This function displays the formatted text.
------------------------------------------------------------------------------------------------------------------*/
void DisplayText(HWND hwnd, LPCSTR text){
	HDC hdc;
	RECT drawingArea;
	
	GetClientRect(hwnd, &drawingArea);
	hdc = GetDC(hwnd);
	DrawText(hdc, pszFileText, -1, &drawingArea, DT_WORDBREAK);
	ReleaseDC(hwnd, hdc);
}
