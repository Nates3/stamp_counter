

#include <stdio.h>
#include <windows.h>
#include "types.h"

#define internal static
#define global_variable static
#define local_persist static

#define ArrayCount(array) (sizeof(array) / sizeof(array[0]))
#define Assert(expression) if(!(expression)) { *(int *)0 = 0; }
#define InvalidCodePath Assert(!"Invalid Code Path")


#define FILE_START_COUNTER_NAME "timer_start_counter.timeinfo"


internal LARGE_INTEGER
Win32GetTime(void)
{
  LARGE_INTEGER Result;
  QueryPerformanceCounter(&Result);
  return(Result);
}

internal u64
Win32GetCounterDifference(LARGE_INTEGER A, LARGE_INTEGER B)
{
  u64 Result = A.QuadPart - B.QuadPart;
  return(Result);
}

internal b32
FileExists(char *FileName)
{
  b32 Result = false;
  DWORD FileAttributes = GetFileAttributesA(FileName);
  Result = (FileAttributes != INVALID_FILE_ATTRIBUTES);
  return(Result);
}

internal b32
SameText(char *A, char *B)
{
  b32 Result = true;
  while(*A++ && *B++)
  {
    if(*A != *B)
    {
      Result = false;
    }
  }
  
  if((*A != 0) || (*B != 0))
  {
    Result = false;
  }
  
  return(Result);
}

internal void *
Win32ReadEntireFile(HANDLE File)
{
  void *Result = 0;
  if(File)
  {
    DWORD FileSize = GetFileSize(File, 0);
    if(FileSize)
    {
      Result = VirtualAlloc(0, FileSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
      
      DWORD IGNORED;
      ReadFile(File, Result, FileSize, &IGNORED, 0);
    }
  }
  else
  {
    InvalidCodePath;
  }
  
  return(Result);
}

internal void
Win32WriteEntireFile(HANDLE File, void *Memory, u32 ByteCount)
{
  if(File)
  {
    DWORD IGNORED;
    WriteFile(File, Memory, ByteCount, &IGNORED, 0);
  }
  else
  {
    InvalidCodePath;
  }
}

int main(int ArgC, char **Args)
{
  LARGE_INTEGER LargeFrequency;
  QueryPerformanceFrequency(&LargeFrequency);
  u64 Frequency = LargeFrequency.QuadPart;
  
  if(ArgC == 2)
  {
    char *Option = Args[1];
    if(SameText(Option, "-Start"))
    {
      LARGE_INTEGER StartCounter = Win32GetTime();
      
      HANDLE File = CreateFileA(FILE_START_COUNTER_NAME,
                                GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
      if(File != INVALID_HANDLE_VALUE)
      {
        Win32WriteEntireFile(File, &StartCounter.QuadPart, sizeof(StartCounter.QuadPart));
      }
      else
      {
        fprintf(stderr, "Start Counter File Couldn't be created for writing\n");
      }
      
      CloseHandle(File);
    }
    else if(SameText(Option, "-End"))
    {
      HANDLE File = CreateFileA(FILE_START_COUNTER_NAME,
                                GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
      if(File != INVALID_HANDLE_VALUE)
      {
        void *FileMemory = Win32ReadEntireFile(File);
        u64 StartCounter = *(u64 *)FileMemory;
        LARGE_INTEGER EndCounter = Win32GetTime();
        
        u64 CycleDiff = EndCounter.QuadPart - StartCounter;
        r64 MilliSeconds = ((r64)CycleDiff / (r64)Frequency)*1000.0f;
        
        fprintf(stdout, "%.02f\n", MilliSeconds);
        
        CloseHandle(File);
        DeleteFile(FILE_START_COUNTER_NAME);
      }
      else
      {
        fprintf(stderr, "Start Counter File Doesn't exist\n");
      }
    }
    else
    {
      fprintf(stderr, "Unrecognized Option: %s\n\n", Option);
      goto USAGE;
    }
  }
  else
  {
    USAGE:
    fprintf(stdout, "Place stamp_timer.exe into wherever command prompt can call it\n");
    fprintf(stdout, "Usage: stamp_timer -OPTION\n");
    fprintf(stdout, "OPTIONS:\n");
    fprintf(stdout, "-Start(Stamps Performance Counter into file)\n");
    fprintf(stdout, "-End(Reads that file and gets difference)\n");
  }
  
  
  return(0);
}