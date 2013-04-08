#ifndef MLIB_EXPRESSION_H
#define MLIB_EXPRESSION_H

namespace mlib
{

class node {
public:
	virtual ~node() {
	}

	virtual bool IsConstant() const {
		return false;
	}

	virtual node * CollapseConstants() {
		return this;
	}

	virtual float GetValue() const = 0;
};

class value_node : public node {
	float v;
public:

	value_node( float _v ) : v(_v) {
	}

	virtual bool IsConstant() const {
		return true;
	}

	virtual float GetValue() const {
		return v;
	}
};


class expr_node : public node {
	node *l;
	int cmd;
	node *r;
public:
	expr_node( node *_l, int _cmd, node *_r) : l(_l), cmd(_cmd), r(_r) {
	}

	~expr_node() {
		delete l;
		delete r;
	}

	virtual bool IsConstant() const {
		return false;
	}

	virtual node * CollapseConstants() {
		node *ol = l;
		node *or = r;
		
		l = l->CollapseConstants();
		r = r->CollapseConstants();

		if ( l != ol ) {
			delete ol;
		}
		if ( r != or ) {
			delete or;
		}

		if ( l->IsConstant() && r->IsConstant() ) {
			node *n = new value_node( GetValue() );
			delete l; 
			l = NULL;
			delete r; 
			r = NULL;
			return n;
		} else
		if ( cmd == '+' && l->IsConstant() && l->GetValue() == 0.f ) {
			delete l; 
			l = NULL;
			return r;
		} else
		if ( cmd == '+' && r->IsConstant() && r->GetValue() == 0.f ) {
			delete r;
			return l;
		} else
		if ( cmd == '-' && l->IsConstant() && l->GetValue() == 0.f ) {
			node *n = new value_node( -r->GetValue() );
			delete l; 
			l = NULL;
			delete r; 
			r = NULL;
			return n;
		} else
		if ( cmd == '-' && r->IsConstant() && r->GetValue() == 0.f ) {
			delete r; 
			r = NULL;
			return l;
		} else
		if ( cmd == '*' && l->IsConstant() && l->GetValue() == 0.f ) {
			node *n = new value_node( 0.f );
			delete l; 
			l = NULL;
			delete r; 
			r = NULL;
			return n;
		} else
		if ( cmd == '*' && r->IsConstant() && r->GetValue() == 0.f ) {
			delete l; 
			l = NULL;
			delete r; 
			r = NULL;
			return new value_node( 0.f );
		} else
		if ( cmd == '/' && l->IsConstant() && l->GetValue() == 0.f ) {
			delete l; 
			l = NULL;
			delete r; 
			r = NULL;
			return new value_node( 0.f );
		} else
		if ( cmd == '*' && l->IsConstant() && l->GetValue() == 1.f ) {
			delete l;
			l = NULL;
			return r;
		} else
		if ( cmd == '*' && r->IsConstant() && r->GetValue() == 1.f ) {
			delete r; 
			r = NULL;
			return l;
		} else
		if ( cmd == '/' && r->IsConstant() && r->GetValue() == 1.f ) {
			delete r; 
			r = NULL;
			return l;
		} else {
			return this;
		}
	}

	virtual float GetValue() const {
		if ( cmd == '*' ) {
			return l->GetValue() * r->GetValue();
		} else
		if ( cmd == '/' ) {
			return l->GetValue() / r->GetValue();
		} else
		if ( cmd == '+' ) {
			return l->GetValue() + r->GetValue();
		} else
		if ( cmd == '-' ) {
			return l->GetValue() - r->GetValue();
		}

		return 0.f;
	}

};

class table_node : public node {
public:

	virtual bool IsConstant() const {
		return false;
	}
};

class runtimeconst_node : public node {
public:
	virtual bool IsConstant() const {
		return false;
	}
};

extern node *ParseExpr();
extern node *OptimizeConstants( node *n );

};

#endif