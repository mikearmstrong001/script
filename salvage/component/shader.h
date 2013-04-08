#ifndef TESTING_SHADER_H
#define TESTING_SHADER_H

#include "resource_core.h"
#include <vector>

namespace hw
{
struct VertexProgram;
struct FragmentProgram;
};


struct VirtualToHwMap
{
	int v;
	int h;
	int num;
};


class Shader : public Resource
{
public:
	enum GeomType
	{
		kStatic,
		kSkinned,
		kTypes,
	};
private:

	hw::VertexProgram *m_vertProgram[kTypes];
	hw::FragmentProgram *m_fragProgram;

	std::vector<VirtualToHwMap> m_vertexParams[kTypes];
	std::vector<VirtualToHwMap> m_vertexSamplers[kTypes];

	std::vector<VirtualToHwMap> m_fragmentParams;
	std::vector<VirtualToHwMap> m_fragmentSamplers;


public:
	Shader();
	virtual ~Shader();

	void Load( const char *filename );
	void Bind( GeomType gt ) const;

	virtual void Purge();
	virtual void Reload()
	{
		Purge();
		Load( Name() );
	}
};

#endif

