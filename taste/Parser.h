/*----------------------------------------------------------------------
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
with improvements by Pat Terry, Rhodes University

This program is free software; you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the 
Free Software Foundation; either version 2, or (at your option) any 
later version.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

As an exception, it is allowed to write an extension of Coco/R that is
used as a plugin in non-free software.

If not otherwise stated, any source code generated by Coco/R (other than 
Coco/R itself) does not fall under the GNU General Public License.
-----------------------------------------------------------------------*/


#if !defined(Taste_COCO_PARSER_H__)
#define Taste_COCO_PARSER_H__

#include "ast.h"
#include "wchar.h"


#include "Scanner.h"

namespace Taste {


class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_ident=1,
		_number=2,
		_floatingpoint=3,
		_characters=4
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token

int // operators
	  plus, minus, times, slash, equ, lss, gtr;

	int // types
	  undef, integer, boolean, floatingpoint, object, voidtype, userptr, stringtype;

	int // object kinds
	  var, proc;
	
	Package		  *package;

	void Err(wchar_t* msg) {
		errors->Error(la->line, la->col, msg);
	}

	void InitDeclarations() { // it must exist
		plus = 0; minus = 1; times = 2; slash = 3; equ = 4; lss = 5; gtr = 6; // operators
		undef = 0; boolean = 1; integer = 2; floatingpoint = 3; object = 4; voidtype = 5; userptr = 6; stringtype=7; // types
		var = 0; proc = 1; // object kinds
		package = NULL;
	}


  
/*--------------------------------------------------------------------------*/


	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void Type(int &type);
	void TypeNotObject(int &type);
	void ReturnType(int &type);
	void VarDecl(VarDeclAst* &varDecl);
	void AddOp(int &op);
	void MulOp(int &op);
	void Expr(AstBase* &expr);
	void SimExpr(AstBase* &expr);
	void RelOp(int &op);
	void ObjectMemberList(IdentVec &vec);
	void Factor(AstBase* &factor);
	void ExprList(AstVec &vec);
	void Declaration(DeclInfo &decl);
	void DeclarationList(DeclVec &vec);
	void ProcDecl(ProcDeclAst* &procDecl);
	void Stat(AstBase *&stat);
	void Term(AstBase* &term);
	void EmbedDecl(EmbedDeclAst *&embed);
	void Def(DefDecl *&def);
	void Struct(DefDecl *&def);
	void ProcDefDecl(ProcDefDeclAst* &procDecl);
	void Interface(InterfaceDecl *&def);
	void Taste();

	void Parse();

}; // end Parser

} // namespace


#endif

