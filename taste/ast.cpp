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

	printf( "%04d: %s", pos, opnames[op] );
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

void DataAst::GenerateNameList( std::wstring &varName, std::vector<int> &names )
{
	if ( m_parent )
	{
		m_parent->GenerateNameList( varName, names );
		names.push_back( Hash( m_subname.c_str() ) );
	}
	else
	{
		varName = m_varname;
		names.push_back( Hash( m_varname.c_str() ) );
	}
}

void DataAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	if ( m_type != -1 )
	{
		if ( m_type == 2 )
		{
			AddOp( oplist, OPC_PUSHI, intval );
		}
		else
		if ( m_type == 3 )
		{
			AddOp( oplist, OPC_PUSHF, fval );
		}
		else
		{
			assert(0);
		}
		std::wstring rootName;
		std::vector<int> nameList;
		GenerateNameList( rootName, nameList );
		StackEntry e;
		int index = frame.FindEntry( e, rootName );
		if ( index == 0x7fffffff )
		{
			AddOp(oplist,OPC_POPITEMG, nameList );
		}
		else
		{
			nameList[0] = index;
			AddOp(oplist,OPC_POPITEM, nameList );
		}
	}
	for (unsigned int i=0; i<m_kids.size(); i++)
	{
		m_kids[i]->Generate( oplist, frame );
	}
}

void BinaryAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	m_left->Generate(oplist, frame);
	m_right->Generate(oplist, frame);
	int openum[] = { OPC_ADD, OPC_SUB, OPC_MUL, OPC_DIV, OPC_EQU, OPC_LSS, OPC_GTR };
	AddOp( oplist, openum[m_op] );
}

void IdentAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	StackEntry e;
	int index = frame.FindEntry( e, m_name );
	if ( m_index != -1 )
	{
		if ( index == 0x7fffffff )
		{
			AddOp( oplist, OPC_PUSHITEMGARRAY, Hash( m_name.c_str() ), m_index );
		}
		else
		{
			AddOp( oplist, OPC_PUSHITEMARRAY, index, m_index );
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

void ConstIntegerAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	AddOp( oplist, OPC_PUSHI, m_value );
}

void ConstFloatingPointAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	AddOp( oplist, OPC_PUSHF, m_value );
}

void ConstBooleanAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	AddOp( oplist, OPC_PUSHI, m_value ? 1 : 0 );
}

void NegateAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	m_child->Generate(oplist,frame);
	AddOp( oplist, OPC_NEG );
}

void VarDeclAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	VARTYPE typemap[] = { INTEGER, INTEGER, INTEGER, FLOATINGPOINT, OBJECT, 
						};
	VARTYPE typemaparray[] = { INTEGERARRAY, INTEGERARRAY, INTEGERARRAY, FLOATINGPOINTARRAY, OBJECTARRAY, 
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
			AddOp( oplist, OPC_CALLG, Hash( (m_prototype + L"~:creator").c_str() ) );
			//AddOp( oplist, OPC_POP, 1 );
		}
	}
	if ( m_expr )
	{
		m_expr->Generate(oplist,frame);
		StackEntry e;
		int index = frame.FindEntry( e, m_name );
		if ( index == 0x7fffffff )
		{
			AddOp( oplist, OPC_POPITEMG, 1, Hash( m_name.c_str() ) );
		}
		else
		{
			AddOp( oplist, OPC_POPITEM, 1, index );
		}
	}
	if ( m_data )
	{
		m_data->Generate(oplist,frame);
	}
}

void VarDeclAst::GenerateConstructor( std::vector<int> &oplist, StackFrame &frame )
{
	VARTYPE typemap[] = { INTEGER, INTEGER, INTEGER, FLOATINGPOINT, OBJECT, 
						};
	VARTYPE typemaparray[] = { INTEGERARRAY, INTEGERARRAY, INTEGERARRAY, FLOATINGPOINTARRAY, OBJECTARRAY, 
						};
	AddOp( oplist, OPC_CONSTRUCTVAR, m_isarray ? typemaparray[m_type] : typemap[m_type], Hash( m_name.c_str() ) );
}


void ProcDeclAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	frame.StartFunction(m_declaration.size());
	printf( "%S:\n", m_name.c_str() );

	VARTYPE typemap[] = { INTEGER, INTEGER, INTEGER, FLOATINGPOINT, OBJECT };
	for (unsigned int i=0; i<m_declaration.size(); i++)
	{
		frame.AddEntry( m_declaration[i].name, typemap[m_declaration[i].type] );
	}
	int patch0 = AddOp( oplist, OPC_PUSHENV, 0 );
	int startProcFrame = frame.PushFrame();
	for (unsigned int i=0; i<m_body.size(); i++)
	{
		m_body[i]->Generate(oplist,frame);
	}
	int sizeProcFrame = frame.PopFrame();
	AddOp( oplist, OPC_POPENV );
	oplist[patch0+1] = sizeProcFrame;

	AddOp( oplist, OPC_POP, (int)m_declaration.size() );
	AddOp( oplist, OPC_RET );
	frame.EndFunction();
}


void AssignAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	m_expr->Generate(oplist,frame);
	StackEntry e;
	int index = frame.FindEntry( e, m_name );
	if ( m_arrayIndex >=0 )
	{
		if ( index == 0x7fffffff )
		{
			AddOp(oplist,OPC_POPITEMGARRAY, Hash( m_name.c_str() ), m_arrayIndex );
		}
		else
		{
			AddOp(oplist,OPC_POPITEMARRAY, index, m_arrayIndex );
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

void CallAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	for (unsigned int i=0; i<m_callExpr.size(); i++)
	{
		m_callExpr[i]->Generate(oplist,frame);
	}
	std::vector<int> lookup;
	lookup.push_back( 0 );
	for (unsigned int i=0; i<m_identVec.size(); i++)
	{
		lookup.push_back( Hash( m_identVec[i].c_str() ) );
	}

	StackEntry e;
	int index = frame.FindEntry( e, m_name );
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

void IfAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	unsigned int elsedest = (((unsigned int)this)+1);
	unsigned int enddest = (((unsigned int)this)+2);
	unsigned int fdest = m_else.size() ? elsedest : enddest;

	m_conditional->Generate(oplist, frame);
	int patch0 = AddOp( oplist, OPC_JF, 0 );

	int patch1 = AddOp( oplist, OPC_PUSHENV, 0 );
	int startIfFrame = frame.PushFrame();
	for ( unsigned int i=0; i<m_if.size(); i++)
	{
		m_if[i]->Generate(oplist,frame);
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
			m_else[i]->Generate(oplist, frame);
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

void WhileAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	int startWhile = oplist.size();
	m_conditional->Generate(oplist,frame);
	int patchLoc0 = AddOp( oplist, OPC_JF, 0 );
		
	int patchLoc1 = AddOp( oplist, OPC_PUSHENV, 0 );
	int startWhileFrame = frame.PushFrame();
	for (unsigned int i=0; i<m_expr.size(); i++)
	{
		m_expr[i]->Generate(oplist,frame);
	}
	int sizeWhileFrame = frame.PopFrame();
	AddOp( oplist, OPC_POPENV );
	oplist[patchLoc1+1] = sizeWhileFrame;

	AddOp( oplist, OPC_JMP, startWhile );
	oplist[patchLoc0+1] = oplist.size();
}

void ReturnStat::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	//assert( !"call convention doesn't work for this, especially within other blocks stackframe needs function start vs current stack" );
	if ( m_expr )
	{
		m_expr->Generate( oplist, frame );
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

void BlockAst::Generate( std::vector<int> &oplist, StackFrame &frame )
{
	int patchLoc0 = AddOp( oplist, OPC_PUSHENV, 0 );
	int startBlockFrame = frame.PushFrame();
	for (unsigned int i=0; i<m_expr.size(); i++)
	{
		m_expr[i]->Generate(oplist,frame);
	}
	int sizeBlockFrame = frame.PopFrame();
	AddOp( oplist, OPC_PUSHENV, sizeBlockFrame );
	oplist[patchLoc0+1] = sizeBlockFrame;
}

void DefDecl::Generate( std::vector<int> &oplist, vmstate &state )
{
	StackFrame frame;
	frame.PushFrame();

	{
		int hash = Hash( (m_name + std::wstring(L":~type_creator")).c_str() );
		printf( "%S:~type_creator %d\n", m_name.c_str(), hash);
		state.globals[hash].type = VMFUNCTION;
		state.globals[hash].i = oplist.size();
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
		printf( "%S:~creator\n", m_name.c_str());
		int hash = Hash( (m_name + std::wstring(L":~creator")).c_str() );
		state.globals[hash].type = VMFUNCTION;
		state.globals[hash].i = oplist.size();
		if ( !m_extends.empty() )
		{
			AddOp( oplist, OPC_PUSHTOP );
			AddOp( oplist, OPC_CALLG, Hash( (m_extends + std::wstring(L":~creator")).c_str() ) );
			//AddOp( oplist, OPC_POP, 1 );
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
		m_procs[i]->Generate(oplist,frame);
	}
}

void Package::Generate()
{
	StackFrame frame;
	frame.PushFrame();

	vmstate state;

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
		m_varDecls[i]->Generate(oplist,frame);
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
		m_defs[i]->Generate( oplist, state );
	}

	int main = -1;
	for ( unsigned int i=0; i<m_procs.size(); i++)
	{
		if ( m_procs[i]->GetName() == L"Main" )
			main = oplist.size();
		int hash = Hash( m_procs[i]->GetName().c_str() );
		state.globals[hash].type = VMFUNCTION;
		state.globals[hash].i = oplist.size();
		m_procs[i]->Generate(oplist,frame);
	}
	RunVMExp( &oplist[0], oplist.size(), 0, state );
	RunVMExp( &oplist[0], oplist.size(), main, state );
	frame.PopFrame();
}

