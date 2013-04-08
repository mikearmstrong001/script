#include "erbase.h"

#ifdef _WIN32

#include <process.h>
#include <windows.h>
#include <time.h>
#include "zlib.h"

static bool s_inited = false;
static HANDLE s_event;
static HANDLE s_moredata;
static HANDLE s_moredecompression;
static uintptr_t s_loader;
static uintptr_t s_decompressor;

struct ReadRequest {
	HANDLE h;
	int ofs;
	int toRead;
	uint8_t *data;
	z_stream strm;
};

struct DecomprssRequest {
	ReadRequest *r;
	int bufferIndex;
	DWORD dataRead;
};


static ReadRequest s_requests[3][16] = {0};
static DecomprssRequest s_decompress[3] = {0};
static uint32_t	   s_requestRead[3] = {0};
static uint32_t	   s_requestCreate[3] = {0};
static uint8_t	   s_buffer[3][ 128 * 1024 ];
static bool		   s_bufferFree[3] = { true, true, true };

static void loaderthread( void * ) {
	
	while ( 1 ) {
		ReadRequest *r = NULL;
		int p = 0;
		for (int i=0; i<3; i++) {
			if ( s_requestRead[i] < s_requestCreate[i] ) {
				p = i;
				r = &s_requests[i][s_requestRead[i]&15];
				break;
			}
		}
		int bufferIdx = -1;
		for (int i=0; i<3; i++) {
			if ( s_bufferFree[i] ) {
				bufferIdx = i;
				break;
			}
		}
		if ( r == NULL || bufferIdx == -1 ) {
			WaitForSingleObject( s_moredata, INFINITE );
			continue;
		}

		int size = 128*1024;
		if ( r->toRead < size ) {
			size = r->toRead;
		}

		OVERLAPPED overlapped = { 0 };
		overlapped.Offset = r->ofs;
		overlapped.hEvent = s_event;

		s_bufferFree[bufferIdx] = false;
		DWORD read = 0;
		BOOL err = ReadFile( r->h, s_buffer[bufferIdx], size, &read, &overlapped );

		DWORD ioerr = 0;
		if ( err ) {
			ioerr = GetLastError();
			if ( WAIT_IO_COMPLETION == ioerr ) {
				ioerr = 0;
			}
		}

		if ( !ioerr ) {
			WaitForSingleObject( s_event, INFINITE );
			BOOL ok = GetOverlappedResult( r->h, &overlapped, &read, TRUE );
			if ( ok ) {
				s_decompress[bufferIdx].bufferIndex = bufferIdx;
				s_decompress[bufferIdx].dataRead = read;
				s_decompress[bufferIdx].r = r;
				SetEvent( s_moredecompression );

				r->toRead -= read;
				r->ofs += read;

				if ( r->toRead == 0 ) {
					s_requestRead[p] ++;
				}
			} else {
				s_bufferFree[bufferIdx] = true;
			}
		}
	}
}

static void decompressorthread( void * ) {
	while ( 1 ) {
		int idx = -1;
		for(int i=0; i<3; i++) {
			if ( s_decompress[i].r ) {
				idx = i;
				break;
			}
		}
		if ( idx == -1 ) {
			WaitForSingleObject( s_moredecompression, INFINITE );
			continue;
		}

		ReadRequest *r = s_decompress[idx].r;
		//memcpy( r->data, s_buffer[idx], s_decompress[idx].dataRead );
		//uLong destLen = 0xfffff;
		//uncompress( r->data, &destLen, s_buffer[idx], s_decompress[idx].dataRead );
		//r->data += destLen;//s_decompress[idx].dataRead;
		r->strm.avail_in = s_decompress[idx].dataRead;
		r->strm.next_in = s_buffer[idx];
		inflate( &r->strm, 0 );
		if ( r->toRead == 0 ) {
			r->data = NULL ;
		}

		s_decompress[idx].r = NULL;
		s_bufferFree[s_decompress[idx].bufferIndex] = true;
		SetEvent( s_moredata );
	}
}

void AsyncInit() {
	if ( s_inited ) {
		return;
	}

	s_event = CreateEvent( NULL, TRUE, FALSE, NULL );
	s_moredata = CreateEvent( NULL, FALSE, FALSE, NULL );
	s_moredecompression = CreateEvent( NULL, FALSE, FALSE, NULL );
	s_loader = _beginthread( loaderthread, 0, NULL );
	s_decompressor = _beginthread( decompressorthread, 0, NULL );
	s_inited = true;
}


void AsyncAddReadRequest( int p, HANDLE h, void *data, int maxdata, int ofs, int toRead) {
	AsyncInit();

	if ( h == 0 ) {
char *buffer;
#include "zlib.h"
	buffer = (char*)malloc( 128 * 1024 * 1024 );
	h = CreateFileA( "test.comp",
		GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_FLAG_NO_BUFFERING|FILE_FLAG_OVERLAPPED, NULL );
	toRead = GetFileSize( h, NULL );

#if 0
	FILE *f = fopen( "e:\\download\\Alive-client-0.3.0.exe", "rb" );
	if ( f ) {
		int d = fread( buffer, 1, 128 * 1024 * 1024, f );
		fclose( f );
		Bytef *buffer2 = (Bytef*)malloc( 128 * 1024 * 1024 );
		uLongf out = 128*1024*1024;
		compress( buffer2, &out, (Bytef*)buffer, d );
		f = fopen( "test.comp", "wb" );
		fwrite( buffer2, out, 1, f );
		fclose( f );
		free( buffer2 );
	}
#endif
	data = buffer;
	}

	uint32_t idx = s_requestCreate[p] & 15;

	ReadRequest *r = &s_requests[p][idx];
	r->h = h;
	r->data = (uint8_t*)data;
	r->ofs = ofs;
	r->toRead = toRead;
	memset( &r->strm, 0, sizeof( r->strm ) );
	r->strm.next_out = (Bytef*)data;
	r->strm.avail_out = 0xffffff;
	inflateInit( &r->strm );

	s_requestCreate[p]++;

	SetEvent( s_moredata );
}

#endif