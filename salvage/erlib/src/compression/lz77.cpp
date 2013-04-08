// lz77.cpp : Defines the entry point for the console application.
//

#include "erbase.h"



typedef struct edoccode { 
	int lenMin, lenMax;
	
	int numExtraBits;
} edoccode;

edoccode edoccodes[] = {
	{ 3, 3, 0 },
	{ 4, 4, 0 },
	{ 5, 5, 0 },
	{ 6, 6, 0 },
	{ 7, 7, 0 },
	{ 8, 8, 0 },
	{ 9, 9, 0 },
	{ 10, 10, 0 },
	{ 11, 12, 1 },
	{ 13, 14, 1 },
	{ 15, 16, 1 },
	{ 17, 18, 1 },
	{ 19, 22, 2 },
	{ 23, 26, 2 },
	{ 27, 30, 2 },
	{ 31, 34, 2 },
	{ 35, 42, 3 },
	{ 43, 50, 3 },
	{ 51, 58, 3 },
	{ 59, 66, 3 },
	{ 67, 82, 4 },
	{ 83, 98, 4 },
	{ 99, 114, 4 },
	{ 115, 130, 4 },
	{ 131, 162, 5 },
	{ 163, 194, 5 },
	{ 195, 226, 5 },
	{ 227, 257, 5 },
	{ 258, 258, 0 }
};

edoccode distancecodes[] = {
	{ 1, 1, 0 },
	{ 2, 2, 0 },
	{ 3, 3, 0 },
	{ 4, 4, 0 },
	{ 5, 6, 1 },
	{ 7, 8, 1 },
	{ 9, 12, 2 },
	{ 13, 16, 2 },
	{ 17, 24, 3 },
	{ 25, 32, 3 },
	{ 33, 48, 4 },
	{ 49, 64, 4 },
	{ 65, 96, 5 },
	{ 97, 128, 5 },
	{ 129, 192, 6 },
	{ 193, 256, 6 },
	{ 257, 384, 7 },
	{ 385, 512, 7 },
	{ 513, 768, 8 },
	{ 769, 1024, 8 },
	{ 1025, 1536, 9 },
	{ 1537, 2048, 9 },
	{ 2049, 3072, 10 },
	{ 3073, 4096, 10 },
	{ 4097, 6144, 11 },
	{ 6145, 8192, 11 },
	{ 8193, 12288, 12 },
	{ 12289, 16384, 12 },
	{ 16385, 24576, 13 },
	{ 24577, 32768, 13 }
};



typedef unsigned char byte;


void writeBits( byte *outBuffer, int *writePos, unsigned int v, int num ) {
	while ( num ) {
		int wp = (*writePos);
		int emptyBits = 8 - (wp & 7);

		if ( emptyBits == num ) {
			outBuffer[ wp/8 ] |= v;
			(*writePos) += num;
			break;
		} else
		if ( emptyBits > num ) {
			outBuffer[ wp/8 ] |= (v << (emptyBits-num));
			(*writePos) += num;
			break;
		} else {
			unsigned int t = v;
			t >>= (num-emptyBits);
			outBuffer[ wp/8 ] |= t;
			(*writePos) += emptyBits;
			num -= emptyBits;
			v &= ((1<<num)-1);
		}
	}	
}


unsigned int readBits( byte *inBuffer, int *readPos, int num ) {
	unsigned int out = 0;
	
	while ( num ) {
		int rp = *readPos;
		int fullBits = 8 - (rp & 7);
		if ( fullBits == num ) {
			out <<= num;
			out |= inBuffer[ rp/8 ] & ((1<<num)-1);
			(*readPos) += num;
			num = 0;
		} else
		if ( fullBits > num ) {
			out <<= num;
			out |= (inBuffer[ rp/8 ] >> (fullBits-num)) & ((1<<num)-1);
			(*readPos) += num;
			num = 0;
		} else {
			out <<= fullBits;
			out |= inBuffer[ rp/8 ] & ((1<<fullBits)-1);
			num -= fullBits;
			(*readPos) += fullBits;
		}
	}

	return out;
}

unsigned int readBits( byte *inBuffer, int *readPos, byte *source, int overlap, int num ) {
	unsigned int out = 0;
	
	while ( num ) {
		int rp = (*readPos) & ((overlap*8)-1);
		int fullBits = 8 - (rp & 7);
		if ( fullBits == num ) {
			//assert( inBuffer[ (rp/8) ] == source[((*readPos)/8)] );
			out <<= num;
			out |= inBuffer[ rp/8 ] & ((1<<num)-1);
			inBuffer[ (rp/8) ] = source[((*readPos)/8)+overlap];
			(*readPos) += num;
			num = 0;
		} else
		if ( fullBits > num ) {
			//assert( inBuffer[ (rp/8) ] == source[((*readPos)/8)] );
			out <<= num;
			out |= (inBuffer[ rp/8 ] >> (fullBits-num)) & ((1<<num)-1);
			(*readPos) += num;
			num = 0;
		} else {
			//assert( inBuffer[ (rp/8) ] == source[((*readPos)/8)] );
			out <<= fullBits;
			out |= inBuffer[ rp/8 ] & ((1<<fullBits)-1);
			num -= fullBits;
			inBuffer[ rp/8 ] = source[((*readPos)/8)+overlap];
			(*readPos) += fullBits;
		}
	}

	return out;
}

