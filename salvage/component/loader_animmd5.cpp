#include "dxut.h"
#include "loader_animmd5.h"
#include "anim_structs.h"
#include <string>
#include "tokeniser.h"
#include "file_system.h"
#include "edge\libedgeanimtool\libedgeanimtool_animation.h"
#include "edge\libedgeanimtool\libedgeanimtool_skeleton.h"

static void CalcW( Vectormath::Aos::Quat &q )
{
	q.setW( -sqrtf( 1.f - (q[0]*q[0]+q[1]*q[1]+q[2]*q[2]) ) );
}

static void ParseMD5AnimHierarchy( PHYSFS_File *f, std::vector<animjoint> &hier, std::vector<int> &jointflags, Edge::Tools::Skeleton &edgeSkel, Edge::Tools::Animation &edgeAnim ) {
	char line[1024];
	while ( PHYSFS_gets( line, sizeof(line), f ) )
	{
		tokeniser t;
		t.cursor = line;
		t.singleCharList = "/\\";

		token tok;
		if ( tokGetToken( tok, t.cursor, t.singleCharList ) )
		{
			if ( tokTest( tok, "}" ) )
			{
				break;
			}

			animjoint h;
			tokGetString(h.name,tok);
			h.parent = tokGetTokenInt(t.cursor,t.singleCharList);
			jointflags.push_back( tokGetTokenInt(t.cursor,t.singleCharList) );
			tokGetTokenInt(t.cursor,t.singleCharList);
			hier.push_back(h);

			edgeSkel.m_parentIndices.push_back( h.parent );
			edgeSkel.m_jointNameHashes.push_back( edgeAnimGenerateNameHash( h.name.c_str() ) );

			Edge::Tools::JointAnimation ja;
			ja.m_jointName = edgeAnimGenerateNameHash( h.name.c_str() );
			ja.m_jointWeight = 1.f;
			edgeAnim.m_jointAnimations.push_back( ja );

		}
	}
}

static void ParseMD5AnimBounds( PHYSFS_File *f ) 
{
	char line[1024];
	while ( PHYSFS_gets( line, sizeof(line), f ) )
	{
		tokeniser t;
		t.cursor = line;
		t.singleCharList = "/\\";

		token tok;
		if ( tokGetToken( tok, t.cursor, t.singleCharList ) )
		{
			if ( tokTest( tok, "}" ) )
			{
				break;
			}
		}
	}
}

static void ParseMD5AnimBaseFrame( PHYSFS_File *f, std::vector<animjoint> &hier, Edge::Tools::Skeleton &edgeSkel ) 
{
	int idx = 0;
	char line[1024];
	while ( PHYSFS_gets( line, sizeof(line), f ) )
	{
		tokeniser t;
		t.cursor = line;
		t.singleCharList = "/\\";

		token tok;
		if ( tokGetToken( tok, t.cursor, t.singleCharList ) )
		{
			if ( tokTest( tok, "}" ) )
			{
				break;
			}

			//tokExpect( t.cursor, t.singleCharList, "(" );
			hier[idx].base.t.setX( tokGetTokenFloat(t.cursor,t.singleCharList) * 1.f/32.f );
			hier[idx].base.t.setY( tokGetTokenFloat(t.cursor,t.singleCharList) * 1.f/32.f );
			hier[idx].base.t.setZ( tokGetTokenFloat(t.cursor,t.singleCharList) * 1.f/32.f );
			tokExpect( t.cursor, t.singleCharList, ")" );
			tokExpect( t.cursor, t.singleCharList, "(" );
			hier[idx].base.q.setX( tokGetTokenFloat(t.cursor,t.singleCharList) );
			hier[idx].base.q.setY( tokGetTokenFloat(t.cursor,t.singleCharList) );
			hier[idx].base.q.setZ( tokGetTokenFloat(t.cursor,t.singleCharList) );
			CalcW( hier[idx].base.q );
			tokExpect( t.cursor, t.singleCharList, ")" );

			Vectormath::Aos::Quat relQ;
			Vectormath::Aos::Point3 relT;
			if ( hier[idx].parent != -1 )
			{
				Vectormath::Aos::Transform3 pm( hier[ hier[idx].parent ].base.q, Vectormath::Aos::Vector3(hier[ hier[idx].parent ].base.t) );
				Vectormath::Aos::Transform3 m( hier[ idx ].base.q, Vectormath::Aos::Vector3(hier[ idx ].base.t) );
				Vectormath::Aos::Transform3 relM;
				relM = Vectormath::Aos::inverse(pm) * m;
				relQ = Vectormath::Aos::Quat( relM.getUpper3x3() );
				relT = Vectormath::Aos::Point3( relM.getTranslation() );
			} else
			{
				relQ = hier[idx].base.q;
				relT = hier[idx].base.t;
			}

			Edge::Tools::Joint edgeJoint;
			edgeJoint.m_rotation[0] = relQ[0];
			edgeJoint.m_rotation[1] = relQ[1];
			edgeJoint.m_rotation[2] = relQ[2];
			edgeJoint.m_rotation[3] = relQ[3];
			edgeJoint.m_translation[0] = relT[0];
			edgeJoint.m_translation[1] = relT[1];
			edgeJoint.m_translation[2] = relT[2];
			edgeJoint.m_translation[3] = 1.f;
			edgeJoint.m_scale[0] = 1.f;
			edgeJoint.m_scale[1] = 1.f;
			edgeJoint.m_scale[2] = 1.f;
			edgeJoint.m_scale[3] = 1.f;
			edgeSkel.m_basePose.push_back( edgeJoint );


			idx++;
		}
	}
}

