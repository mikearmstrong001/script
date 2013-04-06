#pragma once

#include <vector>
#include <string>
#include <assert.h>
#include <map>
#include "vm.h"



struct DeclInfo
{
	int type;
	std::wstring name;
};


typedef std::vector<std::wstring> IdentVec;
typedef std::vector<DeclInfo> DeclVec;
typedef std::vector<class AstBase*> AstVec;


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

struct StackEntry
{
	std::wstring name;
	VARTYPE type;
	int depth;
};

class StackFrame
{
	std::vector< StackEntry > m_active;
	std::vector< int > m_return;
	std::vector< int > m_functionCallDepth;
	std::vector< int > m_functionCallParams;
public:
	int GetActiveCount() const
	{
		return m_active.size();
	}

	int GetDepth() const
	{
		return m_return.size();
	}

	void StartFunction( int args )
	{
		m_functionCallDepth.push_back( m_return.size() );
		m_functionCallParams.push_back( args );
	}

	void EndFunction()
	{
		m_functionCallDepth.pop_back();
		m_functionCallParams.pop_back();
	}

	int DeltaFrame() const
	{
		return m_return.size() - m_functionCallDepth[m_functionCallDepth.size()-1];
	}

	int CurrentFunctionArgCount() const
	{
		return m_functionCallParams[m_functionCallParams.size()-1];
	}

	int PushFrame()
	{
		int pos = m_active.size();
		m_return.push_back( pos );
		return pos;
	}

	int AddEntry( const std::wstring &n, VARTYPE type )
	{
		StackEntry e;
		e.name = n;
		e.type = type;
		e.depth = m_return.size();
		int rel = m_active.size() - m_return[m_return.size()-1];
		m_active.push_back( e );
		return rel;
	}

	int FindEntry( StackEntry &e, const std::wstring &n )
	{
		for (int i=m_active.size()-1; i>=0; i--)
		{
			if ( m_active[i].name == n )
			{
				e = m_active[i];
				/*
				if ( m_active[i].depth == 1 )
				{
					// global
					return i;
				}
				else*/
				{
					int rel = i - m_return[m_return.size()-1];
					return rel;
				}
			}
		}
		return 0x7fffffff;
	}

	int PopFrame()
	{
		int size = m_active.size() - m_return[m_return.size()-1];
		m_active.resize( m_return[m_return.size()-1] );
		m_return.pop_back();
		return size;
	}
};

class AstBase
{
protected:
	static void PrintNameIndent( const char *name, int indent )
	{
		char spaces[] = "                       ";
		printf( "%s%s\n", &spaces[sizeof(spaces)-indent-1], name );
	}

	static const char* IndentString( int indent )
	{
		static char spaces[] = "                       ";
		return &spaces[sizeof(spaces)-indent-1];
	}
public:

	virtual ~AstBase() {};
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame ) = 0;
};

class DataAst : public AstBase
{
	std::wstring m_varname;
	DataAst *m_parent;
	std::wstring m_subname;
	int m_type;
	union
	{
		int intval;
		float fval;
	};
	std::vector<DataAst*> m_kids;
public:
	DataAst( DataAst *parent ) : m_parent(parent), m_type(-1) {}
	DataAst( const wchar_t *varname, DataAst *parent ) : m_varname(varname), m_parent(parent), m_type(-1) {}
	void SetName( wchar_t *name ) { m_subname = name; }
	void SetType( int type ) { m_type = type; }
	void AddKid( DataAst *c ) { m_kids.push_back( c ); }
	void SetValue( wchar_t *v )
	{
		if ( m_type == 2 )
		{
			swscanf(v, L"%d", &intval);
		}
		else
		if ( m_type == 3 )
		{
			swscanf(v, L"%f", &fval);
		}
	}
	void GenerateNameList( std::wstring &varName, std::vector<int> &names )
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
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
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
};

class BinaryAst : public AstBase
{
	AstBase *m_left, *m_right;
	int m_op;
public:

	BinaryAst() : m_left( 0 ), m_right( 0 ), m_op(-1)
	{
	}
	BinaryAst( AstBase *l, int o, AstBase *r ) : m_left( l ), m_right( r ), m_op(o)
	{
	}

