#include<iostream>
#include<windows.h>
#include<tchar.h>
#include <io.h>   // For access().
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <string>
using namespace std;

int main()
{
    GetConsoleWindow();
       string strPath = "./checkSC.ps1";
//access function:
       //The function returns 0 if the file has the given mode.
       //The function returns –1 if the named file does not exist or does not have the given mode
       if(access(strPath.c_str(),0) == 0)
       {
              system("powershell.exe -ExecutionPolicy Bypass -File ./checkSC.ps1");
              system("pause");
       }
       else
       {
              system("cls");
              cout << "File is not exist";
              system("pause");
       }
    return 0;
}