static void ParseMD5AnimFrame( PHYSFS_File *f, std::vector<animjoint> &hier, std::vector<int> &jointflags, int components, float time,  Edge::Tools::Animation &edgeAnim ) {
	unsigned int idx = 0;
	char line[1024];
	while ( PHYSFS_gets( line, sizeof(line), f ) && components)
	{
		while ( idx < jointflags.size() && jointflags[idx] == 0 )
		{
			hier[idx].frames.push_back( hier[idx].base );

			Edge::Tools::AnimationKeyframe rotAnim;
			rotAnim.m_keyTime = time;
			rotAnim.m_keyData[0] = hier[idx].base.q[0];
			rotAnim.m_keyData[1] = hier[idx].base.q[1];
			rotAnim.m_keyData[2] = hier[idx].base.q[2];
			rotAnim.m_keyData[3] = hier[idx].base.q[3];

			Edge::Tools::AnimationKeyframe transAnim;
			transAnim.m_keyTime = time;
			transAnim.m_keyData[0] = hier[idx].base.t[0];
			transAnim.m_keyData[1] = hier[idx].base.t[1];
			transAnim.m_keyData[2] = hier[idx].base.t[2];
			transAnim.m_keyData[3] = 1.f;

			edgeAnim.m_jointAnimations[idx].m_rotationAnimation.push_back( rotAnim );
			edgeAnim.m_jointAnimations[idx].m_translationAnimation.push_back( transAnim );

			idx++;
		}

		tokeniser t;
		t.cursor = line;
		t.singleCharList = "/\\";

		{
			animsample f;
			f = hier[idx].base;
			int flags = jointflags[idx];

			if ( flags & 1 ) {
				f.t.setX( tokGetTokenFloat(t.cursor,t.singleCharList) * 1.f/32.f );
				components--;
			}
			if ( flags & 2 ) {
				f.t.setY( tokGetTokenFloat(t.cursor,t.singleCharList) * 1.f/32.f );
				components--;
			}
			if ( flags & 4 ) {
				f.t.setZ( tokGetTokenFloat(t.cursor,t.singleCharList) * 1.f/32.f );
				components--;
			}
			if ( flags & 8 ) {
				f.q.setX(  tokGetTokenFloat(t.cursor,t.singleCharList) );
				components--;
			}
			if ( flags & 16 ) {
				f.q.setY( tokGetTokenFloat(t.cursor,t.singleCharList) );
				components--;
			}
			if ( flags & 32 ) {
				f.q.setZ( tokGetTokenFloat(t.cursor,t.singleCharList) );
				components--;
			}
			CalcW( f.q );
		
			hier[idx].frames.push_back( f );

			Edge::Tools::AnimationKeyframe rotAnim;
			rotAnim.m_keyTime = time;
			rotAnim.m_keyData[0] = f.q[0];
			rotAnim.m_keyData[1] = f.q[1];
			rotAnim.m_keyData[2] = f.q[2];
			rotAnim.m_keyData[3] = f.q[3];

			Edge::Tools::AnimationKeyframe transAnim;
			transAnim.m_keyTime = time;
			transAnim.m_keyData[0] = f.t[0];
			transAnim.m_keyData[1] = f.t[1];
			transAnim.m_keyData[2] = f.t[2];
			transAnim.m_keyData[3] = 1.f;

			edgeAnim.m_jointAnimations[idx].m_rotationAnimation.push_back( rotAnim );
			edgeAnim.m_jointAnimations[idx].m_translationAnimation.push_back( transAnim );

			idx++;
		}
	}
	tokeniser t;
	t.cursor = line;
	t.singleCharList = "/\\";
	tokExpect( t.cursor, t.singleCharList, "}" );
	for (;idx<hier.size();idx++)
	{
		hier[idx].frames.push_back( hier[idx].base );
	}
}

