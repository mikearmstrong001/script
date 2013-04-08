#include "dxut.h"
#include "console.h"
#include "font_manager.h"
#include "material_manager.h"
#include "technique.h"
#include "model_structs.h"
#include "faff.h"
#include "cvar.h"



Console::Console() : enabled(false)
{
	font = fontManager()->Load( "fonts/times_16.font" );
	background = materialManager()->Load( "debug/consolebg.material" );
}

Console::~Console()
{
}

bool Console::Toggle()
{
	enabled = !enabled;
	return enabled;
}

void Console::parseCommandLine()
{
	std::string::size_type index = 0;
	std::vector<std::string> arguments;

	// tokenize
	while(index != std::string::npos)
	{
		// push word
		std::string::size_type next_space = commandLine.find(' ', index);
		arguments.push_back(commandLine.substr(index, next_space));

		// increment index
		if(next_space != std::string::npos)
		{
			index = next_space + 1;
			while ( commandLine[index] == ' ' )
			{
				index++;
			}
		}
		else break;
	}

	for (unsigned int i=0; i<consoleFunctions.size(); i++)
	{
		if ( consoleFunctions[i].name == arguments[0] )
		{
			consoleFunctions[i].function( arguments );
			return;
		}
	}
	// check for cvar ...
	if ( arguments.size() == 2 )
	{
		cvarManager()->SetValue( arguments[0].c_str(), arguments[1].c_str() );
	}
}

void Console::autoComplete()
{
	for (unsigned int i=0; i<consoleFunctions.size(); i++)
	{
		if ( commandLine.size() <= consoleFunctions[i].name.size() && consoleFunctions[i].name.find_first_of( commandLine.c_str() ) == 0 )
		{
			commandLine = consoleFunctions[i].name;
			return;
		}
	}
	commandLine = cvarManager()->AutoComplete( commandLine.c_str() );
}


void Console::KeyPress( char c )
{
	if ( !enabled )
	{
		return;
	}

	if ( c >= 32 )
	{
		commandLine.push_back( c );
	}
	else if ( c == 9 ) // tab
	{
		if ( commandLine.size() )
		{
			autoComplete();
		}
	}
	else if ( c == 8 ) // backspace
	{
		if ( commandLine.size() )
		{
			commandLine.resize( commandLine.size()-1 );
		}
	}
	else if ( c == 13 && commandLine.size() ) // enter
	{
		parseCommandLine();
		commandLine.erase();
	}
}

int	 Console::AddRenderCommands( struct RenderCommand *cmd )
{
	if ( !enabled )
	{
		return 0;
	}
	int count = 0;
	{
		ModelGeometry *geom = new(FrameAlloc( sizeof(ModelGeometry) )) ModelGeometry;
		ModelVert *verts = (ModelVert*)FrameAlloc( sizeof(ModelVert)*4 );
		unsigned short *indices = (unsigned short*)FrameAlloc( sizeof(unsigned short)*6 );

		const Material *material = background;
		const Technique *tech = material->GetTechnique();

		for (int i=0; i<tech->NumPasses();i++)
		{
			cmd->lightmapBinds = NULL;
			cmd->geom = geom;
			cmd->mat = material;
			cmd->tech = tech->GetPass(i);
			cmd->m_localToWorld = Vectormath::Aos::Transform3::identity();
			cmd->passIdx = RP_Debug2D;//cmd->tech->pass;
			cmd++;
			count++;
		}

		geom->m_indices = indices;
		geom->m_numIndices = 6;
		geom->m_verts = verts;
		geom->m_numVerts = 4;

		ModelVert *v0 = verts++;
		ModelVert *v1 = verts++;
		ModelVert *v2 = verts++;
		ModelVert *v3 = verts++;

		v0->p[0] = 0.f;
		v0->p[1] = 0.f;
		v0->p[2] = 1.f;
		v0->t[0] = 0.f;
		v0->t[1] = 0.f;

		v1->p[0] = 640.f;
		v1->p[1] = 0.0;
		v1->p[2] = 1.f;
		v1->t[0] = 1.f;
		v1->t[1] = 0.f;

		v2->p[0] = 640.f;
		v2->p[1] = 100.f;
		v2->p[2] = 1.f;
		v2->t[0] = 1.f;
		v2->t[1] = 1.f;

		v3->p[0] = 0.f;
		v3->p[1] = 100.f;
		v3->p[2] = 1.f;
		v3->t[0] = 0.f;
		v3->t[1] = 1.f;

		*indices++ = (unsigned short)(0);
		*indices++ = (unsigned short)(1);
		*indices++ = (unsigned short)(2);

		*indices++ = (unsigned short)(0);
		*indices++ = (unsigned short)(2);
		*indices++ = (unsigned short)(3);
	}
	{
		std::string display = std::string(">") + commandLine;

		const Material *material = font->GetMaterial();
		const Technique *tech = material->GetTechnique();

		ModelGeometry *geom = new(FrameAlloc( sizeof(ModelGeometry) )) ModelGeometry;
		ModelVert *verts = (ModelVert*)FrameAlloc( sizeof(ModelVert)*display.size()*4 );
		unsigned short *indices = (unsigned short*)FrameAlloc( sizeof(unsigned short)*display.size()*6 );

		for (int i=0; i<tech->NumPasses();i++)
		{
			cmd->geom = geom;
			cmd->mat = material;
			cmd->tech = tech->GetPass(i);
			cmd->m_localToWorld = Vectormath::Aos::Transform3::identity();
			cmd->passIdx = RP_Debug2D;//cmd->tech->pass;
			cmd++;
			count++;
		}

		float x = 100.f, y = 100.f;
		int tot = font->Print( verts, indices, 0, &x, &y, display.c_str(), 0.1f );
		geom->m_indices = indices;
		geom->m_numIndices = tot * 6;
		geom->m_verts = verts;
		geom->m_numVerts = tot * 4;
	}
	return count;
}


void Console::RegisterFuntion( const char *name, console_function f )
{
	for (unsigned int i=0; i<consoleFunctions.size(); i++)
	{
		if ( consoleFunctions[i].name == name )
		{
			return;
		}
	}
	console_item_t item;
	item.name = name;
	item.function = f;
	consoleFunctions.push_back( item );
}

bool Console::IsEnabled()
{
	return enabled;
}
