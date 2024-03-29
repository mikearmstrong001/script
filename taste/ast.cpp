#include "ast.h"
#include <vector>
#include <stack>

inline int AddOp( std::vector<int> &oplist, int op )
{
	int pos = oplist.size();

	printf( "%04d: %s\n", pos, opnames[op] );

	oplist.push_back( op );
	return pos;
}

inline int AddOp( std::vector<int> &oplist, int op, const wchar_t *str )
{
	int len = wcslen( str );
	int pos = oplist.size();

	printf( "%04d: %s %S\n", pos, opnames[op], str );

	oplist.push_back( op );
	oplist.push_back( len );
	for (int i=0; i<len; i+=2)
	{
		oplist.push_back((str[i+1]<<16)|(str[i]<<0));
	}
	return pos;
}

inline int AddOp( std::vector<int> &oplist, int op, int v )
{
	int pos = oplist.size();

	printf( "%04d: %s %d\n", pos, opnames[op], v );

	oplist.push_back( op );
	oplist.push_back( v );
	return pos;
}

inline int AddOp( std::vector<int> &oplist, int op, int v0, int v1 )
{
	int pos = oplist.size();

	assert( op>=0 && op < OPC_MAX );
	printf( "%04d: %s %d %d\n", pos, opnames[op], v0, v1 );

	oplist.push_back( op );
	oplist.push_back( v0 );
	oplist.push_back( v1 );
	return pos;
}

inline int AddOp( std::vector<int> &oplist, int op, int v0, int v1, int v2 )
{
	int pos = oplist.size();

	printf( "%04d: %s %d %d %d\n", pos, opnames[op], v0, v1, v2 );

	oplist.push_back( op );
	oplist.push_back( v0 );
	oplist.push_back( v1 );
	oplist.push_back( v2 );
	return pos;
}

inline int AddOp( std::vector<int> &oplist, int op, std::vector<int> const &vars )
{
	int pos = oplist.size();

	printf( "%04d: %s %d", pos, opnames[op], vars.size() );
	for (unsigned int i=0; i<vars.size(); i++)
	{
		printf(" %d", vars[i] );
	}
	printf("\n");

	oplist.push_back( op );
	oplist.push_back( vars.size() );
	oplist.insert( oplist.end(), vars.begin(), vars.end() );
	return pos;
}

inline int AddOp( std::vector<int> &oplist, int op, float v )
{
	int pos = oplist.size();

	printf( "%04d: %s %f\n", pos, opnames[op], v );

	oplist.push_back( op );
	int iv;
	memcpy( &iv, &v, sizeof(iv) );
	oplist.push_back( iv );
	return pos;
}


void BinaryAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	m_left->Generate(oplist, frame, pkg);
	m_right->Generate(oplist, frame, pkg);
	int openum[] = { OPC_ADD, OPC_SUB, OPC_MUL, OPC_DIV, OPC_EQU, OPC_LSS, OPC_GTR };
	AddOp( oplist, openum[m_op] );
}

void IdentAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	StackEntry e;
	int index = frame.FindEntry( e, m_name );
	if ( index == 0x7fffffff )
	{
		VarDeclAst *var = pkg->FindVar( m_name.c_str() );
		if ( var && var->IsUserType() )
		{
			e.type = STRUCT;
			e.usertype = var->GetType();
		}
		else
		if ( var == NULL )
		{
			e.type = INTEGER;
		}
	}

	if ( m_index != 0 )
	{
		m_index->Generate( oplist, frame, pkg );
		if ( e.type == STRUCT )
		{
			int lookupName = Hash( m_identVec[0].c_str() );
			for (unsigned int i=1; i<m_identVec.size(); i++)
			{
				lookupName = lookupName ^ Hash( m_identVec[i].c_str() );
			}
			DefDecl *sd = pkg->FindStruct( e.usertype );
			int eleIndex = sd->FindElementIndex( lookupName );
			if ( index == 0x7fffffff )
			{
				AddOp(oplist,OPC_PUSHITEMGARRAY, Hash(m_name.c_str()), eleIndex );
			}
			else
			{
				AddOp(oplist,OPC_PUSHITEMARRAY, index, eleIndex );
			}
		}
		else
		{
			if ( index == 0x7fffffff )
			{
				AddOp( oplist, OPC_PUSHITEMGARRAY, Hash( m_name.c_str() ) );
			}
			else
			{
				AddOp( oplist, OPC_PUSHITEMARRAY, index );
			}
		}
	}
	else
	{
		if ( e.type == STRUCT )
		{
			int lookupName = Hash( m_identVec[0].c_str() );
			for (unsigned int i=1; i<m_identVec.size(); i++)
			{
				lookupName = lookupName ^ Hash( m_identVec[i].c_str() );
			}
			DefDecl *sd = pkg->FindStruct( e.usertype );
			int eleIndex = sd->FindElementIndex( lookupName );
			if ( index == 0x7fffffff )
			{
				AddOp(oplist,OPC_PUSHITEMG, 2, Hash(m_name.c_str()), eleIndex );
			}
			else
			{
				AddOp(oplist,OPC_PUSHITEM, 2, index, eleIndex );
			}
		}
		else
		{
			std::vector<int> lookup;
			lookup.push_back( 0 );
			for (unsigned int i=0; i<m_identVec.size(); i++)
			{
				lookup.push_back( Hash( m_identVec[i].c_str() ) );
			}
			if ( m_name == L"super" )
			{
				int index = frame.FindEntry( e, L"self" );
				lookup[0] = index;
				AddOp( oplist, OPC_PUSHSUPER, lookup );
			}
			else
			if ( index == 0x7fffffff )
			{
				lookup[0] = Hash( m_name.c_str() );
				AddOp( oplist, OPC_PUSHITEMG, lookup );
			}
			else
			{
				lookup[0] = index;
				AddOp( oplist, OPC_PUSHITEM, lookup );
			}
		}
	}
}

void ConstIntegerAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	AddOp( oplist, OPC_PUSHI, m_value );
}

void ConstFloatingPointAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	AddOp( oplist, OPC_PUSHF, m_value );
}

void ConstBooleanAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	AddOp( oplist, OPC_PUSHI, m_value ? 1 : 0 );
}

void ConstStringAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	AddOp( oplist, OPC_PUSHSTR, m_value.c_str() );
}


void NegateAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	m_child->Generate(oplist,frame, pkg);
	AddOp( oplist, OPC_NEG );
}

void VarDeclAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	if ( ((unsigned int)m_type) >= MAX_VARTYPE )
	{
		if ( frame.GetDepth() == 1 )
		{
			AddOp( oplist, OPC_MAKESTRUCTG, m_type, Hash(m_name.c_str()) );
		}
		else
		{
			int index = frame.AddEntry( m_name, STRUCT, m_type );
			AddOp( oplist, OPC_MAKESTRUCT, m_type, index );
		}
	}
	else
	{
		VARTYPE typemap[] = { INTEGER, INTEGER, INTEGER, FLOATINGPOINT, OBJECT, VOID, USERPTR, STRING
							};
		VARTYPE typemaparray[] = { INTEGERARRAY, INTEGERARRAY, INTEGERARRAY, FLOATINGPOINTARRAY, OBJECTARRAY, INTEGERARRAY, USERPTRARRAY, STRINGARRAY
							};
		if ( frame.GetDepth() == 1 )
		{
			AddOp( oplist, OPC_MAKEVARG, m_isarray ? typemaparray[m_type] : typemap[m_type], Hash( m_name.c_str() ), m_prototype.empty() ? 0 : Hash( m_prototype.c_str() ) );
		}
		else
		{
			int index = frame.AddEntry( m_name, m_isarray ? typemaparray[m_type] : typemap[m_type] );
			AddOp( oplist, OPC_MAKEVAR, m_isarray ? typemaparray[m_type] : typemap[m_type], index, m_prototype.empty() ? 0 : Hash( m_prototype.c_str() ) );
			if ( !m_prototype.empty() )
			{
				AddOp( oplist, OPC_PUSHITEM, 1, index );
				AddOp( oplist, OPC_CALLG, Hash( (m_prototype + L":~creator").c_str() ) );
			}
		}
	}
}