	void SetLeft( AstBase *l ) { m_left = l; }
	void SetRight( AstBase *r ) { m_right = r; }
	void SetOp( int o ) { m_op = o; }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
	{
		m_left->Generate(oplist, frame);
		m_right->Generate(oplist, frame);
		int openum[] = { OPC_ADD, OPC_SUB, OPC_MUL, OPC_DIV, OPC_EQU, OPC_LSS, OPC_GTR };
		AddOp( oplist, openum[m_op] );
	}
};

class ValueAst : public AstBase
{
};

class IdentAst : public ValueAst
{
	std::wstring m_name;
	IdentVec m_identVec;
	int		m_index;
public:
	IdentAst( const wchar_t *token ) : m_name(token), m_index(-1) {}
	void SetArrayIndex( const wchar_t *token ) { swscanf(token, L"%d", &m_index); }
	void SetMemberList( const IdentVec &v ) { m_identVec = v; }
	std::wstring const &GetName() const { return m_name; }
	wchar_t const *GetNameWC() const { return m_name.c_str(); }
	int GetArrayIndex() const { return m_index; }
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
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
};

class ConstIntegerAst : public ValueAst
{
	int m_value;
public:
	ConstIntegerAst( const wchar_t *token )
	{
		swscanf(token, L"%d", &m_value);
	}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
	{
		AddOp( oplist, OPC_PUSHI, m_value );
	}
};

class ConstFloatingPointAst : public ValueAst
{
	float m_value;
public:
	ConstFloatingPointAst( const wchar_t *token )
	{
		swscanf(token, L"%f", &m_value);
	}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
	{
		AddOp( oplist, OPC_PUSHF, m_value );
	}
};

class ConstBooleanAst : public ValueAst
{
	bool m_value;
public:
	ConstBooleanAst( bool v ) : m_value(v) {}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
	{
		AddOp( oplist, OPC_PUSHI, m_value ? 1 : 0 );
	}
};

class NegateAst : public AstBase
{
	AstBase *m_child;
public:
	NegateAst( AstBase *child ) : m_child(child) {}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
	{
		m_child->Generate(oplist,frame);
		AddOp( oplist, OPC_NEG );
	}
};

class ExprAst : public AstBase
{
};

class VarDeclAst : public AstBase
{
	int m_type;
	bool m_isarray;
	std::wstring m_name;
	std::wstring m_prototype;
	AstBase *m_expr;
	DataAst *m_data;
public:
	VarDeclAst() : m_type(0), m_isarray(false), m_expr(0), m_data(0) {}

	void SetType( int type ) { m_type = type; }
	void SetPrototype( const wchar_t *name ) { m_prototype = name; }
	void SetName( const wchar_t *name ) { m_name = name; }
	void SetExpr( AstBase *expr ) { m_expr = expr; }
	void SetData( DataAst *data ) { m_data = data; }
	void SetIsArray() { m_isarray = true; }
	wchar_t const *GetNameWC() const { return m_name.c_str(); }
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
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
	virtual void GenerateConstructor( std::vector<int> &oplist, StackFrame &frame )
	{
		VARTYPE typemap[] = { INTEGER, INTEGER, INTEGER, FLOATINGPOINT, OBJECT, 
							};
		VARTYPE typemaparray[] = { INTEGERARRAY, INTEGERARRAY, INTEGERARRAY, FLOATINGPOINTARRAY, OBJECTARRAY, 
							};
		AddOp( oplist, OPC_CONSTRUCTVAR, m_isarray ? typemaparray[m_type] : typemap[m_type], Hash( m_name.c_str() ) );
	}
};

class ProcDeclAst : public AstBase
{
	int m_returnType;
	std::wstring m_name;
	std::vector< AstBase* > m_body;
	DeclVec m_declaration;
public:

	void SetReturnType( int type ) { m_returnType = type; }
	void SetName( const wchar_t *name ) { m_name = name; }
	void SetDeclaration( const DeclVec &v ) { m_declaration = v; }
	void AddBody( AstBase *e ) { m_body.push_back( e ); }

