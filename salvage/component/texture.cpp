#include "texture.h"
#include "image.h"
#include "stb_image.h"
#include "hw.h"
#include "faff.h"
#include "file_system.h"
#include "tokeniser.h"

static stbi_uc default_img[] = {	0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
									0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
									0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff,
									0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, };


Texture::Texture() :
	m_hw(NULL),
	m_w(0),
	m_h(0),
	m_d(0),
	m_tt(TextureType_2d)
{
}

Texture::~Texture()
{
	Purge();
}

void Texture::Purge()
{
	hw::ReleaseTexture2D( m_hw );
	m_hw = NULL;
	m_w = 0;
	m_h = 0;
	m_d = 0;
	m_tt = TextureType_2d;

	SetLoaded( false );
	SetDefaulted( false );
}


void Texture::CreateRenderTexture( int w, int h )
{
	m_hw = hw::CreateRenderTexture2D( w, h );
	m_w = w;
	m_h = h;
	m_d = 1;
}

static int ParseImage( stbi_uc *images[6], char *&cursor, int &x, int &y, uint64_t *hn )
{
	token t;
	if ( tokGetToken( t, cursor, "(,)" ) )
	{
		if ( tokTest( t, "heightmap" ) )
		{
			if ( hn )
			{
				*hn = HashedString( *hn, "heightmap" );
			}

			tokExpect( cursor, "(),", "(" );
			int num = ParseImage( images, cursor, x, y, hn );
			tokExpect( cursor, "(),", ")" );
			if ( images )
			{
				for (int i=0; i<num; i++)
				{
					img::MakeNormalMap( images[i], x, y );
				}
			}
			return num;
		}
		else
		if ( tokTest( t, "vt" ) )
		{
			if ( hn )
			{
				*hn = HashedString( *hn, "vt" );
			}

			tokExpect( cursor, "(),", "(" );
			stbi_uc *imgs[6] = { NULL };
			int num = ParseImage( imgs, cursor, x, y, hn );
			tokExpect( cursor, "(),", ")" );
			if ( images )
			{
				for (int i=0; i<num; i++)
				{
					img::MakeVirtualTexture( "test.vt", imgs[i], x, y, 2.2f );
				}
			}
		}
		else
		if ( tokTest( t, "cube" ) )
		{
			if ( hn )
			{
				*hn = HashedString( *hn, "cube" );
			}

			tokExpect( cursor, "(),", "(" );

			{
				stbi_uc *imgs[6] = { NULL };
				int num = ParseImage( images ? imgs : NULL, cursor, x, y, hn);
				if ( num == 0 )
				{
					return 0;
				}
				if ( images )
				{
					if ( x != y )
					{
						images[1] = (stbi_uc*)malloc(x*x*4);
						img::Resize( imgs[0], x, y, images[1], x, x );
						y = x;
						stbi_image_free( imgs[0] );
					} else
					{
						images[0] = imgs[0];
					}
				}
			}

			tokExpect( cursor, "(),", "," );

			{
				stbi_uc *imgs[6] = { NULL };
				int tx, ty;
				int num = ParseImage( images ? imgs : NULL, cursor, tx, ty, hn );
				if ( num == 0 )
				{
					return 0;
				}
				if ( images )
				{
					if ( (tx != x || ty != y) )
					{
						images[1] = (stbi_uc*)malloc(x*y*4);
						img::Resize( imgs[0], tx, ty, images[1], x, y );
						stbi_image_free( imgs[0] );
					} else
					{
						images[1] = imgs[0];
					}
				}
			}

			tokExpect( cursor, "(),", "," );

			{
				stbi_uc *imgs[6] = { NULL };
				int tx, ty;
				int num = ParseImage( images ? imgs : NULL, cursor, tx, ty, hn );
				if ( num == 0 )
				{
					return 0;
				}
				if ( images )
				{
					if ( (tx != x || ty != y) )
					{
						images[2] = (stbi_uc*)malloc(x*y*4);
						img::Resize( imgs[0], tx, ty, images[1], x, y );
						stbi_image_free( imgs[0] );
					} else
					{
						images[2] = imgs[0];
					}
				}
			}

			tokExpect( cursor, "(),", "," );

			{
				stbi_uc *imgs[6] = { NULL };
				int tx, ty;
				int num = ParseImage( images ? imgs : NULL, cursor, tx, ty, hn );
				if ( num == 0 )
				{
					return 0;
				}
				if ( images )
				{
					if ( (tx != x || ty != y) )
					{
						images[3] = (stbi_uc*)malloc(x*y*4);
						img::Resize( imgs[0], tx, ty, images[1], x, y );
						stbi_image_free( imgs[0] );
					} else
					{
						images[3] = imgs[0];
					}
				}
			}

			tokExpect( cursor, "(),", "," );

			{
				stbi_uc *imgs[6] = { NULL };
				int tx, ty;
				int num = ParseImage( images ? imgs : NULL, cursor, tx, ty, hn );
				if ( num == 0 )
				{
					return 0;
				}
				if ( images )
				{
					if ( (tx != x || ty != y) )
					{
						images[4] = (stbi_uc*)malloc(x*y*4);
						img::Resize( imgs[0], tx, ty, images[1], x, y );
						stbi_image_free( imgs[0] );
					} else
					{
						images[4] = imgs[0];
					}
				}
			}

			tokExpect( cursor, "(),", "," );

			{
				stbi_uc *imgs[6] = { NULL };
				int tx, ty;
				int num = ParseImage( images ? imgs : NULL, cursor, tx, ty, hn );
				if ( num == 0 )
				{
					return 0;
				}
				if ( images )
				{
					if ( (tx != x || ty != y) )
					{
						images[5] = (stbi_uc*)malloc(x*y*4);
						img::Resize( imgs[0], tx, ty, images[1], x, y );
						stbi_image_free( imgs[0] );
					} else
					{
						images[5] = imgs[0];
					}
				}
			}

			tokExpect( cursor, "(),", ")" );

			return 6;
		}
		else
		{
			char buffer[512];
			char filename[512];
			tokGetString( filename, sizeof(filename), t );
			if ( hn )
			{
				*hn = HashedString( *hn, filename );
			}
			if ( images )
			{
				int c;
				int fileLen = 0;
				stbi_uc *fileBuffer = NULL;
				fileLen = (int)PHYSFS_readFile( fs::MakeCanonicalForm( buffer, filename ), (void**)&fileBuffer );
				images[0] = stbi_load_from_memory( fileBuffer, fileLen, &x, &y, &c, 4);
				free( fileBuffer );
				//images[0] = stbi_load( fs::MakeCanonicalForm( buffer, filename ), &x, &y, &c, 4);
				return images[0] ? 1 : 0;
			} else
			{
				return 1;
			}
		}
	}

	return 0;
}