void VarDeclAst::GenerateConstructor( std::vector<int> &oplist, StackFrame &frame )
{
	VARTYPE typemap[] = { INTEGER, INTEGER, INTEGER, FLOATINGPOINT, OBJECT, VOID, USERPTR, STRING
						};
	VARTYPE typemaparray[] = { INTEGERARRAY, INTEGERARRAY, INTEGERARRAY, FLOATINGPOINTARRAY, OBJECTARRAY, INTEGERARRAY, USERPTRARRAY, STRINGARRAY
						};
	AddOp( oplist, OPC_CONSTRUCTVAR, m_isarray ? typemaparray[m_type] : typemap[m_type], Hash( m_name.c_str() ) );
}


void ProcDeclAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	frame.StartFunction(m_declaration.size());
	printf( "%S:\n", m_name.c_str() );

	VARTYPE typemap[] = { INTEGER, INTEGER, INTEGER, FLOATINGPOINT, OBJECT, USERPTR };
	for (unsigned int i=0; i<m_declaration.size(); i++)
	{
		frame.AddEntry( m_declaration[i].name, typemap[m_declaration[i].type] );
	}
	int patch0 = AddOp( oplist, OPC_PUSHENV, 0 );
	int startProcFrame = frame.PushFrame();
	for (unsigned int i=0; i<m_body.size(); i++)
	{
		m_body[i]->Generate(oplist,frame,pkg);
	}
	int sizeProcFrame = frame.PopFrame();
	AddOp( oplist, OPC_POPENV );
	oplist[patch0+1] = sizeProcFrame;

	AddOp( oplist, OPC_POP, (int)m_declaration.size() );
	AddOp( oplist, OPC_RET );
	frame.EndFunction();
}


void AssignAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	m_expr->Generate(oplist,frame,pkg);

	StackEntry e;
	int index = frame.FindEntry( e, m_name );
	if ( index == 0x7fffffff )
	{
		VarDeclAst *var = pkg->FindVar( m_name.c_str() );
		if ( var->IsUserType() )
		{
			e.type = STRUCT;
			e.usertype = var->GetType();
		}
	}

	if ( m_arrayIndex != 0 )
	{
		m_arrayIndex->Generate( oplist, frame, pkg );
		if ( e.type == STRUCT )
		{
			int lookupName = Hash( m_identVec[0].c_str() );
			for (unsigned int i=1; i<m_identVec.size(); i++)
			{
				lookupName = lookupName ^ Hash(m_identVec[i].c_str());
			}
			DefDecl *sd = pkg->FindStruct( e.usertype );
			int eleIndex = sd->FindElementIndex( lookupName );
			if ( index == 0x7fffffff )
			{
				AddOp(oplist,OPC_POPITEMGARRAY, Hash(m_name.c_str()), eleIndex );
			}
			else
			{
				AddOp(oplist,OPC_POPITEMARRAY, index, eleIndex );
			}
		}
		else
		if ( index == 0x7fffffff )
		{
			AddOp(oplist,OPC_POPITEMGARRAY, Hash( m_name.c_str() ) );
		}
		else
		{
			// check if struct
			AddOp(oplist,OPC_POPITEMARRAY, index );
		}
	}
	else
	{
		if ( e.type == STRUCT )
		{
			int lookupName = Hash( m_identVec[0].c_str() );
			for (unsigned int i=1; i<m_identVec.size(); i++)
			{
				lookupName = lookupName ^ Hash(m_identVec[i].c_str());
			}
			DefDecl *sd = pkg->FindStruct( e.usertype );
			int eleIndex = sd->FindElementIndex( lookupName );
			if ( index == 0x7fffffff )
			{
				AddOp(oplist,OPC_POPITEMG, 2, Hash(m_name.c_str()), eleIndex );
			}
			else
			{
				AddOp(oplist,OPC_POPITEM, 2, index, eleIndex );
			}
		}
		else
		{
			std::vector<int> lookup;
			lookup.push_back( 0 );
			for (unsigned int i=0; i<m_identVec.size(); i++)
			{
				lookup.push_back( Hash( m_identVec[i].c_str() ) );
			}
			if ( m_name == L"super" )
			{
				assert(0);
			}
			else
			if ( index == 0x7fffffff )
			{
				lookup[0] = Hash( m_name.c_str() );
				AddOp(oplist,OPC_POPITEMG, lookup );
			}
			else
			{
				lookup[0] = index;
				AddOp(oplist,OPC_POPITEM, lookup );
			}
		}
	}
}

void CallAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	StackEntry e;
	int index = frame.FindEntry( e, m_name );

	if ( m_identVec.size() )
	{
		std::vector<int> lookup;
		lookup.push_back( 0 );
		for (unsigned int i=0; i<(m_identVec.size()-1); i++)
		{
			lookup.push_back( Hash( m_identVec[i].c_str() ) );
		}
		if ( index == 0x7fffffff )
		{
			lookup[0] = Hash( m_name.c_str() );
			AddOp(oplist,OPC_PUSHITEMG, lookup );
		}
		else
		{
			lookup[0] = index;
			AddOp(oplist,OPC_PUSHITEM, lookup );
		}
	}

	for (unsigned int i=0; i<m_callExpr.size(); i++)
	{
		m_callExpr[i]->Generate(oplist,frame,pkg);
	}
	std::vector<int> lookup;
	lookup.push_back( 0 );
	for (unsigned int i=0; i<m_identVec.size(); i++)
	{
		lookup.push_back( Hash( m_identVec[i].c_str() ) );
	}

	if ( m_name == L"super" )
	{
		int index = frame.FindEntry( e, L"self" );
		lookup[0] = index;
		AddOp( oplist, OPC_PUSHSUPER, lookup );
	}
	else
	if ( index == 0x7fffffff )
	{
		lookup[0] = Hash( m_name.c_str() );
		AddOp(oplist,OPC_PUSHITEMG, lookup );
	}
	else
	{
		lookup[0] = index;
		AddOp(oplist,OPC_PUSHITEM, lookup );
	}
	AddOp( oplist, OPC_CALL );
	//AddOp( oplist, OPC_POP, (int)m_callExpr.size() );
}

void IfAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	unsigned int elsedest = (((unsigned int)this)+1);
	unsigned int enddest = (((unsigned int)this)+2);
	unsigned int fdest = m_else.size() ? elsedest : enddest;

	m_conditional->Generate(oplist, frame, pkg);
	int patch0 = AddOp( oplist, OPC_JF, 0 );

	int patch1 = AddOp( oplist, OPC_PUSHENV, 0 );
	int startIfFrame = frame.PushFrame();
	for ( unsigned int i=0; i<m_if.size(); i++)
	{
		m_if[i]->Generate(oplist,frame,pkg);
	}
	int sizeIfFrame = frame.PopFrame();
	AddOp( oplist, OPC_POPENV );
	oplist[patch1+1] = sizeIfFrame;

	if ( m_else.size() )
	{
		int patch2 = AddOp( oplist, OPC_JMP, 0 );

		oplist[patch0+1] = oplist.size();
		int patch3 = AddOp( oplist, OPC_PUSHENV, 0 );
		int startElseFrame = frame.PushFrame();
		for ( unsigned int i=0; i<m_else.size(); i++)
		{
			m_else[i]->Generate(oplist, frame,pkg);
		}
		int sizeElseFrame = frame.PopFrame();
		AddOp( oplist, OPC_POPENV );
		oplist[patch3+1] = sizeElseFrame;

		oplist[patch2+1] = oplist.size();
	}
	else
	{
		oplist[patch0+1] = oplist.size();
	}
}

void WhileAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	int startWhile = oplist.size();
	m_conditional->Generate(oplist,frame,pkg);
	int patchLoc0 = AddOp( oplist, OPC_JF, 0 );
		
	int patchLoc1 = AddOp( oplist, OPC_PUSHENV, 0 );
	int startWhileFrame = frame.PushFrame();
	for (unsigned int i=0; i<m_expr.size(); i++)
	{
		m_expr[i]->Generate(oplist,frame,pkg);
	}
	int sizeWhileFrame = frame.PopFrame();
	AddOp( oplist, OPC_POPENV );
	oplist[patchLoc1+1] = sizeWhileFrame;

	AddOp( oplist, OPC_JMP, startWhile );
	oplist[patchLoc0+1] = oplist.size();
}

void ReturnStat::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	//assert( !"call convention doesn't work for this, especially within other blocks stackframe needs function start vs current stack" );
	if ( m_expr )
	{
		m_expr->Generate( oplist, frame, pkg );
		AddOp( oplist, OPC_POPR );
	}
	int delta = frame.DeltaFrame();
	for (int i=0; i<delta; i++)
	{
		AddOp( oplist, OPC_POPENV );
	}
	AddOp( oplist, OPC_POP, frame.CurrentFunctionArgCount() );
	if ( m_expr )
	{
		AddOp( oplist, OPC_PUSHR );
	}
	AddOp( oplist, OPC_RET );
}

void BlockAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	int patchLoc0 = AddOp( oplist, OPC_PUSHENV, 0 );
	int startBlockFrame = frame.PushFrame();
	for (unsigned int i=0; i<m_expr.size(); i++)
	{
		m_expr[i]->Generate(oplist,frame,pkg);
	}
	int sizeBlockFrame = frame.PopFrame();
	AddOp( oplist, OPC_PUSHENV, sizeBlockFrame );
	oplist[patchLoc0+1] = sizeBlockFrame;
}

void EmbedDeclAst::Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg )
{
	if ( frame.GetDepth() == 1 )
	{
		AddOp( oplist, OPC_MAKESTRUCTG, Hash(m_name.c_str()), Hash( m_type.c_str() ) );
	}
	else
	{
		int index = frame.AddEntry( m_name, STRUCT, m_type.c_str() );
		AddOp( oplist, OPC_MAKESTRUCT, Hash( m_type.c_str() ), index );
	}
}

int DefDecl::FindElementIndex( int hashName )
{
	for ( unsigned int i=0; i<m_flattenedProps.size(); i++ )
	{
		if ( m_flattenedProps[i].hashName == hashName )
			return i;
	}
	return -1;
}

