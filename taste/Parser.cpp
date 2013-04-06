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


#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"


namespace Taste {


void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::Type(int &type) {
		if (la->kind == 4 /* "int" */) {
			Get();
			type = integer; 
		} else if (la->kind == 5 /* "bool" */) {
			Get();
			type = boolean; 
		} else if (la->kind == 6 /* "float" */) {
			Get();
			type = floatingpoint; 
		} else if (la->kind == 7 /* "object" */) {
			Get();
			type = object; 
		} else if (la->kind == 8 /* "userptr" */) {
			Get();
			type = userptr; 
		} else SynErr(41);
}

void Parser::TypeNotObject(int &type) {
		if (la->kind == 4 /* "int" */) {
			Get();
			type = integer; 
		} else if (la->kind == 5 /* "bool" */) {
			Get();
			type = boolean; 
		} else if (la->kind == 6 /* "float" */) {
			Get();
			type = floatingpoint; 
		} else if (la->kind == 8 /* "userptr" */) {
			Get();
			type = userptr; 
		} else SynErr(42);
}

void Parser::ReturnType(int &type) {
		if (la->kind == 9 /* "void" */) {
			Get();
			type = voidtype; 
		} else if (StartOf(1)) {
			Type(type);
		} else SynErr(43);
}

void Parser::DataEntry(DataAst *parent) {
		DataAst *data = new DataAst(parent); if ( parent ) parent->AddKid(data); 
		if (la->kind == 7 /* "object" */) {
			Get();
			Expect(_ident);
			Expect(10 /* ":" */);
			DataDefinition(data);
		} else if (la->kind == 4 /* "int" */) {
			Get();
			data->SetType( integer ); 
			Expect(_ident);
			data->SetName( t->val ); 
			Expect(10 /* ":" */);
			Expect(_number);
			data->SetValue( t->val ); 
		} else if (la->kind == 6 /* "float" */) {
			Get();
			data->SetType( floatingpoint ); 
			Expect(_ident);
			data->SetName( t->val ); 
			Expect(10 /* ":" */);
			if (la->kind == _number) {
				Get();
			} else if (la->kind == _floatingpoint) {
				Get();
			} else SynErr(44);
			data->SetValue( t->val ); 
		} else SynErr(45);
}

void Parser::DataDefinition(DataAst *parent) {
		Expect(11 /* "{" */);
		if (la->kind == 4 /* "int" */ || la->kind == 6 /* "float" */ || la->kind == 7 /* "object" */) {
			DataEntry(parent);
			while (la->kind == 12 /* "," */) {
				Get();
				DataEntry(parent);
			}
		}
		Expect(13 /* "}" */);
}

void Parser::VarDecl(VarDeclAst* &varDecl) {
		int type; varDecl = new VarDeclAst; AstBase *expr; 
		Expect(14 /* "var" */);
		if (la->kind == 7 /* "object" */) {
			Get();
			varDecl->SetType(object); 
			if (la->kind == 15 /* "[" */) {
				Get();
				Expect(16 /* "]" */);
				varDecl->SetIsArray(); 
			}
			Expect(_ident);
			varDecl->SetName(t->val); 
			if (la->kind == 17 /* "=" */) {
				Get();
				if (la->kind == 18 /* "new" */) {
					Get();
					Expect(_ident);
					varDecl->SetPrototype(t->val); 
				} else if (StartOf(2)) {
					Expr(expr);
					varDecl->SetExpr( expr ); 
				} else if (la->kind == 11 /* "{" */) {
					DataAst *data = new DataAst(varDecl->GetNameWC(), NULL); 
					DataDefinition(data);
					varDecl->SetData( data ); 
				} else SynErr(46);
			}
			Expect(19 /* ";" */);
		} else if (StartOf(3)) {
			TypeNotObject(type);
			varDecl->SetType(type); 
			if (la->kind == 15 /* "[" */) {
				Get();
				Expect(16 /* "]" */);
				varDecl->SetIsArray(); 
			}
			Expect(_ident);
			varDecl->SetName(t->val); 
			if (la->kind == 17 /* "=" */) {
				Get();
				Expr(expr);
				varDecl->SetExpr( expr ); 
			}
			Expect(19 /* ";" */);
		} else SynErr(47);
}

void Parser::Expr(AstBase* &expr) {
		AstBase* other_expr; int op; 
		SimExpr(other_expr);
		expr = other_expr; 
		if (la->kind == 30 /* "==" */ || la->kind == 31 /* "<" */ || la->kind == 32 /* ">" */) {
			RelOp(op);
			SimExpr(other_expr);
			expr = new BinaryAst( expr, op, other_expr ); 
		}
}

void Parser::AddOp(int &op) {
		if (la->kind == 20 /* "+" */) {
			Get();
			op = plus; 
		} else if (la->kind == 21 /* "-" */) {
			Get();
			op = minus; 
		} else SynErr(48);
}

void Parser::MulOp(int &op) {
		if (la->kind == 22 /* "*" */) {
			Get();
			op = times; 
		} else if (la->kind == 23 /* "/" */) {
			Get();
			op = slash; 
		} else SynErr(49);
}

void Parser::SimExpr(AstBase* &expr) {
		int op; AstBase *term; 
		Term(term);
		expr = term; 
		while (la->kind == 20 /* "+" */ || la->kind == 21 /* "-" */) {
			AddOp(op);
			Term(term);
			expr = new BinaryAst( expr, op, term ); 
		}
}

void Parser::RelOp(int &op) {
		if (la->kind == 30 /* "==" */) {
			Get();
			op = equ; 
		} else if (la->kind == 31 /* "<" */) {
			Get();
			op = lss; 
		} else if (la->kind == 32 /* ">" */) {
			Get();
			op = gtr; 
		} else SynErr(50);
}

void Parser::ObjectMemberList(IdentVec &vec) {
		Expect(_ident);
		vec.push_back( t->val ); 
		while (la->kind == 24 /* "." */) {
			Get();
			ObjectMemberList(vec);
		}
}

void Parser::Factor(AstBase* &factor) {
		AstBase *other_factor; 
		switch (la->kind) {
		case _ident: {
			Get();
			IdentAst *identFactor = new IdentAst( t->val ); factor = identFactor; IdentVec identVec; AstVec exprVec; 
			if (la->kind == 15 /* "[" */ || la->kind == 24 /* "." */ || la->kind == 25 /* "(" */) {
				if (la->kind == 15 /* "[" */) {
					Get();
					Expect(_number);
					identFactor->SetArrayIndex( t->val ); 
					Expect(16 /* "]" */);
				} else if (la->kind == 24 /* "." */) {
					Get();
					ObjectMemberList(identVec);
					identFactor->SetMemberList( identVec ); 
					if (la->kind == 25 /* "(" */) {
						Get();
						if (StartOf(2)) {
							ExprList(exprVec);
						}
						Expect(26 /* ")" */);
						factor = new CallAst( identFactor->GetNameWC(), identVec, exprVec ); delete identFactor; 
					}
				} else {
					Get();
					if (StartOf(2)) {
						ExprList(exprVec);
					}
					Expect(26 /* ")" */);
					factor = new CallAst( identFactor->GetNameWC(), identVec, exprVec ); delete identFactor; 
				}
			}
			break;
		}
		case _number: {
			Get();
			factor = new ConstIntegerAst( t->val ); 
			break;
		}
		case _floatingpoint: {
			Get();
			factor = new ConstFloatingPointAst( t->val ); 
			break;
		}
		case 21 /* "-" */: {
			Get();
			Factor(other_factor);
			factor = new NegateAst( other_factor ); 
			break;
		}
		case 27 /* "true" */: {
			Get();
			factor = new ConstBooleanAst( true ); 
			break;
		}
		case 28 /* "false" */: {
			Get();
			factor = new ConstBooleanAst( false ); 
			break;
		}
		case 25 /* "(" */: {
			Get();
			Expr(factor);
			Expect(26 /* ")" */);
			break;
		}
		default: SynErr(51); break;
		}
}

void Parser::ExprList(AstVec &vec) {
		AstBase *expr; 
		Expr(expr);
		vec.push_back( expr ); 
		while (la->kind == 12 /* "," */) {
			Get();
			Expr(expr);
			vec.push_back( expr ); 
		}
}

void Parser::Declaration(DeclInfo &decl) {
		Type(decl.type);
		Expect(_ident);
		decl.name = t->val; 
}

void Parser::DeclarationList(DeclVec &vec) {
		DeclInfo decl; 
		Declaration(decl);
		vec.push_back( decl ); 
		while (la->kind == 12 /* "," */) {
			Get();
			Declaration(decl);
			vec.push_back( decl ); 
		}
}

void Parser::ProcDecl(ProcDeclAst* &procDecl) {
		VarDeclAst *varDecl; int type; AstBase *stat; DeclVec declVec; 
		Expect(29 /* "function" */);
		procDecl = new ProcDeclAst(); 
		ReturnType(type);
		procDecl->SetReturnType( type ); 
		Expect(_ident);
		procDecl->SetName( t->val ); 
		Expect(25 /* "(" */);
		if (StartOf(1)) {
			DeclarationList(declVec);
		}
		Expect(26 /* ")" */);
		procDecl->SetDeclaration(declVec); 
		Expect(11 /* "{" */);
		while (StartOf(4)) {
			if (la->kind == 14 /* "var" */) {
				VarDecl(varDecl);
				procDecl->AddBody( varDecl ); 
			} else {
				Stat(stat);
				procDecl->AddBody( stat ); 
			}
		}
		Expect(13 /* "}" */);
}

void Parser::Stat(AstBase *&stat) {
		VarDeclAst *varDecl; AstBase *expr = NULL; AstBase *substat; 
		if (la->kind == 33 /* "if" */) {
			Get();
			Expect(25 /* "(" */);
			Expr(expr);
			Expect(26 /* ")" */);
			IfAst *ifstat = new IfAst( expr ); stat = ifstat; 
			Expect(11 /* "{" */);
			while (StartOf(4)) {
				if (StartOf(5)) {
					Stat(substat);
					ifstat->AddIf( substat ); 
				} else {
					VarDecl(varDecl);
					ifstat->AddIf( varDecl ); 
				}
			}
			Expect(13 /* "}" */);
			if (la->kind == 34 /* "else" */) {
				Get();
				Expect(11 /* "{" */);
				while (StartOf(4)) {
					if (StartOf(5)) {
						Stat(substat);
						ifstat->AddElse( substat ); 
					} else {
						VarDecl(varDecl);
						ifstat->AddElse( varDecl ); 
					}
				}
				Expect(13 /* "}" */);
			}
		} else if (la->kind == 35 /* "return" */) {
			Get();
			if (StartOf(2)) {
				Expr(expr);
			}
			Expect(19 /* ";" */);
			stat = new ReturnStat( expr ); 
		} else if (la->kind == 36 /* "while" */) {
			Get();
			Expect(25 /* "(" */);
			Expr(expr);
			Expect(26 /* ")" */);
			WhileAst *whilestat = new WhileAst( expr ); stat = whilestat; 
			Expect(11 /* "{" */);
			while (StartOf(4)) {
				if (StartOf(5)) {
					Stat(substat);
					whilestat->AddExpr( substat ); 
				} else {
					VarDecl(varDecl);
					whilestat->AddExpr( varDecl ); 
				}
			}
			Expect(13 /* "}" */);
		} else if (la->kind == 11 /* "{" */) {
			Get();
			BlockAst *blockstat = new BlockAst; stat = blockstat; 
			while (StartOf(4)) {
				if (StartOf(5)) {
					Stat(substat);
					blockstat->AddExpr( substat ); 
				} else {
					VarDecl(varDecl);
					blockstat->AddExpr( varDecl ); 
				}
			}
			Expect(13 /* "}" */);
		} else if (la->kind == _ident) {
			Get();
			IdentAst *identStat = new IdentAst( t->val ); stat = identStat; IdentVec identVec; 
			if (la->kind == 15 /* "[" */ || la->kind == 24 /* "." */) {
				if (la->kind == 15 /* "[" */) {
					Get();
					Expect(_number);
					identStat->SetArrayIndex( t->val ); 
					Expect(16 /* "]" */);
				} else {
					Get();
					ObjectMemberList(identVec);
					identStat->SetMemberList( identVec ); 
				}
			}
			if (la->kind == 17 /* "=" */ || la->kind == 25 /* "(" */) {
				if (la->kind == 17 /* "=" */) {
					Get();
					Expr(expr);
					stat = new AssignAst( identStat->GetNameWC(), identStat->GetArrayIndex(), identVec, expr ); delete identStat; 
				} else {
					Get();
					AstVec exprVec; 
					if (StartOf(2)) {
						ExprList(exprVec);
					}
					Expect(26 /* ")" */);
					stat = new CallAst( identStat->GetNameWC(), identVec, exprVec ); delete identStat; 
				}
			}
			Expect(19 /* ";" */);
		} else SynErr(52);
}

void Parser::Term(AstBase* &term) {
		int op; AstBase *factor; 
		Factor(factor);
		term = factor; 
		while (la->kind == 22 /* "*" */ || la->kind == 23 /* "/" */) {
			MulOp(op);
			Factor(factor);
			term = new BinaryAst( term, op, factor ); 
		}
}

void Parser::Def(DefDecl *&def) {
		VarDeclAst *varDecl; ProcDeclAst *procDecl; 
		Expect(37 /* "def" */);
		Expect(_ident);
		def = new DefDecl( t->val ); 
		if (la->kind == 38 /* "extends" */) {
			Get();
			Expect(_ident);
			def->SetExtends( t->val ); 
		}
		Expect(11 /* "{" */);
		while (la->kind == 14 /* "var" */ || la->kind == 29 /* "function" */) {
			if (la->kind == 14 /* "var" */) {
				VarDecl(varDecl);
				def->AddVarDecl( varDecl ); 
			} else {
				ProcDecl(procDecl);
				def->AddProcDecl( procDecl ); 
			}
		}
		Expect(13 /* "}" */);
		Expect(19 /* ";" */);
}

void Parser::Taste() {
		DefDecl *defDecl; VarDeclAst *varDecl; ProcDeclAst *procDecl; 
		Expect(39 /* "package" */);
		Expect(_ident);
		package = new Package( t->val ); 
		Expect(11 /* "{" */);
		while (la->kind == 14 /* "var" */ || la->kind == 29 /* "function" */ || la->kind == 37 /* "def" */) {
			if (la->kind == 37 /* "def" */) {
				Def(defDecl);
				package->AddDefDecl( defDecl ); 
			} else if (la->kind == 14 /* "var" */) {
				VarDecl(varDecl);
				package->AddVarDecl( varDecl ); 
			} else {
				ProcDecl(procDecl);
				package->AddProcDecl( procDecl ); 
			}
		}
		Expect(13 /* "}" */);
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	Taste();
	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 40;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[6][42] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, T,T,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,T,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,x,x,x, T,T,T,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,T, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, T,x,x,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, T,x,x,x, x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"ident expected"); break;
			case 2: s = coco_string_create(L"number expected"); break;
			case 3: s = coco_string_create(L"floatingpoint expected"); break;
			case 4: s = coco_string_create(L"\"int\" expected"); break;
			case 5: s = coco_string_create(L"\"bool\" expected"); break;
			case 6: s = coco_string_create(L"\"float\" expected"); break;
			case 7: s = coco_string_create(L"\"object\" expected"); break;
			case 8: s = coco_string_create(L"\"userptr\" expected"); break;
			case 9: s = coco_string_create(L"\"void\" expected"); break;
			case 10: s = coco_string_create(L"\":\" expected"); break;
			case 11: s = coco_string_create(L"\"{\" expected"); break;
			case 12: s = coco_string_create(L"\",\" expected"); break;
			case 13: s = coco_string_create(L"\"}\" expected"); break;
			case 14: s = coco_string_create(L"\"var\" expected"); break;
			case 15: s = coco_string_create(L"\"[\" expected"); break;
			case 16: s = coco_string_create(L"\"]\" expected"); break;
			case 17: s = coco_string_create(L"\"=\" expected"); break;
			case 18: s = coco_string_create(L"\"new\" expected"); break;
			case 19: s = coco_string_create(L"\";\" expected"); break;
			case 20: s = coco_string_create(L"\"+\" expected"); break;
			case 21: s = coco_string_create(L"\"-\" expected"); break;
			case 22: s = coco_string_create(L"\"*\" expected"); break;
			case 23: s = coco_string_create(L"\"/\" expected"); break;
			case 24: s = coco_string_create(L"\".\" expected"); break;
			case 25: s = coco_string_create(L"\"(\" expected"); break;
			case 26: s = coco_string_create(L"\")\" expected"); break;
			case 27: s = coco_string_create(L"\"true\" expected"); break;
			case 28: s = coco_string_create(L"\"false\" expected"); break;
			case 29: s = coco_string_create(L"\"function\" expected"); break;
			case 30: s = coco_string_create(L"\"==\" expected"); break;
			case 31: s = coco_string_create(L"\"<\" expected"); break;
			case 32: s = coco_string_create(L"\">\" expected"); break;
			case 33: s = coco_string_create(L"\"if\" expected"); break;
			case 34: s = coco_string_create(L"\"else\" expected"); break;
			case 35: s = coco_string_create(L"\"return\" expected"); break;
			case 36: s = coco_string_create(L"\"while\" expected"); break;
			case 37: s = coco_string_create(L"\"def\" expected"); break;
			case 38: s = coco_string_create(L"\"extends\" expected"); break;
			case 39: s = coco_string_create(L"\"package\" expected"); break;
			case 40: s = coco_string_create(L"??? expected"); break;
			case 41: s = coco_string_create(L"invalid Type"); break;
			case 42: s = coco_string_create(L"invalid TypeNotObject"); break;
			case 43: s = coco_string_create(L"invalid ReturnType"); break;
			case 44: s = coco_string_create(L"invalid DataEntry"); break;
			case 45: s = coco_string_create(L"invalid DataEntry"); break;
			case 46: s = coco_string_create(L"invalid VarDecl"); break;
			case 47: s = coco_string_create(L"invalid VarDecl"); break;
			case 48: s = coco_string_create(L"invalid AddOp"); break;
			case 49: s = coco_string_create(L"invalid MulOp"); break;
			case 50: s = coco_string_create(L"invalid RelOp"); break;
			case 51: s = coco_string_create(L"invalid Factor"); break;
			case 52: s = coco_string_create(L"invalid Stat"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}

} // namespace