void LoadMD5Anim( anim &a, const char *filename )
{
	int numJoints = 0;
	int numAnimatedComponents = 0;
	int numFrames;
	std::vector<int> flags;

	PHYSFS_File *f = PHYSFS_openRead( filename );
	if ( f )
	{
		Edge::Tools::Animation edgeAnim;
		Edge::Tools::Skeleton edgeSkel;

		edgeAnim.m_enableLocoDelta = true;
		edgeAnim.m_numFrames = 0;
		edgeAnim.m_startTime = 0.f;
		edgeAnim.m_endTime = 0.f;
		edgeAnim.m_period = 0.f;

		edgeSkel.m_numJoints = 0;
		edgeSkel.m_locoJointIndex = 0;
		edgeSkel.m_numUserChannels = 0;

		float time = 0.f;


		char line[1024];
		while ( PHYSFS_gets( line, sizeof(line), f ) )
		{
			tokeniser t;
			t.cursor = line;
			t.singleCharList = "/\\";

			token tok;
			if ( tokGetToken( tok, t.cursor, t.singleCharList ) )
			{
				if ( tokTest( tok, "MD5Version" ) )
				{
					continue;
				} else
				if ( tokTest( tok, "commandline" ) )
				{
					continue;
				} else
				if ( tokTest( tok, "numJoints" ) ) 
				{
					numJoints = tokGetTokenInt( t.cursor, t.singleCharList );
					edgeSkel.m_numJoints = numJoints;
				} else
				if ( tokTest( tok, "numFrames" ) ) 
				{
					numFrames = tokGetTokenInt( t.cursor, t.singleCharList );
				} else
				if ( tokTest( tok, "frameRate" ) ) 
				{
					a.frameRate = tokGetTokenFloat( t.cursor, t.singleCharList );
				} else
				if ( tokTest( tok, "numAnimatedComponents" ) ) 
				{
					numAnimatedComponents = tokGetTokenInt( t.cursor, t.singleCharList );
				} else
				if ( tokTest( tok, "hierarchy" ) ) {
					ParseMD5AnimHierarchy( f, a.joints, flags, edgeSkel, edgeAnim );
				} else
				if ( tokTest( tok, "bounds" ) ) {
					ParseMD5AnimBounds( f );
				} else
				if ( tokTest( tok, "baseframe" ) ) {
					ParseMD5AnimBaseFrame( f, a.joints, edgeSkel );
				} else
				if ( tokTest( tok, "frame" ) ) 
				{
					edgeAnim.m_numFrames++;
					ParseMD5AnimFrame( f, a.joints, flags, numAnimatedComponents, time, edgeAnim );
					time += 1.f/a.frameRate;
				}
			}
		}
		PHYSFS_close( f );
		edgeAnim.m_endTime = edgeAnim.m_numFrames * 1.f/a.frameRate;
		edgeAnim.m_period = 1.f/a.frameRate;

		if ( edgeAnim.m_enableLocoDelta ) {
			const unsigned int locoJointIndex = edgeSkel.m_locoJointIndex;
			const size_t startFrame = 0;
			const size_t endFrame = edgeAnim.m_jointAnimations[ locoJointIndex ].m_rotationAnimation.size() - 1;

			float *sq = (float*) &edgeAnim.m_jointAnimations[ locoJointIndex ].m_rotationAnimation[ startFrame ].m_keyData;
			float *st = (float*) &edgeAnim.m_jointAnimations[ locoJointIndex ].m_translationAnimation[ startFrame ].m_keyData;
			Vectormath::Aos::Quat quatStart = Vectormath::Aos::Quat( sq[0], sq[1], sq[2], sq[3] );
			Vectormath::Aos::Vector3 transStart = Vectormath::Aos::Vector3( st[0], st[1], st[2] );

			float *eq = (float*) &edgeAnim.m_jointAnimations[ locoJointIndex ].m_rotationAnimation[ endFrame ].m_keyData;
			float *et = (float*) &edgeAnim.m_jointAnimations[ locoJointIndex ].m_translationAnimation[ endFrame ].m_keyData;
			Vectormath::Aos::Quat quatEnd = Vectormath::Aos::Quat( eq[0], eq[1], eq[2], eq[3] );
			Vectormath::Aos::Vector3 transEnd = Vectormath::Aos::Vector3( et[0], et[1], et[2] );

			Vectormath::Aos::Quat quatDelta = quatEnd * ( Vectormath::Aos::conj( quatStart ) );
			Vectormath::Aos::Vector3 transDelta = transEnd - transStart;

			edgeAnim.m_enableLocoDelta = true;
			edgeAnim.m_locoDeltaQuat[0] = quatDelta[0];
			edgeAnim.m_locoDeltaQuat[1] = quatDelta[1];
			edgeAnim.m_locoDeltaQuat[2] = quatDelta[2];
			edgeAnim.m_locoDeltaQuat[3] = quatDelta[3];
			edgeAnim.m_locoDeltaTrans[0] = transDelta[0];
			edgeAnim.m_locoDeltaTrans[1] = transDelta[1];
			edgeAnim.m_locoDeltaTrans[2] = transDelta[2];
			edgeAnim.m_locoDeltaTrans[3] = 0.0f;
		}

		char outputName[256];
		sprintf( outputName, "generated\\%s.edgeanim", filename );
		Edge::Tools::CompressionInfo compInfo;
		compInfo.m_compressionTypeTranslation = Edge::Tools::COMPRESSION_TYPE_BITPACKED;
		Edge::Tools::ExportAnimation( outputName, edgeAnim, edgeSkel, false, false, compInfo );

		for ( unsigned int j=0; j<a.joints.size(); j++ )
		{
			if ( a.joints[j].parent != -1 )
			{
				continue;
			}
			Vectormath::Aos::Matrix4 fixup = Vectormath::Aos::Matrix4::identity();
			fixup[0][0] = 1.f;
			fixup[1][1] = 0.f;
			fixup[1][2] = -1.f;
			fixup[2][1] = 1.f;
			fixup[2][2] = 0.f;
			fixup[3][3] = 1.f;
			for (unsigned int f=0; f<a.joints[j].frames.size(); f++)
			{
				const Vectormath::Aos::Quat &q = a.joints[0].frames[f].q;
				const Vectormath::Aos::Point3 &t = a.joints[0].frames[f].t;
				Vectormath::Aos::Matrix3 r = Vectormath::Aos::Matrix3( q );
				Vectormath::Aos::Matrix4 m( r, Vectormath::Aos::Vector3(t) );
				m = fixup * m;
//				float d = Vectormath::Aos::determinant( m );
				a.joints[0].frames[f].q = Vectormath::Aos::Quat( m.getUpper3x3() );
				a.joints[0].frames[f].t = Vectormath::Aos::Point3( m.getTranslation() );
			}
		}
		if ( a.joints.size() )
		{
			a.locomotion = a.joints[0].frames[a.joints[0].frames.size()-1].t - a.joints[0].frames[0].t;
			for (unsigned int f=0; f<a.joints[0].frames.size(); f++)
			{
				a.joints[0].frames[f].t -= (float)f/(a.joints[0].frames.size()-1) * a.locomotion;
			}
		}
	}
}