void Texture::Load( const char *filename )
{
	//char buffer[512];
	int x, y, c;
	char *cursor;
	stbi_uc *images[6] = {NULL};
	int imgcount;
	uint64_t hashedName = 0;
	cursor = (char*)filename;
	imgcount = ParseImage( NULL, cursor, x, y, &hashedName );
	if ( imgcount == 1 )
	{
		char outfile[256];
		sprintf( outfile, "textures/%08x_%08x_0.tex", (uint32_t)(hashedName>>32), (uint32_t)(hashedName&0xffffffff) );
		PHYSFS_File *texture = PHYSFS_openRead( outfile );
		if ( texture )
		{
			uint32_t version;
			PHYSFS_readULE32( texture, &version );
			PHYSFS_readSLE32( texture, &m_w );
			PHYSFS_readSLE32( texture, &m_h );
			PHYSFS_readSLE32( texture, &m_d );
			PHYSFS_readSLE32( texture, &m_tt ); 
			PHYSFS_readSLE32( texture, &m_m );
			uint32_t size;
			PHYSFS_readULE32( texture, &size );
			img::uint8_t *compressed = (img::uint8_t *)malloc( size );
			PHYSFS_read( texture, compressed, size, 1 );
			PHYSFS_close( texture );

			m_hw = hw::CreateCompressedTexture2D( m_w, m_h, m_m, compressed, true );

			free( compressed );

			return;
		}
	} else
	if ( imgcount == 6 )
	{
		char outfile[256];
		sprintf( outfile, "textures/%08x_%08x_0.tex", (uint32_t)(hashedName>>32), (uint32_t)(hashedName&0xffffffff) );
		PHYSFS_File *texture = PHYSFS_openRead( outfile );
		if ( texture )
		{
			uint32_t version;
			PHYSFS_readULE32( texture, &version );
			PHYSFS_readSLE32( texture, &m_w );
			PHYSFS_readSLE32( texture, &m_h );
			PHYSFS_readSLE32( texture, &m_d );
			PHYSFS_readSLE32( texture, &m_tt ); 
			PHYSFS_readSLE32( texture, &m_m );
			uint32_t size;
			PHYSFS_readULE32( texture, &size );

			img::uint8_t *compressed[6];
			
			for ( int i=0; i<6; i++ )
			{
				compressed[i] = (img::uint8_t *)malloc( size );
				PHYSFS_read( texture, compressed[i], size, 1 );
			}

			PHYSFS_close( texture );

			m_hw = hw::CreateCompressedTextureCube( m_w, m_m, compressed, true );

			for ( int i=0; i<6; i++ )
			{
				free( compressed[i] );
			}

			return;
		}
	}

	cursor = (char*)filename;
	imgcount = ParseImage( images, cursor, x, y, NULL );
	if ( imgcount == 0 )
	{
		images[0] = default_img;
		x = 4;
		y = 4;
		c = 4;
		imgcount = 1;
	}

	if ( imgcount == 1 )
	{
		stbi_uc *img = images[0];

#if 0
		{
			PHYSFS_mkdir( "vt/" );
			char outfile[256];
			sprintf( outfile, "vt/%08x_%08x_0.vt", (uint32_t)(hashedName>>32), (uint32_t)(hashedName&0xffffffff) );
			if ( PHYSFS_exists( outfile ) == 0 )
			{
				img::MakeVirtualTexture( outfile, images[0], x, y, 2.2f );
			}
		}
#endif

		int size = img::CalcTotalDXTBlocks( x, y ) * 16;
		img::uint8_t *compressed = (img::uint8_t *)malloc( size );
		img::RGBAToDXT( compressed, img, x, y, true, 2.2f );

		m_m = img::CalcTotalMipLevels(x,y);
		m_hw = hw::CreateCompressedTexture2D( x, y, m_m, compressed, true );
		m_w = x;
		m_h = y;
		m_d = 1;
		m_tt = TextureType_2d;

		PHYSFS_mkdir( "textures/" );
		if ( images[0] != default_img )
		{
			char outfile[256];
			sprintf( outfile, "textures/%08x_%08x_0.tex", (uint32_t)(hashedName>>32), (uint32_t)(hashedName&0xffffffff) );
			PHYSFS_File *texture = PHYSFS_openWrite( outfile );
			if ( texture )
			{
				PHYSFS_writeULE32( texture, 0x090 );
				PHYSFS_writeSLE32( texture, m_w );
				PHYSFS_writeSLE32( texture, m_h );
				PHYSFS_writeSLE32( texture, m_d );
				PHYSFS_writeSLE32( texture, m_tt );
				PHYSFS_writeSLE32( texture, m_m );
				PHYSFS_writeSLE32( texture, size );
				PHYSFS_write( texture, compressed, size, 1 );
				PHYSFS_close( texture );
			}
		}

		free( compressed );
		if ( img != default_img )
		{
			stbi_image_free( img );
		} else
		{
			SetDefaulted( true );
		}
	} else
	if ( imgcount == 6 )
	{
		img::uint8_t *compressed[6];
		uint32_t compressedSize[6];
		for ( int i=0; i<6; i++)
		{
			stbi_uc *img = images[i];

			int size = img::CalcTotalDXTBlocks( x, y ) * 16;
			compressedSize[i] = size;
			compressed[i] = (img::uint8_t *)malloc( size );
			img::RGBAToDXT( compressed[i], img, x, y, true, 2.2f );
		}

		m_m = img::CalcTotalMipLevels(x,y);
		m_hw = hw::CreateCompressedTextureCube( x, m_m, compressed, true );
		m_w = x;
		m_h = y;
		m_d = 1;
		m_tt = TextureType_Cube;

		PHYSFS_mkdir( "textures/" );
		{
			char outfile[256];
			sprintf( outfile, "textures/%08x_%08x_0.tex", (uint32_t)(hashedName>>32), (uint32_t)(hashedName&0xffffffff) );
			PHYSFS_File *texture = PHYSFS_openWrite( outfile );
			if ( texture )
			{
				PHYSFS_writeULE32( texture, 0x090 );
				PHYSFS_writeSLE32( texture, m_w );
				PHYSFS_writeSLE32( texture, m_h );
				PHYSFS_writeSLE32( texture, m_d );
				PHYSFS_writeSLE32( texture, m_tt );
				PHYSFS_writeSLE32( texture, m_m );
				PHYSFS_writeSLE32( texture, compressedSize[0] );
				for ( int i=0; i<6; i++)
				{
					PHYSFS_write( texture, compressed[i], compressedSize[i], 1 );
				}
				PHYSFS_close( texture );
			}
		}

		for ( int i=0; i<6; i++)
		{
			free( compressed[i] );

			if ( images[i] != default_img )
			{
				stbi_image_free( images[i] );
			} else
			{
				SetDefaulted( true );
			}
		}
	}
}
