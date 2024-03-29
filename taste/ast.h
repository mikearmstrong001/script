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
	int usertype;
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

	int AddEntry( const std::wstring &n, VARTYPE type, const std::wstring &usertype )
	{
		StackEntry e;
		e.name = n;
		e.type = type;
		e.usertype = Hash(usertype.c_str());
		e.depth = m_return.size();
		int rel = m_active.size() - m_return[m_return.size()-1];
		m_active.push_back( e );
		return rel;
	}

	int AddEntry( const std::wstring &n, VARTYPE type, int usertype = 0 )
	{
		StackEntry e;
		e.name = n;
		e.type = type;
		e.usertype = usertype;
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
				int rel = i - m_return[m_return.size()-1];
				return rel;
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
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg ) = 0;
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

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class ValueAst : public AstBase
{
};

class IdentAst : public ValueAst
{
	std::wstring m_name;
	IdentVec m_identVec;
	AstBase		*m_index;
public:
	IdentAst( const wchar_t *token ) : m_name(token), m_index(0) {}
	void SetArrayIndex( AstBase *index ) { m_index = index; }
	void SetMemberList( const IdentVec &v ) { m_identVec = v; }
	std::wstring const &GetName() const { return m_name; }
	wchar_t const *GetNameWC() const { return m_name.c_str(); }
	AstBase *GetArrayIndex() const { return m_index; }
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class ConstIntegerAst : public ValueAst
{
	int m_value;
public:
	ConstIntegerAst( const wchar_t *token )
	{
		swscanf(token, L"%d", &m_value);
	}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class ConstFloatingPointAst : public ValueAst
{
	float m_value;
public:
	ConstFloatingPointAst( const wchar_t *token )
	{
		swscanf(token, L"%f", &m_value);
	}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class ConstStringAst : public ValueAst
{
	std::wstring m_value;
public:
	ConstStringAst( const wchar_t *token )
	{
		m_value = std::wstring(token+1);
		m_value = m_value.substr( 0, m_value.size()-1 );
	}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class ConstBooleanAst : public ValueAst
{
	bool m_value;
public:
	ConstBooleanAst( bool v ) : m_value(v) {}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class NegateAst : public AstBase
{
	AstBase *m_child;
public:
	NegateAst( AstBase *child ) : m_child(child) {}
	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
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
public:
	VarDeclAst() : m_type(0), m_isarray(false) {}

	void SetType( int type ) { m_type = type; }
	void SetPrototype( const wchar_t *name ) { m_prototype = name; }
	void SetName( const wchar_t *name ) { m_name = name; }
	void SetIsArray() { m_isarray = true; }

	wchar_t const *GetNameWC() const { return m_name.c_str(); }
	std::wstring const &GetName() const { return m_name; }
	int	GetType() const { return m_type; }
	bool IsUserType() const { return (unsigned int)m_type >= MAX_VARTYPE; }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
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

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class AssignAst : public AstBase
{
	std::wstring m_name;
	AstBase  *m_arrayIndex;
	IdentVec m_identVec;
	AstBase *m_expr;
public:

	AssignAst( const wchar_t *token, AstBase  *arrayindex, const IdentVec &v, AstBase *expr ) : m_name(token), m_arrayIndex(arrayindex), m_identVec(v), m_expr(expr) {}

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class CallAst : public AstBase
{
	AstVec m_callExpr;
	std::wstring m_name;
	IdentVec m_identVec;
public:

	CallAst( const wchar_t *token, const IdentVec &v, AstVec &expr ) : m_name(token) , m_identVec(v), m_callExpr(expr) {}

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
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

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class WhileAst : public AstBase
{
	AstBase *m_conditional;
	std::vector<AstBase *> m_expr;

public:

	WhileAst( AstBase *conditional ) : m_conditional(conditional) {}

	void AddExpr( AstBase *i ) { m_expr.push_back( i ); }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class ReturnStat : public AstBase
{
	AstBase *m_expr;
public:
	ReturnStat( AstBase *e ) : m_expr(e) {}

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class BlockAst : public AstBase
{
	std::vector<AstBase *> m_expr;

public:

	void AddExpr( AstBase *i ) { m_expr.push_back( i ); }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class EmbedDeclAst : public AstBase
{
	std::wstring m_type;
	std::wstring m_name;
public:
	EmbedDeclAst( const wchar_t *type ) : m_type(type) {}

	void SetName( const wchar_t *name ) { m_name = name; }

	std::wstring const &GetType() const { return m_type; }
	std::wstring const &GetName() const { return m_name; }

	virtual void Generate( std::vector<int> &oplist, StackFrame &frame, class Package *pkg );
};

class DefDecl
{
	std::wstring m_name;
	std::wstring m_extends;
	std::vector< VarDeclAst* > m_varDecls;
	std::vector< ProcDeclAst* > m_procs;
	std::vector< EmbedDeclAst* > m_embeds;

	struct Element
	{
		//std::wstring name;
		int hashName;
		VarDeclAst* varInfo;
	};

	std::vector< Element > m_flattenedProps;


public:

	DefDecl( const wchar_t *name ) : m_name(name) {}

	void SetExtends( const wchar_t *ex ) { m_extends = ex; }
	void AddVarDecl( VarDeclAst *v ) { m_varDecls.push_back(v); }
	void AddProcDecl( ProcDeclAst *p ) { m_procs.push_back(p); }
	void AddEmbedDecl( EmbedDeclAst *p ) { m_embeds.push_back(p); }

	std::wstring const &GetName() const { return m_name; }
	std::wstring const &GetExtends() const { return m_extends; }

	std::vector< Element > const &GetProps() const { return m_flattenedProps; }

	int FindElementIndex( int hashName );

	virtual void GenerateDef( std::vector<int> &oplist, vmstate &state, class Package *pkg );
	virtual void GenerateProps( class Package *pkg, vmstate &state );
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

	std::vector< ProcDefDeclAst* > const &GetProcs() { return m_procs; }
};

class Package
{
	std::wstring m_name;
	std::vector< VarDeclAst* > m_varDecls;
	std::vector< ProcDeclAst* > m_procs;
	std::vector< DefDecl* > m_defs;
	std::vector< DefDecl* > m_structs;
	std::vector< InterfaceDecl* > m_interfaces;

public:

	Package( const wchar_t *name ) : m_name(name) {}

	DefDecl *FindStruct( const wchar_t *name );
	DefDecl *FindStruct( int name );
	VarDeclAst *FindVar( const wchar_t *name );
	VarDeclAst *FindVar( int name );
	ProcDeclAst *FindProc( const wchar_t *name );
	ProcDeclAst *FindProc( int name );

	void AddDefDecl( DefDecl *v ) { m_defs.push_back(v); }
	void AddStructDecl( DefDecl *v ) { m_structs.push_back(v); }
	void AddInterfaceDecl( InterfaceDecl *v ) { m_interfaces.push_back(v); }
	void AddVarDecl( VarDeclAst *v ) { m_varDecls.push_back(v); }
	void AddProcDecl( ProcDeclAst *p ) { m_procs.push_back(p); }


	void Generate();
};