int calcMatchLength( int *bestMatchI, byte *buffer, int pos, int end ) {
	int bestMatch = 0;
	int i;

	for (i=0; i<pos; i++) {
		int j = 0;
		for (j=0; j<258; j++) {
			
			if ( (pos+j) >= end ) {
				break;
			}
			byte c = (i+j) >= pos ? buffer[pos-1] : buffer[i+j];
			if ( buffer[ pos+j ] != c ) {
				break;
			}
		}
		if ( j >= bestMatch ) {
			bestMatch = j;
			*bestMatchI = pos - i;
		}
	}
	return bestMatch;
}

int edocBits( int *base, int edoc ) {
	if ( edoc <= 143 ) {
		*base = 0x30 + ( edoc );
		return 8;
	}
	if ( edoc <= 255 ) {
		*base = 0x190 + ( edoc - 144 );
		return 9;
	}
	if ( edoc <= 279 ) {
		*base = 0x0 + ( edoc - 256 );
		return 7;
	}
	if ( edoc <= 287 ) {
		*base = 0xc0 + ( edoc - 280 );
		return 8;
	}
	ASSERT( 0 );
	return 0;
}


int edocFromLength( int len ) {
	int i;
	for (i=0; i<sizeof(edoccodes)/sizeof(edoccodes[0]); i++) {
		if ( len >= edoccodes[i].lenMin && len <= edoccodes[i].lenMax ) {
			return 257+i;
		}
	}
	return -1;
}

int edocExtraBits( int edoc ) {
	if ( edoc < 257 ) {
		return 0;
	}
	return edoccodes[edoc-257].numExtraBits;
}

int distcodeFromLength( int len ) {
	int i;
	for (i=0; i<sizeof(distancecodes)/sizeof(distancecodes[0]); i++) {
		if ( len >= distancecodes[i].lenMin && len <= distancecodes[i].lenMax ) {
			return i;
		}
	}
	return -1;
}


int compress( byte *outBuffer, byte *inBuffer, int end ) {
	int i = 0;
	int wp = 0;
	writeBits( outBuffer, &wp, 1, 3 );
	while ( i < end ) {
		int p;
		int m = calcMatchLength( &p, inBuffer, i, end );
		//printf( "match %d %d %d\n", i, m, i-p );
		int edoc;
		if ( m > 2 ) {
			edoc = edocFromLength( m );
		//printf( "compresss edoc1 %d %d %d %d", i, readLen, edoc, m );
			i+=m;
		} else {
			edoc = inBuffer[i];
		//printf( "compresss edoc2 %d %d %d ", i, readLen, edoc );
			i++;
		}
		int out;
		int numBits = edocBits( &out, edoc );
		writeBits( outBuffer, &wp, out, numBits );

		if ( edoc >= 257 ) {
			writeBits( outBuffer, &wp, m - edoccodes[ edoc - 257 ].lenMin, edocExtraBits( edoc ) );

			int idx = distcodeFromLength( p );
			writeBits( outBuffer, &wp, idx, 5 );
		//printf( "%d %d %d",idx, p, m );
			writeBits( outBuffer, &wp, p - distancecodes[idx].lenMin, distancecodes[idx].numExtraBits );
//break;
		}
	//printf( "\n");

	}
	writeBits( outBuffer, &wp, 0, 7 );

	return ( wp+7 ) / 8;
}

int check( byte *a, byte *b, int len ) {
	int i;
	for ( i=0; i<len; i++) {
		if ( a[i] != b[i] ) {
			printf( "failed comparison %d\n", i );
			return 1;
		}
	}
	return 0;
}
// a b c 
int readEdoc( byte *inBuffer, int *rp ) {
	unsigned int v = readBits( inBuffer, rp, 7 );
	
	if ( v >= 0x00 && v <= 0x17 ) {
		return 256 + v;
	}
	v = (v<<1) | readBits( inBuffer, rp, 1);
	if ( v >= 0x30 && v <= 0xbf ) {
		return (v-0x30);
	}
	if ( v >= 0xc0 && v <= 0xc5 ) {
		return 280 + (v-0xc0);
	}
	v = (v<<1) | readBits( inBuffer, rp, 1);
	return 144 + (v-0x190);
}

