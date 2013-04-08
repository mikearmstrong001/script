#include "../erbase.h"
#include "maths.h"

// http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html

struct vcache_vertex_data {
	int		NumActiveTris;
	int		CacheTag;
	float	CurrentScore;
	int		NumTris;
	struct vcache_tri_data		**Tris;
};

struct vcache_tri_data {
	int		Active;
	float	CurrentScore;
	int		v[3];
};

const int   MaxSizeVertexCache = 16;

const float FindVertexScore_CacheDecayPower = 1.5f;
const float FindVertexScore_LastTriScore = 0.75f;
const float FindVertexScore_ValenceBoostScale = 2.0f;
const float FindVertexScore_ValenceBoostPower = 0.5f;
static float FindVertexScore ( vcache_vertex_data *VertexData )
{
    if ( VertexData->NumActiveTris == 0 )
    {
        // No tri needs this vertex!
        return -1.0f;
    }
 
    float Score = 0.0f;
    int CachePosition = VertexData->CacheTag;
    if ( CachePosition < 0 )
    {
        // Vertex is not in FIFO cache - no score.
    }
    else
    {
        if ( CachePosition < 3 )
        {
            // This vertex was used in the last triangle,
            // so it has a fixed score, whichever of the three
            // it's in. Otherwise, you can get very different
            // answers depending on whether you add
            // the triangle 1,2,3 or 3,1,2 - which is silly.
            Score = FindVertexScore_LastTriScore;
        }
        else
        {
            assert ( CachePosition < MaxSizeVertexCache );
            // Points for being high in the cache.
            const float Scaler = 1.0f / ( MaxSizeVertexCache - 3 );
            Score = 1.0f - ( CachePosition - 3 ) * Scaler;
            Score = powf ( Score, FindVertexScore_CacheDecayPower );
        }
    }
 
    // Bonus points for having a low number of tris still to
    // use the vert, so we get rid of lone verts quickly.
    float ValenceBoost = powf ( (float)VertexData->NumActiveTris,
                                -FindVertexScore_ValenceBoostPower );
    Score += FindVertexScore_ValenceBoostScale * ValenceBoost;
 
    return Score;
}

static int FindBestTri( vcache_tri_data *tris, vcache_vertex_data *verts, int num ) {
	float bestScore = MAX_FLT;
	int   bestIdx = -1;

	for (int i=0; i<num; i++) {
		if ( tris[i].Active == 0 ) {
			continue;
		}

		tris[i].CurrentScore = verts[ tris[i].v[0] ].CurrentScore +
			verts[ tris[i].v[1] ].CurrentScore +
			verts[ tris[i].v[2] ].CurrentScore;


		if ( tris[i].CurrentScore < bestScore ) {
			bestScore = tris[i].CurrentScore;
			bestIdx = i;
		}
	}

	return bestIdx;
}

static void UpdateCache( int *cache, int *indices, vcache_vertex_data *verts ) {
	int entries = MaxSizeVertexCache;
	for ( int i=0; i<3; i++ ) {
		int num;
		if ( verts[ indices[i] ].CacheTag >= 0 ) {
			num = verts[ indices[i] ].CacheTag;
		} else {
			num = entries;
			entries++;
		}
		memmove( &cache[1], &cache[0], num * sizeof(int) );
		cache[0] = indices[i];
	}

	for (int i=MaxSizeVertexCache; i<entries; i++) {
		verts[ cache[i] ].CacheTag = -1;
		verts[ cache[i] ].CurrentScore = FindVertexScore( &verts[ cache[i] ] );
	}

	for (int i=0; i<MaxSizeVertexCache; i++) {
		if ( cache[i] != -1 ) {
			break;
		}
		verts[ cache[i] ].CacheTag = i;
		verts[ cache[i] ].CurrentScore = FindVertexScore( &verts[ cache[i] ] );
	}
}


void VCacheOptimise( int numVerts, int numTris, int *indices ) {
	vcache_tri_data *tris = new vcache_tri_data[ numTris ];
	vcache_vertex_data *verts = new vcache_vertex_data[ numVerts ];

	memset( tris, 0, sizeof(vcache_tri_data)*numTris );
	memset( verts, 0, sizeof(vcache_vertex_data)*numVerts );

	for (int i=0; i<numTris; i++) {
		tris[i].Active = 1;
		tris[i].v[0] = indices[i*3+0];
		tris[i].v[1] = indices[i*3+1];
		tris[i].v[2] = indices[i*3+2];

		verts[tris[i].v[0]].NumTris++;
		verts[tris[i].v[1]].NumTris++;
		verts[tris[i].v[2]].NumTris++;
	}

	for (int i=0; i<numVerts; i++) {
		verts[i].CurrentScore = FindVertexScore( &verts[i] );
		verts[i].Tris = new vcache_tri_data*[ verts[i].NumTris ];
		verts[i].CacheTag = -1;
	}

	for (int i=0; i<numTris; i++) {
		verts[ tris[i].v[0] ].Tris[ verts[ tris[i].v[0] ].NumActiveTris++ ] = &tris[i];
		verts[ tris[i].v[1] ].Tris[ verts[ tris[i].v[1] ].NumActiveTris++ ] = &tris[i];
		verts[ tris[i].v[2] ].Tris[ verts[ tris[i].v[2] ].NumActiveTris++ ] = &tris[i];
		tris[i].CurrentScore = verts[ tris[i].v[0] ].CurrentScore +
			verts[ tris[i].v[1] ].CurrentScore +
			verts[ tris[i].v[2] ].CurrentScore;
	}

	int cache[ MaxSizeVertexCache + 3 ];
	for (int i=0; i<MaxSizeVertexCache; i++) {
		cache[i] = -1;
	}

	int idxPos = 0;
	int best = FindBestTri( tris, verts, numTris );
	while ( best != -1 ) {
		tris[best].Active = 0;

		indices[idxPos++] = tris[best].v[0];
		indices[idxPos++] = tris[best].v[1];
		indices[idxPos++] = tris[best].v[2];

		UpdateCache( cache, tris[best].v, verts );

		best = FindBestTri( tris, verts, numTris );
	}
}