	std::wstring const &GetName() { return m_name; }

	void PrependDeclInfo( const DeclInfo &info ) { m_declaration.insert( m_declaration.begin(), info ); }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
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
};

class AssignAst : public AstBase
{
	std::wstring m_name;
	int m_arrayIndex;
	IdentVec m_identVec;
	AstBase *m_expr;
public:

	AssignAst( const wchar_t *token, int arrayindex, const IdentVec &v, AstBase *expr ) : m_name(token), m_arrayIndex(arrayindex), m_identVec(v), m_expr(expr) {}

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
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
};

class CallAst : public AstBase
{
	AstVec m_callExpr;
	std::wstring m_name;
	IdentVec m_identVec;
public:

	CallAst( const wchar_t *token, const IdentVec &v, AstVec &expr ) : m_name(token) , m_identVec(v), m_callExpr(expr) {}

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
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
};

class IfAst : public AstBase
{
	AstBase *m_conditional;
	std::vector<AstBase *> m_if;
	std::vector<AstBase *> m_else;

public:

	IfAst( AstBase *conditional ) : m_conditional(conditional) {}

	void AddIf( AstBase *i ) { m_if.push_back( i ); }
	void AddElse( AstBase *e ) { m_else.push_back( e ); }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
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
};

class WhileAst : public AstBase
{
	AstBase *m_conditional;
	std::vector<AstBase *> m_expr;

public:

	WhileAst( AstBase *conditional ) : m_conditional(conditional) {}

	void AddExpr( AstBase *i ) { m_expr.push_back( i ); }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
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
};

class ReturnStat : public AstBase
{
	AstBase *m_expr;
public:
	ReturnStat( AstBase *e ) : m_expr(e) {}

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
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
};

class BlockAst : public AstBase
{
	std::vector<AstBase *> m_expr;

public:

	void AddExpr( AstBase *i ) { m_expr.push_back( i ); }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame )
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
};

class DefDecl
{
	std::wstring m_name;
	std::wstring m_extends;
	std::vector< VarDeclAst* > m_varDecls;
	std::vector< ProcDeclAst* > m_procs;
public:

	DefDecl( const wchar_t *name ) : m_name(name) {}

	void SetExtends( const wchar_t *ex ) { m_extends = ex; }
	void AddVarDecl( VarDeclAst *v ) { m_varDecls.push_back(v); }
	void AddProcDecl( ProcDeclAst *p ) { m_procs.push_back(p); }

	std::wstring const &GetName() { return m_name; }
	std::wstring const &GetExtends() { return m_extends; }

	virtual void Generate( std::vector<int> &oplist, vmstate &state )
	{
		StackFrame frame;
		frame.PushFrame();

		{
			printf( "%S:~type_creator\n", m_name.c_str());
			int hash = Hash( (m_name + std::wstring(L":~type_creator")).c_str() );
			state.globals[hash].type = VMFUNCTION;
			state.globals[hash].i = oplist.size();
			for ( unsigned int i=0; i<m_procs.size(); i++)
			{
				int function_hash = Hash( (m_name + L":" + m_procs[i]->GetName()).c_str() );
				AddOp( oplist, OPC_CONSTRUCTVARFROMGLOBAL, Hash(m_procs[i]->GetName().c_str()), function_hash );
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
};

class Package
{
	std::wstring m_name;
	std::vector< VarDeclAst* > m_varDecls;
	std::vector< ProcDeclAst* > m_procs;
	std::vector< DefDecl* > m_defs;

public:

	Package( const wchar_t *name ) : m_name(name) {}

	void AddDefDecl( DefDecl *v ) { m_defs.push_back(v); }
	void AddVarDecl( VarDeclAst *v ) { m_varDecls.push_back(v); }
	void AddProcDecl( ProcDeclAst *p ) { m_procs.push_back(p); }

	void Generate()
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
		RunVM( &oplist[0], oplist.size(), 0, state );
		RunVM( &oplist[0], oplist.size(), main, state );
		frame.PopFrame();
	}
};