int readEdoc( byte *inBuffer, byte *source, int overlap, int *rp ) {
	unsigned int v = readBits( inBuffer, rp, source, overlap, 7 );
	
	if ( v >= 0x00 && v <= 0x17 ) {
		return 256 + v;
	}
	v = (v<<1) | readBits( inBuffer, rp, source, overlap, 1);
	if ( v >= 0x30 && v <= 0xbf ) {
		return (v-0x30);
	}
	if ( v >= 0xc0 && v <= 0xc5 ) {
		return 280 + (v-0xc0);
	}
	v = (v<<1) | readBits( inBuffer, rp, source, overlap, 1);
	return 144 + (v-0x190);
}

int decompress( byte *outBuffer, byte *inBuffer ) {
	int rp = 0;
	int decompLen = 0;
	unsigned int v = readBits( inBuffer, &rp, 3 );
	while ( 1 ) {
		int edoc = readEdoc( inBuffer, &rp );
		if ( edoc == 256 ) {
			break;
		} else
		if ( edoc >= 257 ) {
			int eb = readBits( inBuffer, &rp, edocExtraBits( edoc ) );
			int len = edoccodes[ edoc - 257 ].lenMin + eb;

			int idx = readBits( inBuffer, &rp, 5 );
			int dist = readBits( inBuffer, &rp, distancecodes[idx].numExtraBits ) + distancecodes[idx].lenMin;
			
			int i;
			for (i=0; i<len; i++) {
				int ofs = decompLen - dist + i;
				byte c = (ofs >= decompLen) ? outBuffer[ decompLen-1 ] : outBuffer[ ofs ];
				outBuffer[ decompLen+i ] = c;
			}
			decompLen += len;
		} else {
			outBuffer[ decompLen++ ] = edoc;
		}
	}
	return decompLen;
}


int decompress_inplace( byte *outBuffer, byte *inBuffer ) {
	int rp = 0;
	int decompLen = 0;
	byte overlap[ 1024 ];
	memcpy( overlap, inBuffer, sizeof(overlap) );
	unsigned int v = readBits( overlap, &rp, inBuffer, sizeof(overlap), 3 );

	while ( 1 ) {
		if ( &outBuffer[decompLen] >= &inBuffer[(rp/8)+sizeof(overlap)] ) {
			return decompLen;
		}
		int edoc = readEdoc( overlap, inBuffer, sizeof(overlap), &rp );
		if ( edoc == 256 ) {
			break;
		} else
		if ( edoc >= 257 ) {
			int eb = readBits( overlap, &rp, inBuffer, sizeof(overlap), edocExtraBits( edoc ) );
			int len = edoccodes[ edoc - 257 ].lenMin + eb;

			int idx = readBits( overlap, &rp, inBuffer, sizeof(overlap), 5 );
			int dist = readBits( overlap, &rp, inBuffer, sizeof(overlap), distancecodes[idx].numExtraBits ) + distancecodes[idx].lenMin;
			
			int i;
			for (i=0; i<len; i++) {
				int ofs = decompLen - dist + i;
				byte c = (ofs >= decompLen) ? outBuffer[ decompLen-1 ] : outBuffer[ ofs ];
				outBuffer[ decompLen+i ] = c;
			}
			decompLen += len;
		} else {
			outBuffer[ decompLen++ ] = edoc;
		}
	}
	return decompLen;
}


#if 0
int _tmain(int argc, _TCHAR* argv[])
{
	FILE *f = fopen( argv[1], "rb" );
	if ( f ) {
		byte buffer[32*1024];
		byte decomp[32*1024];
		byte outBuffer[64*1024];
		int totread = 0;
		int totcomp = 0;
		while ( 1 ) {
			printf( "%d ", ftell( f ) );
			int readLen = fread( buffer, 1, sizeof(buffer), f );
			printf( "%d\n", readLen );

			memset( outBuffer, 0, sizeof(outBuffer) );

			int writePos = compress( outBuffer, buffer, readLen );
			printf( "numBytes %d\n", writePos );
			int decompLen = decompress( decomp, outBuffer );

			memcpy( &decomp[ readLen-writePos ], outBuffer, writePos );
			decompress_inplace( decomp, &decomp[ readLen-writePos ] );
				
			printf( "%d %d\n", readLen, decompLen );
			int c = check( decomp, buffer, readLen );
			printf( "%d\n", c );
			printf( "%d\n", (writePos*100)/(readLen) );

			if ( c != 0 ) {
				return 1;
			}

			totread += readLen;
			totcomp += writePos;

			if ( readLen < sizeof( buffer ) ) {
				break;
			}
		}
		fclose( f );
		printf( "%d %d\n", totread, totcomp );
		printf( "%d\n", (totcomp*100)/(totread) );
	}
	return 0;
}

#endif