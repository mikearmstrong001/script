#pragma once

#include <vector>
#include <string>
#include <assert.h>
#include <map>
#include "vm.h"

enum BINOP
{
	BINOP_ADD,
	BINOP_SUB,
	BINOP_MUL,
	BINOP_DIV,
	BINOP_EQU,
	BINOP_LSS,
	BINOP_GTR,
	BINOP_LEQ,
	BINOP_GEQ,
};

struct DeclInfo
{
	int type;
	std::wstring name;
};


typedef std::vector<std::wstring> IdentVec;
typedef std::vector<DeclInfo> DeclVec;
typedef std::vector<class AstBase*> AstVec;


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
	void GenerateNameList( std::wstring &varName, std::vector<int> &names );
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
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

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
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
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
};

class ConstIntegerAst : public ValueAst
{
	int m_value;
public:
	ConstIntegerAst( const wchar_t *token )
	{
		swscanf(token, L"%d", &m_value);
	}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
};

class ConstFloatingPointAst : public ValueAst
{
	float m_value;
public:
	ConstFloatingPointAst( const wchar_t *token )
	{
		swscanf(token, L"%f", &m_value);
	}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
};

class ConstBooleanAst : public ValueAst
{
	bool m_value;
public:
	ConstBooleanAst( bool v ) : m_value(v) {}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
};

class NegateAst : public AstBase
{
	AstBase *m_child;
public:
	NegateAst( AstBase *child ) : m_child(child) {}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
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
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
	virtual void GenerateConstructor( std::vector<int> &oplist, StackFrame &frame );
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

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
};

class AssignAst : public AstBase
{
	std::wstring m_name;
	int m_arrayIndex;
	IdentVec m_identVec;
	AstBase *m_expr;
public:

	AssignAst( const wchar_t *token, int arrayindex, const IdentVec &v, AstBase *expr ) : m_name(token), m_arrayIndex(arrayindex), m_identVec(v), m_expr(expr) {}

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
};

class CallAst : public AstBase
{
	AstVec m_callExpr;
	std::wstring m_name;
	IdentVec m_identVec;
public:

	CallAst( const wchar_t *token, const IdentVec &v, AstVec &expr ) : m_name(token) , m_identVec(v), m_callExpr(expr) {}

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
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

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
};

class WhileAst : public AstBase
{
	AstBase *m_conditional;
	std::vector<AstBase *> m_expr;

public:

	WhileAst( AstBase *conditional ) : m_conditional(conditional) {}

	void AddExpr( AstBase *i ) { m_expr.push_back( i ); }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
};

class ReturnStat : public AstBase
{
	AstBase *m_expr;
public:
	ReturnStat( AstBase *e ) : m_expr(e) {}

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
};

class BlockAst : public AstBase
{
	std::vector<AstBase *> m_expr;

public:

	void AddExpr( AstBase *i ) { m_expr.push_back( i ); }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame );
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

	virtual void Generate( std::vector<int> &oplist, vmstate &state );
};

class ProcDefDeclAst
{
	int m_returnType;
	std::wstring m_name;
	DeclVec m_declaration;
public:

	void SetReturnType( int type ) { m_returnType = type; }
	void SetName( const wchar_t *name ) { m_name = name; }
	void SetDeclaration( const DeclVec &v ) { m_declaration = v; }

	std::wstring const &GetName() { return m_name; }

	void PrependDeclInfo( const DeclInfo &info ) { m_declaration.insert( m_declaration.begin(), info ); }
};


class InterfaceDecl
{
	std::wstring m_name;
	std::wstring m_extends;
	std::vector< ProcDefDeclAst* > m_procs;
public:

	InterfaceDecl( const wchar_t *name ) : m_name(name) {}

	void SetExtends( const wchar_t *ex ) { m_extends = ex; }
	void AddProcDecl( ProcDefDeclAst *p ) { m_procs.push_back(p); }

	std::wstring const &GetName() { return m_name; }
	std::wstring const &GetExtends() { return m_extends; }
};

class Package
{
	std::wstring m_name;
	std::vector< VarDeclAst* > m_varDecls;
	std::vector< ProcDeclAst* > m_procs;
	std::vector< DefDecl* > m_defs;
	std::vector< InterfaceDecl* > m_interfaces;

public:

	Package( const wchar_t *name ) : m_name(name) {}

	void AddDefDecl( DefDecl *v ) { m_defs.push_back(v); }
	void AddInterfaceDecl( InterfaceDecl *v ) { m_interfaces.push_back(v); }
	void AddVarDecl( VarDeclAst *v ) { m_varDecls.push_back(v); }
	void AddProcDecl( ProcDeclAst *p ) { m_procs.push_back(p); }

	void Generate();
};