void DefDecl::GenerateDef( std::vector<int> &oplist, vmstate &state, class Package *pkg )
{
	StackFrame frame;
	frame.PushFrame();

	{
		int hash = Hash( (m_name + std::wstring(L":~type_creator")).c_str() );
		printf( "%S:~type_creator %d\n", m_name.c_str(), hash);
		state.globals[hash].type = VMFUNCTION;
		state.globals[hash].i = oplist.size();
		for ( unsigned int i=0; i<m_embeds.size(); i++)
		{
			AddOp( oplist, OPC_PUSHTOP );
			AddOp( oplist, OPC_CALLG, Hash( (m_embeds[i]->GetType() + std::wstring(L":~type_creator")).c_str() ) );
		}
		for ( unsigned int i=0; i<m_procs.size(); i++)
		{
			int function_hash = Hash( (m_name + L":" + m_procs[i]->GetName()).c_str() );
			AddOp( oplist, OPC_CONSTRUCTVARFROMGLOBAL, Hash(m_procs[i]->GetName().c_str()), function_hash );
		}
		{
			int function_hash = Hash( L"_implements" );
			AddOp( oplist, OPC_CONSTRUCTVARFROMGLOBAL, Hash( L"implements" ), function_hash );
		}
		AddOp( oplist, OPC_POP, 1 );
		AddOp( oplist, OPC_RET );
	}

	{
		int hash = Hash( (m_name + std::wstring(L":~creator")).c_str() );
		printf( "%S:~creator %d\n", m_name.c_str(), hash);
		state.globals[hash].type = VMFUNCTION;
		state.globals[hash].i = oplist.size();
		if ( !m_extends.empty() )
		{
			AddOp( oplist, OPC_PUSHTOP );
			AddOp( oplist, OPC_CALLG, Hash( (m_extends + std::wstring(L":~creator")).c_str() ) );
		}
		for ( unsigned int i=0; i<m_embeds.size(); i++)
		{
			AddOp( oplist, OPC_PUSHTOP );
			AddOp( oplist, OPC_CALLG, Hash( (m_embeds[i]->GetType() + std::wstring(L":~creator")).c_str() ) );
		}
		for ( unsigned int i=0; i<m_varDecls.size(); i++)
		{
			m_varDecls[i]->GenerateConstructor(oplist,frame);
		}
		AddOp( oplist, OPC_POP, 1 );
		AddOp( oplist, OPC_RET );
	}

	for ( unsigned int i=0; i<m_procs.size(); i++)
	{
		int hash = Hash( (m_name + L":" + m_procs[i]->GetName()).c_str() );
		state.globals[hash].type = VMFUNCTION;
		state.globals[hash].i = oplist.size();
		DeclInfo self;
		self.type = OBJECT;
		self.name = L"self";
		m_procs[i]->PrependDeclInfo( self );
		m_procs[i]->Generate(oplist,frame,pkg);
	}
}

void DefDecl::GenerateProps( class Package *pkg, vmstate &state )
{
	if ( m_flattenedProps.size() )
		return;

	for (unsigned int i=0; i<m_varDecls.size(); i++)
	{
		if ( (unsigned int)m_varDecls[i]->GetType() >= OPC_MAX )
		{
			DefDecl *s = pkg->FindStruct( m_varDecls[i]->GetType() );
			s->GenerateProps( pkg, state );
			std::vector< Element > const &childProps = s->GetProps();
			for (unsigned int j=0; j<childProps.size(); j++)
			{
				Element e;
				e.hashName = Hash( m_varDecls[i]->GetName().c_str() ) ^ childProps[j].hashName;
				e.varInfo = childProps[j].varInfo;
				m_flattenedProps.push_back( e );
			}
		}
		else
		{
			Element e;
			e.hashName = Hash( m_varDecls[i]->GetNameWC() );
			e.varInfo = m_varDecls[i];
			m_flattenedProps.push_back( e );
		}
	}
	VARTYPE typemap[] = { INTEGER, INTEGER, INTEGER, FLOATINGPOINT, OBJECT, VOID, USERPTR, STRING
						};
	vmstructprops *vmstruct = new vmstructprops;
	state.structProps[Hash( m_name.c_str() )] = vmstruct;
	for (unsigned int i=0; i<m_flattenedProps.size(); i++)
	{
		vmelement vme;
		vme.itemName = m_flattenedProps[i].hashName;
		vme.itemType = typemap[m_flattenedProps[i].varInfo->GetType()];
		vmstruct->properties.push_back( vme );
	}
}

DefDecl *Package::FindStruct( const wchar_t *name )
{
	for (unsigned int i=0; i<m_structs.size(); i++)
	{
		if ( m_structs[i]->GetName() == name )
		{
			return m_structs[i];
		}
	}
	return NULL;
}

DefDecl *Package::FindStruct( int name )
{
	for (unsigned int i=0; i<m_structs.size(); i++)
	{
		if ( Hash(m_structs[i]->GetName().c_str()) == name )
		{
			return m_structs[i];
		}
	}
	return NULL;
}

