#ifndef MAIN_H
#define MAIN_H

#include "Includes.h"
#include "Session.h"
#include "Transport.h"

#define UNTITLED TEXT ("(untitled)")
#define EDITID   1

// Function Prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL Register(HINSTANCE);
HWND Create(HINSTANCE, int);
BOOL Window_OnCreate(HWND, LPARAM);
void Window_OnCommand (HWND, int, HWND, UINT);
void Window_OnDestroy (HWND);
BOOL CALLBACK AboutDlgProc (HWND, UINT, WPARAM, LPARAM);
void OpenFileInitialize(HWND);
BOOL FileOpenDlg (HWND, PTSTR, LPCSTR);
BOOL FileRead(HWND, LPCSTR);
void OkMessage(HWND, TCHAR*, TCHAR*);
BOOL ErrorCheck(char);
void DisplayText(HWND, LPCSTR);

// Variables
const LPTSTR lpszCommName	= TEXT("COM1");




#endif
