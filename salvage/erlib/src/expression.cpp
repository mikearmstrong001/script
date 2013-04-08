
#include "erbase.h"
#include "expression.h"
// E = E + T
//  |  E - T
//  |  T

// T = T * F
//	|  T / F
//	|  F

// F = num
//  | id


// E = T E'
// E' = + E
//  | = - E
//  | =

// T 
#if 0
procedure node * expr()
	term(); term_tail();

procedure term_tail()
	case (input_token())
		of ’+’: match(’+’); expr();
		of ’-’: match(’-’); expr();
		otherwise: /* skip */

procedure term()
	factor(); factor_tail();

procedure factor_tail()
	case (input_token())
		of ’*’: match(’*’); term();
		of ’/’:  match(’/’); term();
		otherwise: /* skip */

procedure factor()
	case (input_token())
		of ’(’: match(’(’); expr(); match(’)’);
		of ’-’: match(’-’); factor();
		of identifier: match(identifier);
		of number: match(number);
		otherwise: error;
		
#endif


int curpos = 0;
const char *tokens[] = 
{ "1", "+", "3", "*", "5", "+", "9", "*", "0", NULL };

extern node * ParseTerm();
extern node * ParseExpr();

node * ParseFactor() {
	const char *t = tokens[ curpos++ ];
	return new value_node( (float)atof( t ) );
}

node * ParseFactorTail( int &cmd ) {
	const char *t = tokens[ curpos ];
	if ( t == NULL ) {
		return NULL;
	}

	if ( strcmp( t, "*" ) == 0 ) {
		cmd = '*';
		curpos++;
		return ParseTerm();
	} else
	if ( strcmp( t, "/" ) == 0 ) {
		cmd = '/';
		curpos++;
		return ParseTerm();
	} else {
		return NULL;
	}
}

node * ParseTerm() {
	node *f = ParseFactor();
	int cmd;
	node *n = ParseFactorTail( cmd );

	if ( n ) {
		return new expr_node( f, cmd, n );
	} else {
		return f;
	}
}

node * ParseTermTail( int &cmd ) {
	const char *t = tokens[ curpos ];
	if ( t == NULL ) {
		return NULL;
	}

	if ( strcmp( t, "+" ) == 0 ) {
		cmd = '+';
		curpos++;
		return ParseExpr();
	} else
	if ( strcmp( t, "-" ) == 0 ) {
		cmd = '-';
		curpos++;
		return ParseExpr();
	} else {
		return NULL;
	}
}

node * ParseExpr() {
	node *t = ParseTerm();
	int cmd;
	node *n = ParseTermTail( cmd );

	if ( n ) {
		return new expr_node( t, cmd, n );
	} else {
		return t;
	}
};

node *OptimizeConstants( node *n ) {
	node *n2 = n->CollapseConstants();
	if ( n2 != n ) {
		delete n;
		n = n2;
	}
	return n;
}

