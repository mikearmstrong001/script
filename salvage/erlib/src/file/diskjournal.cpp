#include "../erbase.h"

/*
* Copyright (c) 2007, Insomniac Games
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY INSOMNIAC GAMES ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL INSOMNIAC GAMES BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
 
// must be using WIN32 5.0 APIS otherwise we have to use the DDK
#define _WIN32_WINNT  0x0500
#include <windows.h>
#include <winioctl.h>  // used instead of the DDK
#include <stdio.h>
#include <time.h>
 
 
void diskjournal()
{
  USN_JOURNAL_DATA journal_data;                                    // will hold the top level journal info
  READ_USN_JOURNAL_DATA read_data = {0, 0xFFFFFFFF, FALSE, 0, 0};   // what do we want to query (everything starting at the start)
  USN_RECORD* usn_record;  
  HANDLE volume;
  DWORD bytes,returned_bytes;
  char buffer[128*1024];     // buffer to hold the results of the journal query
  
   // open c: as a device, any drive can be opened but it has to be a physical drive, no hardlinks or sub partitions.
   // The drive must be an NTFS drive and you must be running windows 2000 or later
   // ### BE CAREFUL NOT TO CALL WRITEFILE() ON THE HANDLE AS IT WILL PERFORM SECTOR WRITES DIRECTLY TO THE PARTITION###
   volume = CreateFile( TEXT("\\\\.\\c:"), 
               GENERIC_READ | GENERIC_WRITE,  FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
 
   if( volume == INVALID_HANDLE_VALUE )
   {
      printf("CreateFile() failed\n");
      return;
   }
 
   //  synchronous driver call to query the device for the journal control structure, all we need from this is the Journal ID value.
   // The structure contains some misc info regarding the size of the journal, the free space in the journal etc
   if( !DeviceIoControl( volume, FSCTL_QUERY_USN_JOURNAL, NULL, 0,
          &journal_data,  sizeof(journal_data), &bytes, NULL) )
   {
      printf( "Query journal failed\n");
      return;
   }
 
   // The journal data structure contains the ID of the next journal entry, if you want to record all disk activity
   // from now forwards you would start at the USN in the journal. You can get all disk changes from a moment in time
   // if you know the base USN which is just a 64bit number
   printf( "Journal ID:              0x%I64x\n", journal_data.UsnJournalID );
   printf( "First USN:               0x%I64x\n", journal_data.FirstUsn );
   printf( "Next USN:                0x%I64x\n", journal_data.NextUsn );
   printf( "Max USN:                 0x%I64x\n", journal_data.MaxUsn );
   printf( "Journal size (max size): 0x%I64x\n\n", journal_data.MaximumSize );
 
   // copy the ID of the journal into the read structure
   read_data.UsnJournalID = journal_data.UsnJournalID;
 
   while(1)
   {
      //clear out the working buffer
      memset( buffer, 0, sizeof(buffer) );
 
      // synchronous driver call to grab the journal data starting at the specified usn entry
      if( !DeviceIoControl( volume, FSCTL_READ_USN_JOURNAL, 
            &read_data, sizeof(read_data),
            &buffer, sizeof(buffer), &bytes,NULL) )
      {
         printf( "Read journal failed (%d)\n", GetLastError());
         return;
      }
 
      // the data returned is the next USN followed by zero or more USN_RECORD structures
      returned_bytes = bytes - sizeof(USN);
 
      // ran out of journal entries
      if (returned_bytes==0)
        break;
 
      // Find the first record (skip the USN)
      usn_record = (USN_RECORD*)(((char*)buffer) + sizeof(USN));  
 
      while( returned_bytes > 0 )
      {
         printf( "\nUSN: %I64x\n", usn_record->Usn );
         printf("File name: %.*S\n", 
                      usn_record->FileNameLength/2, 
                      usn_record->FileName );
         printf("File reference: %x\n",usn_record->FileReferenceNumber);
         printf("Parent reference: %x\n",usn_record->ParentFileReferenceNumber);
 
         SYSTEMTIME time;
         FileTimeToSystemTime((FILETIME*)&usn_record->TimeStamp,&time);
         printf("Date:    %d/%d/%d   Time: %02d:%02d.%02d\n",time.wDay,time.wMonth,time.wYear,time.wHour,time.wMinute,time.wSecond);
         printf("Reasons: %x\n", usn_record->Reason );
         printf("  ");
         if (usn_record->Reason & USN_REASON_BASIC_INFO_CHANGE)
           printf("BASIC ATTRIBUTES, ");
         if (usn_record->Reason & USN_REASON_CLOSE)
           printf("CLOSED, ");
         if (usn_record->Reason & USN_REASON_COMPRESSION_CHANGE)
           printf("COMPRESSION CHANGE, ");
         if (usn_record->Reason & USN_REASON_DATA_EXTEND)
           printf("EXTEND, ");
         if (usn_record->Reason & USN_REASON_DATA_OVERWRITE)
           printf("OVERWRITE, ");
         if (usn_record->Reason & USN_REASON_DATA_TRUNCATION)
           printf("TRUNCATE, ");
         if (usn_record->Reason & USN_REASON_EA_CHANGE)
           printf("EXTENDED ATTRIBUTES, ");
         if (usn_record->Reason & USN_REASON_ENCRYPTION_CHANGE)
           printf("ENCRYPTION CHANGE, ");
         if (usn_record->Reason & USN_REASON_FILE_CREATE)
           printf("CREATE, ");
         if (usn_record->Reason & USN_REASON_FILE_DELETE)
           printf("DELETE, ");
         if (usn_record->Reason & USN_REASON_HARD_LINK_CHANGE)
           printf("HARDLINK CHANGE, ");
         if (usn_record->Reason & USN_REASON_INDEXABLE_CHANGE)
           printf("INDEXABLE CHANGE, ");
         if (usn_record->Reason & USN_REASON_NAMED_DATA_EXTEND)
           printf("STREAM EXTEND, ");
         if (usn_record->Reason & USN_REASON_NAMED_DATA_OVERWRITE)
           printf("STREAM OVERWRITE, ");
         if (usn_record->Reason & USN_REASON_NAMED_DATA_TRUNCATION)
           printf("STREAM TRUNCATE, ");
         if (usn_record->Reason & USN_REASON_OBJECT_ID_CHANGE)
           printf("ID CHANGE, ");
         if (usn_record->Reason & USN_REASON_RENAME_NEW_NAME)
           printf("NEW NAME, ");
         if (usn_record->Reason & USN_REASON_RENAME_OLD_NAME)
           printf("OLD NAME, ");
         if (usn_record->Reason & USN_REASON_REPARSE_POINT_CHANGE)
           printf("REPARSE CHANGE, ");
         if (usn_record->Reason & USN_REASON_SECURITY_CHANGE)
           printf("SECURITY CHANGE, ");
         if (usn_record->Reason & USN_REASON_STREAM_CHANGE)
           printf("STREAM CHANGE, ");
         printf("\n");
 
         printf("Attribs: %x\n",usn_record->FileAttributes);
         printf("  ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_ARCHIVE)
           printf("ARCHIVE, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_COMPRESSED)
           printf("COMPRESSED, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
           printf("DIRECTORY, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
           printf("ENCRYPTED, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_HIDDEN)
           printf("HIDDEN, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_NORMAL)
           printf("NORMAL, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
           printf("NOT INDEXED, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_OFFLINE)
           printf("OFFLINE, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_READONLY)
           printf("READONLY, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
           printf("REPARSE POINT, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_SPARSE_FILE)
           printf("SPARSE, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_SYSTEM)
           printf("SYSTEM, ");
         if (usn_record->FileAttributes & FILE_ATTRIBUTE_TEMPORARY)
           printf("TEMP,");
         printf("\n");
 
         // because of the filename each USN RECORD is a variable length
         returned_bytes -= usn_record->RecordLength;
             
         // Find the next record
         usn_record = (USN_RECORD*)(((char*)usn_record) + usn_record->RecordLength); 
      }
 
      // Update starting USN for next call obtaining the next block of memory.
      // The first 8 bytes of the block returned from the current call is the USN of the next block
      read_data.StartUsn = *(USN*)&buffer; 
   }
}
 