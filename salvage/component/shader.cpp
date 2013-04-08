#include "shader.h"
#include "hw.h"
#include "file_system.h"

static const char *defaultProgram = 
"\n"
"uniform float4 ModelJoint[64*3];\n"
"uniform float4x4 ViewPersp;\n"
"uniform float4x4 ModelViewPersp;\n"
"uniform sampler2D diffuse;\n"
"\n"
"void vpmain( in float3 ipos : POSITION,\n"
"			 in float2 itex : TEXCOORD0,\n"
"#if defined(SKINNED)\n"
"			in float4 iboneIndex : TEXCOORD3,\n"
"			in float4 iboneWeight : TEXCOORD4,\n"
"#endif\n"
"			 out float4 opos : POSITION,\n"
"			 out float2 otex : TEXCOORD0 )\n"
"{\n"
"#if defined(SKINNED)\n"
"	float4 dp0 = ModelJoint[iboneIndex.x*3+0]*iboneWeight.x + ModelJoint[iboneIndex.y*3+0]*iboneWeight.y + ModelJoint[iboneIndex.z*3+0]*iboneWeight.z + ModelJoint[iboneIndex.w*3+0]*iboneWeight.w;\n"
"	float4 dp1 = ModelJoint[iboneIndex.x*3+1]*iboneWeight.x + ModelJoint[iboneIndex.y*3+1]*iboneWeight.y + ModelJoint[iboneIndex.z*3+1]*iboneWeight.z + ModelJoint[iboneIndex.w*3+1]*iboneWeight.w;\n"
"	float4 dp2 = ModelJoint[iboneIndex.x*3+2]*iboneWeight.x + ModelJoint[iboneIndex.y*3+2]*iboneWeight.y + ModelJoint[iboneIndex.z*3+2]*iboneWeight.z + ModelJoint[iboneIndex.w*3+2]*iboneWeight.w;\n"
"	float3 objectSpacePos;\n"
"	objectSpacePos.x = dot( dp0, float4(ipos,1.f) );\n"
"	objectSpacePos.y = dot( dp1, float4(ipos,1.f) );\n"
"	objectSpacePos.z = dot( dp2, float4(ipos,1.f) );\n"
"	opos = mul( ModelViewPersp, float4(objectSpacePos,1.f) );\n"
"#else\n"
"	opos = mul( ModelViewPersp, float4(ipos,1.f) );\n"
"#endif\n"
"	otex = float2( itex.x, 1-itex.y );\n"
"}\n"
"void fpmain( in float2 itex : TEXCOORD0, out float4 ocol0 : COLOR0 )\n"
"{\n"
"	ocol0 = tex2D( diffuse, itex );\n"
"	ocol0.x = 1.f;\n"
"}\n";

Shader::Shader() : m_fragProgram( NULL )
{
	m_vertProgram[kStatic] = NULL;
	m_vertProgram[kSkinned] = NULL;
}

Shader::~Shader()
{
	Purge();
}

void Shader::Purge()
{
	hw::ReleaseVertexProgram( m_vertProgram[kStatic] );
	hw::ReleaseVertexProgram( m_vertProgram[kSkinned] );
	hw::ReleaseFragmentProgram( m_fragProgram );

	m_vertProgram[kStatic] = NULL;
	m_vertProgram[kSkinned] = NULL;
	m_fragProgram = NULL;

	m_vertexParams[kStatic].clear();
	m_vertexSamplers[kStatic].clear();
	m_vertexParams[kSkinned].clear();
	m_vertexSamplers[kSkinned].clear();

	m_fragmentParams.clear();
	m_fragmentSamplers.clear();

	SetLoaded( false );
	SetDefaulted( false );
}


void Shader::Load( const char *filename )
{
	char buffer[512];

	m_vertProgram[kStatic] = hw::CreateVertexProgramFromFile( fs::MakeCanonicalForm( buffer, filename ), "vpmain", "", m_vertexParams[kStatic], m_vertexSamplers[kStatic] );
	m_vertProgram[kSkinned] = hw::CreateVertexProgramFromFile( fs::MakeCanonicalForm( buffer, filename ), "vpmain", "SKINNED", m_vertexParams[kSkinned], m_vertexSamplers[kSkinned] );

	m_fragProgram = hw::CreateFragmentProgramFromFile( fs::MakeCanonicalForm( buffer, filename ), "fpmain", "", m_fragmentParams, m_fragmentSamplers );

	if ( m_vertProgram == NULL || m_fragProgram == NULL )
	{
		Purge();
		m_vertProgram[kStatic] = hw::CreateVertexProgram( defaultProgram, "vpmain", "", m_vertexParams[kStatic], m_vertexSamplers[kStatic] );
		m_vertProgram[kSkinned] = hw::CreateVertexProgram( defaultProgram, "vpmain", "SKINNED", m_vertexParams[kSkinned], m_vertexSamplers[kSkinned] );

		m_fragProgram = hw::CreateFragmentProgram( defaultProgram, "fpmain", "", m_fragmentParams, m_fragmentSamplers );
		SetDefaulted( true );
	}
}

void Shader::Bind( GeomType gt ) const
{
	hw::BindVertexProgram( m_vertProgram[gt], m_vertexParams[gt], m_vertexSamplers[gt] );
	hw::BindFragmentProgram( m_fragProgram, m_fragmentParams, m_fragmentSamplers );
}
