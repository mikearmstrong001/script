#ifndef ER_HEADER_CALLBACK_H
#define ER_HEADER_CALLBACK_H


class callback {
public:
	virtual ~callback() {}

	virtual void func() = 0;
};




template <class classType, typename funcType >
class callbackClass0 : public callback {

	classType c;
	funcType f;

public:

	callbackClass0( classType _c, funcType _f ) : c(_c), f(_f) {
	}

	virtual void func() {
		(c->*f)();
	}
};

template <class classType, typename funcType, typename arg0 >
class callbackClass1 : public callback {

	classType c;
	funcType f;
	arg0     a0;

public:

	callbackClass1( classType _c, funcType _f, arg0 _a0 ) : c(_c), f(_f), a0(_a0) {
	}

	virtual void func() {
		(c->*f)( a0 );
	}
};

template <class classType, typename funcType, typename arg0, typename arg1 >
class callbackClass2 : public callback {

	classType c;
	funcType f;
	arg0     a0;
	arg1     a1;

public:

	callbackClass2( classType _c, funcType _f, arg0 _a0, arg1 _a1 ) : c(_c), f(_f), a0(_a0), a1(_a1) {
	}

	virtual void func() {
		(c->*f)( a0, a1 );
	}
};

template <class classType, typename funcType, typename arg0, typename arg1,  typename arg2 >
class callbackClass3 : public callback {

	classType c;
	funcType f;
	arg0     a0;
	arg1     a1;
	arg2     a2;

public:

	callbackClass3( classType _c, funcType _f, arg0 _a0, arg1 _a1, arg2 _a2 ) : c(_c), f(_f), a0(_a0), a1(_a1), a2(_a2) {
	}

	virtual void func() {
		(c->*f)( a0, a1, a2 );
	}
};







template <typename funcType >
class callbackFunc0 : public callback {

	funcType f;

public:

	callbackFunc0( funcType _f ) : f(_f) {
	}

	virtual void func() {
		(*f)();
	}
};

template <typename funcType, typename arg0 >
class callbackFunc1 : public callback {

	funcType f;
	arg0     a0;

public:

	callbackFunc1( funcType _f, arg0 _a0 ) : f(_f), a0(_a0) {
	}

	virtual void func() {
		(*f)( a0 );
	}
};

template <typename funcType, typename arg0, typename arg1 >
class callbackFunc2 : public callback {

	funcType f;
	arg0     a0;
	arg1     a1;

public:

	callbackFunc2( funcType _f, arg0 _a0, arg1 _a1 ) : f(_f), a0(_a0), a1(_a1) {
	}

	virtual void func() {
		(*f)( a0, a1 );
	}
};

template <typename funcType, typename arg0, typename arg1,  typename arg2 >
class callbackFunc3 : public callback {

	funcType f;
	arg0     a0;
	arg1     a1;
	arg2     a2;

public:

	callbackFunc3( funcType _f, arg0 _a0, arg1 _a1, arg2 _a2 ) : f(_f), a0(_a0), a1(_a1), a2(_a2) {
	}

	virtual void func() {
		(*f)( a0, a1, a2 );
	}
};


template <class classType, typename funcType >
callback *make_cbc( classType c, funcType f ) {
	return new callbackClass0<classType, funcType>( c, f );
}
template <class classType, typename funcType, typename arg0 >
callback *make_cbc( classType c, funcType f, arg0 a0 ) {
	return new callbackClass1<classType, funcType, arg0>( c, f, a0 );
}
template <class classType, typename funcType, typename arg0, typename arg1 >
callback *make_cbc( classType c, funcType f, arg0 a0, arg1 a1 ) {
	return new callbackClass2<classType, funcType, arg0, arg1>( c, f, a0, a1 );
}
template <class classType, typename funcType, typename arg0, typename arg1,  typename arg2 >
callback *make_cbc( classType c, funcType f, arg0 a0, arg1 a1, arg2 a2 ) {
	return new callbackClass3<classType, funcType, arg0, arg1, arg2>( c, f, a0, a1, a2 );
}



template <typename funcType >
callback *make_cbf( funcType f ) {
	return new callbackFunc0<funcType>( f );
}
template <typename funcType, typename arg0 >
callback *make_cbf( funcType f, arg0 a0 ) {
	return new callbackFunc1<funcType, arg0>( f, a0 );
}
template <typename funcType, typename arg0, typename arg1 >
callback *make_cbf( funcType f, arg0 a0, arg1 a1 ) {
	return new callbackFunc2<funcType, arg0, arg1>( f, a0, a1 );
}
template <typename funcType, typename arg0, typename arg1, typename arg2 >
callback *make_cbf( funcType f, arg0 a0, arg1 a1, arg2 a2 ) {
	return new callbackFunc3<funcType, arg0, arg1, arg2 >( f, a0, a1, a2 );
}

#endif