VarDeclAst *Package::FindVar( const wchar_t *name )
{
	for (unsigned int i=0; i<m_varDecls.size(); i++)
	{
		if ( m_varDecls[i]->GetName() == name )
			return m_varDecls[i];
	}
	return NULL;
}

VarDeclAst *Package::FindVar( int name )
{
	for (unsigned int i=0; i<m_varDecls.size(); i++)
	{
		if ( Hash(m_varDecls[i]->GetName().c_str()) == name )
			return m_varDecls[i];
	}
	return NULL;
}

ProcDeclAst *Package::FindProc( const wchar_t *name )
{
	for (unsigned int i=0; i<m_procs.size(); i++)
	{
		if ( m_procs[i]->GetName() == name )
			return m_procs[i];
	}
	return NULL;
}

ProcDeclAst *Package::FindProc( int name )
{
	for (unsigned int i=0; i<m_procs.size(); i++)
	{
		if ( Hash(m_procs[i]->GetName().c_str()) == name )
			return m_procs[i];
	}
	return NULL;
}


void Package::Generate()
{
	StackFrame frame;
	frame.PushFrame();

	vmstate state;

	for ( unsigned int i=0; i<m_defs.size(); i++)
	{
		m_structs[i]->GenerateProps( this, state );
	}


	std::vector<int> oplist;
	for ( unsigned int i=0; i<m_defs.size(); i++)
	{
		AddOp( oplist, OPC_MAKEVARG, OBJECT, Hash( m_defs[i]->GetName().c_str() ), m_defs[i]->GetExtends().empty() ? 0 : Hash( m_defs[i]->GetExtends().c_str() ) );
		AddOp( oplist, OPC_PUSHITEMG, 1, Hash( m_defs[i]->GetName().c_str() ) );
		int hash = Hash( (m_defs[i]->GetName() + std::wstring(L":~type_creator")).c_str() );
		AddOp( oplist, OPC_CALLG, hash );
		//AddOp( oplist, OPC_POP, 1 );
	}
	for ( unsigned int i=0; i<m_varDecls.size(); i++)
	{
		m_varDecls[i]->Generate(oplist,frame,this);
	}
	for ( unsigned int i=0; i<m_interfaces.size(); i++)
	{
		AddOp( oplist, OPC_MAKEVARG, INTEGER, Hash( m_interfaces[i]->GetName().c_str() ), 0 );
		AddOp( oplist, OPC_PUSHI, Hash( m_interfaces[i]->GetName().c_str() ) );
		AddOp( oplist, OPC_POPITEMG, 1, Hash( m_interfaces[i]->GetName().c_str() ) );

		vminterface &iface = state.ifaces.add( Hash( m_interfaces[i]->GetName().c_str() ) );
		std::vector< ProcDefDeclAst* > const &procs = m_interfaces[i]->GetProcs();
		for ( unsigned int j=0; j<procs.size(); j++)
		{
			iface.interfaceFunctions.push_back( Hash( procs[j]->GetName().c_str() ) );
		}
	}
	AddOp( oplist, OPC_RET );

	for ( unsigned int i=0; i<m_defs.size(); i++)
	{
		m_defs[i]->GenerateDef( oplist, state, this );
	}

	int main = -1;
	for ( unsigned int i=0; i<m_procs.size(); i++)
	{
		if ( m_procs[i]->GetName() == L"Main" )
			main = oplist.size();
		int hash = Hash( m_procs[i]->GetName().c_str() );
		state.globals[hash].type = VMFUNCTION;
		state.globals[hash].i = oplist.size();
		m_procs[i]->Generate(oplist,frame,this);
	}
	RunVMExp( &oplist[0], oplist.size(), 0, state );
	RunVMExp( &oplist[0], oplist.size(), main, state );
	frame.PopFrame();
}

