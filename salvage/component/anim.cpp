#include "dxut.h"
#include "anim.h"
#include "anim_structs.h"
#include "file_system.h"
#include "bind.h"
#include "loader_animmd5.h"

Anim::Anim() : edgeAnim(NULL)
{
}

Anim::~Anim()
{
}

void Anim::Purge()
{
	a.joints.clear();
	_aligned_free( edgeAnim );
	edgeAnim = NULL;
}

void Anim::LoadMD5( const char *filename )
{
	LoadMD5Anim( a, filename );

	char outputName[256];
	sprintf( outputName, "%s.edgeanim", filename );
	PHYSFS_readFileAligned( outputName, (void**)&edgeAnim, 16 );
}

void Anim::Load( const char *filename )
{
	Purge();

	if ( strstr( filename, ".md5anim" ) )
	{
		LoadMD5( filename );
	}
}

int  Anim::NumTransforms() const
{
	return a.joints.size();
}

void Anim::SampleObjectSpace( Vectormath::Aos::Transform3 *out, int num, float time ) const
{
	if ( a.joints.size() == 0 )
	{
		for (int i=0; i<num; i++)
		{
			out[i] = Vectormath::Aos::Transform3::identity();
		}
		return;
	}
	float animTime = time * a.frameRate;
	int frame = (int)(animTime) % a.joints[0].frames.size();
	int nextFrame = (frame+1) % a.joints[0].frames.size();
	float delta = animTime - floorf( animTime );
	for (int i=0; i<num && i<(int)a.joints.size(); i++)
	{
		const animsample &s0 = a.joints[i].frames[frame];
		const animsample &s1 = a.joints[i].frames[nextFrame];

		Vectormath::Aos::Quat q = Vectormath::Aos::slerp( delta, s0.q, s1.q );
		Vectormath::Aos::Vector3 t = Vectormath::Aos::lerp( delta, Vectormath::Aos::Vector3(s0.t), Vectormath::Aos::Vector3(s1.t) );

		Vectormath::Aos::Transform3 joint = Vectormath::Aos::Transform3( q, t );

		if ( a.joints[i].parent != -1 )
		{
			out[i] = out[a.joints[i].parent] * joint;
		} else
		{
			out[i] = joint;
		}
	}
}

void Anim::SampleJointSpace( animsample *out, int num, float time, AnimMirrorPair *mirror ) const
{
	if ( a.joints.size() == 0 )
	{
		for (int i=0; i<num; i++)
		{
			out[i].q = Vectormath::Aos::Quat::identity();
			out[i].t = Vectormath::Aos::Point3(0.f,0.f,0.f);
		}
		return;
	}
	animsample *target = out;
	if ( mirror )
	{
		target = (animsample*)malloc( sizeof(animsample) * num );
	}

	float animTime = time * a.frameRate;
	int frame = (int)(animTime) % a.joints[0].frames.size();
	int nextFrame = (frame+1) % a.joints[0].frames.size();
	float delta = animTime - floorf( animTime );
	for (int i=0; i<num && i<(int)a.joints.size(); i++)
	{
		const animsample &s0 = a.joints[i].frames[frame];
		const animsample &s1 = a.joints[i].frames[nextFrame];

		target[i].q = Vectormath::Aos::slerp( delta, s0.q, s1.q );
		target[i].t = Vectormath::Aos::lerp( delta, s0.t, s1.t );
	}

	if ( mirror )
	{
		for ( unsigned int i = 0; i < a.joints.size(); i++ )
		{
			animsample *dst = &out[ i ];
			animsample *src = &target[ mirror[i].idx ];

			unsigned int mirrorFunc = mirror[ i ].mirrorfunc;
			
			Vectormath::Aos::Quat		dstRotation;
			Vectormath::Aos::Vector4	dstTranslation;
			
			for ( int loop = 3; loop >= 0; loop-- )
			{
				unsigned int rSpecBits = ( mirrorFunc & 0x000f0000 ) >> 16;
				unsigned int tSpecBits = ( mirrorFunc & 0x0000000f );

				float rVal = src->q[ rSpecBits & 0x03 ];
				float tVal = src->t[ tSpecBits & 0x03 ];
			
				if ( rSpecBits & 0x04 )	rVal = -rVal;
				if ( tSpecBits & 0x04 ) tVal = -tVal;

				dstRotation[loop] = rVal;
				dstTranslation[loop] = tVal;
					
				mirrorFunc = mirrorFunc >> 4;
			}

			dst->q = dstRotation;
			dst->t = Vectormath::Aos::Point3( dstTranslation.getXYZ() );
		}
		free( target );
	}
}

int Anim::JointParent( int jointIndex )
{
	return a.joints[jointIndex].parent;
}


Anim* AnimManager::CreateEmpty( const char *filename )
{
	return NULL;
}

Anim* AnimManager::LoadResource( const char *filename )
{
	Anim *a = new Anim;
	a->Load( filename );
	return a;
}

AnimManager::AnimManager()
{
}


AnimManager *animManager()
{
	static AnimManager mgr;
	return &mgr;
}
