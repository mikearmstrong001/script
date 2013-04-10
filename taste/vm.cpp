#include "vm.h"
#include <memory.h>
#include <stdio.h>

const char *opnames[] =
{
	"OPC_ADD",
	"OPC_SUB",
	"OPC_MUL",
	"OPC_DIV",
	"OPC_EQU",
	"OPC_LSS",
	"OPC_GTR",
	"OPC_PUSHI",
	"OPC_PUSHF",
	"OPC_NEG",
	"OPC_MAKEVAR",
	"OPC_MAKEVARG",
	"OPC_CONSTRUCTVAR",
	"OPC_CONSTRUCTVARFROMGLOBAL",
	"OPC_PUSHTOP",
	"OPC_PUSHENV",
	"OPC_POPENV",
	"OPC_RET",
	"OPC_CALL",
	"OPC_CALLG",
	"OPC_JF",
	"OPC_JMP",
	"OPC_POP",
	"OPC_POPR",
	"OPC_PUSHR",
	"OPC_PUSHITEM",
	"OPC_PUSHITEMG",
	"OPC_PUSHITEMARRAY",
	"OPC_PUSHITEMGARRAY",
	"OPC_POPITEM",
	"OPC_POPITEMG",
	"OPC_POPITEMARRAY",
	"OPC_POPITEMGARRAY",
	"OPC_PUSHSUPER",
};

static bool HasVar( object const *top, int key, vmstate &state )
{
	// first check object;
	const object *o = top;
	while ( o )
	{
		Map<var>::ConstIterator f = o->m_tbl.cfind( key );
		if ( f )
		{
			return true;
		}
		// not found check prototype
		if ( o->prototype != -1 )
		{
			Map<var>::ConstIterator p = state.globals.cfind(o->prototype);
			if ( p )
			{
				const var &po = *p.second;
				if ( po.type == OBJECT )
				{
					o = po.o;
				}
				else
				{
					o = 0;
				}
			}
		}
		else
		{
			o = 0;
		}
	}
	return false;
}

static bool pushbackObject( vmstate &state )
{
	const var &arg0 = state.GetArg( -2 );
	const var &arg1 = state.GetArg( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == OBJECTARRAY && arg1.type == OBJECT, "Expecting Object in builtin array pushback" );

	arg0.oArrayPtr->m_items.push_back( arg1.o );

	return false;
}

static bool pushbackFloatingPoint( vmstate &state )
{
	const var &arg0 = state.GetArg( -2 );
	const var &arg1 = state.GetArg( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == FLOATINGPOINTARRAY && arg1.type == FLOATINGPOINT, "Expecting FloatingPoint in builtin array pushback" );

	arg0.fArrayPtr->m_items.push_back( arg1.f );

	return false;
}

static bool pushbackInteger( vmstate &state )
{
	const var &arg0 = state.GetArg( -2 );
	const var &arg1 = state.GetArg( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == INTEGERARRAY && arg1.type == INTEGER, "Expecting Integer in builtin array pushback" );

	arg0.iArrayPtr->m_items.push_back( arg1.i );

	return false;
}

static bool pushbackUserPtr( vmstate &state )
{
	const var &arg0 = state.GetArg( -2 );
	const var &arg1 = state.GetArg( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == USERPTRARRAY && arg1.type == USERPTR, "Expecting userptr in builtin array pushback" );

	arg0.uArrayPtr->m_items.push_back( arg1.u );

	return false;
}

static bool sizeObject( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 1 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == OBJECTARRAY, "Expecting Object in builtin array size" );

	state.SetReturn( arg0.oArrayPtr->m_items.size() );

	return true;
}

static bool sizeFloatingPoint( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 1 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == FLOATINGPOINTARRAY, "Expecting FloatingPoint in builtin array size" );

	state.SetReturn( arg0.fArrayPtr->m_items.size() );

	return true;
}

static bool sizeInteger( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 1 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == INTEGERARRAY, "Expecting Integer in builtin array size" );

	state.SetReturn( arg0.iArrayPtr->m_items.size() );

	return true;
}

static bool sizeUserPtr( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 1 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == USERPTRARRAY, "Expecting Integer in builtin array size" );

	state.SetReturn( arg0.uArrayPtr->m_items.size() );

	return true;
}

static bool resizeObject( vmstate &state )
{
	const var &arg0 = state.GetArg( -2 );
	int arg1 = state.GetArgAsInt( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == OBJECTARRAY, "Expecting Object in builtin array size" );

	arg0.oArrayPtr->m_items.resize( arg1 );

	return false;
}

static bool resizeFloatingPoint( vmstate &state )
{
	const var &arg0 = state.GetArg( -2 );
	int arg1 = state.GetArgAsInt( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == FLOATINGPOINTARRAY, "Expecting Integer in builtin array size" );
	
	arg0.fArrayPtr->m_items.resize( arg1 );

	return false;
}

static bool resizeInteger( vmstate &state )
{
	const var &arg0 = state.GetArg( -2 );
	int arg1 = state.GetArgAsInt( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == INTEGERARRAY, "Expecting Integer in builtin array size" );

	arg0.iArrayPtr->m_items.resize( arg1 );

	return false;
}

static bool resizeUserPtr( vmstate &state )
{
	const var &arg0 = state.GetArg( -2 );
	int arg1 = state.GetArgAsInt( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == USERPTRARRAY, "Expecting Integer in builtin array size" );

	arg0.uArrayPtr->m_items.resize( arg1 );

	return false;
}

static bool testimplements( vmstate &state )
{
	const var &arg0 = state.GetArg( -2 );
	const var &arg1 = state.GetArg( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == OBJECT, "Arg0 expected to be of type OBJECT" );
	CEXCEPTION_ERROR_CONDITION( arg1.type == INTEGER, "Arg1 expected to be of type INTEGER" );

	int result = 1;

	Map<vminterface>::ConstIterator cit = state.ifaces.cfind( arg1.i );
	CEXCEPTION_ERROR_CONDITION( cit, "Unknown interface" );
	for (int i=0; i<cit.second->interfaceFunctions.size(); i++)
	{
		int key = cit.second->interfaceFunctions[i];
		if ( !HasVar( arg0.o, key, state ) )
		{
			result = 0;
			break;
		}
	}

	state.SetReturn( result );
	return true;
}

bool testcfunc( vmstate &state )
{
	int i0 = state.GetArgAsInt( -1 );
	state.rv.type = INTEGER;
	state.rv.i = i0 * 2;
	state.stack.pop(1);
	return true;
}

bool printffloat( vmstate &state )
{
	float f = state.GetArgAsFloat( -1 );
	printf( "%f\n", f );
	state.stack.pop(1);
	return false;
}

static const var &FindVar( object const *top, int key, vmstate &state )
{
	// first check object;
	const object *o = top;
	while ( o )
	{
		Map<var>::ConstIterator f = o->m_tbl.cfind( key );
		if ( f )
		{
			return *f.second;
		}
		// not found check prototype
		if ( o->prototype != -1 )
		{
			Map<var>::ConstIterator p = state.globals.cfind(o->prototype);
			if ( p )
			{
				const var &po = *p.second;
				if ( po.type == OBJECT )
				{
					o = po.o;
				}
				else
				{
					o = 0;
				}
			}
		}
		else
		{
			o = 0;
		}
	}
	CEXCEPTION_ERROR("null var");
	static var NULLVAR;
	return NULLVAR;
}

static object *CreateEntries( const int *ops, int &pc, object *o, int entries )
{
	for (int i=0; i<entries; i++)
	{
		int key = ops[pc++];
		Map<var>::Iterator f = o->m_tbl.find(key);
		if ( !f )
		{
			var &tv = o->m_tbl[key];
			tv.type = OBJECT;
			tv.o = new object;
			o = tv.o;
		}
		else
		{
			var &tv = *f.second;
			if ( tv.type == OBJECT )
			{
				o = tv.o;
			}
			else
			{
				tv.type = OBJECT;
				tv.o = new object;
				o = tv.o;
			}
		}
	}
	return o;
}

static void PushEntry( const int *ops, int &pc, object *o, int entries, vmstate &state )
{
	for (int i=0; i<entries; i++)
	{
		int key = ops[pc++];
		const var &v = FindVar( o, key, state );
		if ( v.type == OBJECT )
		{
			o = v.o;
		}
		else
		{
			CEXCEPTION_ERROR("bad type");
		}
	}
	int key = ops[pc++];
	state.stack.push( FindVar( o, key, state ) );
}

void RunVM( int const *ops, int numOps, int loc, vmstate &state )
{
	{
	var &v = state.globals[Hash(L"cfunc")];
	v.type = CFUNCTION;
	v.cfunc = testcfunc;
	}
	{
	var &v = state.globals[Hash(L"printffloat")];
	v.type = CFUNCTION;
	v.cfunc = printffloat;
	}
	{
	var &v = state.globals[Hash(L"_implements")];
	v.type = CFUNCTION;
	v.cfunc = testimplements;
	}
	{
	var &v = state.globals[Hash(L"pushback")^INTEGERARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	pushbackInteger;
	}
	{
	var &v = state.globals[Hash(L"pushback")^FLOATINGPOINTARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	pushbackFloatingPoint;
	}
	{
	var &v = state.globals[Hash(L"pushback")^OBJECTARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	pushbackObject;
	}
	{
	var &v = state.globals[Hash(L"pushback")^USERPTRARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	pushbackUserPtr;
	}
	{
	var &v = state.globals[Hash(L"size")^INTEGERARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	sizeInteger;
	}
	{
	var &v = state.globals[Hash(L"size")^FLOATINGPOINTARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	sizeFloatingPoint;
	}
	{
	var &v = state.globals[Hash(L"size")^OBJECTARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	sizeObject;
	}
	{
	var &v = state.globals[Hash(L"size")^USERPTRARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	sizeUserPtr;
	}
	{
	var &v = state.globals[Hash(L"resize")^INTEGERARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	resizeInteger;
	}
	{
	var &v = state.globals[Hash(L"resize")^FLOATINGPOINTARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	resizeFloatingPoint;
	}
	{
	var &v = state.globals[Hash(L"resize")^OBJECTARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	resizeObject;
	}
	{
	var &v = state.globals[Hash(L"resize")^USERPTRARRAY];
	v.type = CFUNCTION;
	v.cfunc = 	resizeUserPtr;
	}
	int pc = loc;
	bool done = false;
	state.envStack.reset( 0 );
	//state.envStack.push( state.stack.size() );
	state.stack.reset( 0 );
	while ( !done )
	{
		int op = ops[pc++];
		CEXCEPTION_ERROR_CONDITION( op >= 0 && op < OPC_MAX, "bad opcode" );
		const char *opname = opnames[op];
		switch(op)
		{
		case OPC_PUSHTOP:
			{
				state.stack.push( state.stack.top() );
			}
			break;
		case OPC_PUSHENV:
			{
				int reqStack = ops[pc++];
				state.envStack.push( state.stack.size() );
				state.stack.reserve( reqStack );
			}
			break;
		case OPC_POP:
			{
				int num = ops[pc++];
				state.stack.pop( num );
			}
			break;
		case OPC_POPENV:
			{
				state.stack.reset( state.envStack.top() );
				state.envStack.pop();
			}
			break;
		case OPC_RET:
			{
				if ( state.pcStack.empty() )
				{
					done = true;
					break;
				}
				else
				{
					pc = state.pcStack.top();
					state.pcStack.pop();
				}
			}
			break;
		case OPC_MAKEVAR:
			{
				VARTYPE type = (VARTYPE)ops[pc++];
				int index = ops[pc++];
				int prototype = ops[pc++];
				var &item = state.stack[state.envStack.top()+index];
				item.type = type;
				if ( type == OBJECT )
				{
					item.o = new object;
					if ( prototype )
					{
						item.o->prototype = prototype;
					}
				}
				else
				if ( type == INTEGERARRAY )
				{
					item.iArrayPtr = new arrayvar<int>;
				}
				else
				if ( type == FLOATINGPOINTARRAY )
				{
					item.fArrayPtr = new arrayvar<float>;
				}
				else
				if ( type == OBJECTARRAY )
				{
					item.oArrayPtr = new arrayvar<object*>;
				}
				else
				{
					item.o = 0;
				}
			}
			break;
		case OPC_CONSTRUCTVAR:
			{
				VARTYPE type = (VARTYPE)ops[pc++];
				int index = ops[pc++];				
				var &obj = state.stack.top(); // top but don't pop
				CEXCEPTION_ERROR_CONDITION( obj.type == OBJECT, "var is not object" );
				var &item = obj.o->m_tbl[index];

				item.type = type;
				if ( type == OBJECT )
				{
					item.o = new object;
				}
				else
				if ( type == INTEGERARRAY )
				{
					item.iArrayPtr = new arrayvar<int>;
				}
				else
				if ( type == FLOATINGPOINTARRAY )
				{
					item.fArrayPtr = new arrayvar<float>;
				}
				else
				if ( type == OBJECTARRAY )
				{
					item.oArrayPtr = new arrayvar<object*>;
				}
				else
				{
					item.o = 0;
				}
			}
			break;
		case OPC_CONSTRUCTVARFROMGLOBAL:
			{
				int index = ops[pc++];				
				int gindex = ops[pc++];				
				var &obj = state.stack.top(); // top but don't pop
				CEXCEPTION_ERROR_CONDITION( obj.type == OBJECT, "var is not object" );
				var &item = state.globals[gindex];
				obj.o->m_tbl[index] = item;
			}
			break;
		case OPC_MAKEVARG:
			{
				VARTYPE type = (VARTYPE)ops[pc++];
				int index = ops[pc++];
				int prototype = ops[pc++];
				var &item = state.globals[index];
				item.type = type;
				if ( type == OBJECT )
				{
					item.o = new object;
					if ( prototype )
					{
						item.o->prototype = prototype;
					}
				}
				else
				if ( type == INTEGERARRAY )
				{
					item.iArrayPtr = new arrayvar<int>;
				}
				else
				if ( type == FLOATINGPOINTARRAY )
				{
					item.fArrayPtr = new arrayvar<float>;
				}
				else
				if ( type == OBJECTARRAY )
				{
					item.oArrayPtr = new arrayvar<object*>;
				}
				else
				{
					item.i = 0;
				}
			}
			break;
		case OPC_PUSHI:
			{
				int value = ops[pc++];
				var &item = state.stack.push();
				item.type = INTEGER;
				item.i = value;
			}
			break;
		case OPC_CALL:
			{
				var val = state.stack.top(); state.stack.pop();
				if ( val.type == CFUNCTION )
				{					
					state.envStack.push( state.stack.size() );
					bool res = val.cfunc( state );
					// cfunc should clear the stack based on args
					state.envStack.pop();
					if ( res )
					{
						state.stack.push( state.rv );
					}
				}
				else
				if ( val.type == VMFUNCTION )
				{
					state.pcStack.push( pc );
					pc = val.i;
				}
				else
				{
					CEXCEPTION_ERROR("bad type");
				}
			}
			break;
		case OPC_CALLG:
			{
				int index = ops[pc++];
				const var &val = state.globals[index];
				if ( val.type == CFUNCTION )
				{
					state.envStack.push( state.stack.size() );
					bool res = val.cfunc( state );
					// cfunc should clear the stack based on args
					state.envStack.pop();
					if ( res )
					{
						state.stack.push( state.rv );
					}
				}
				else
				if ( val.type == VMFUNCTION )
				{
					state.pcStack.push( pc );
					pc = val.i;
				}
				else
				{
					CEXCEPTION_ERROR("bad type");
				}
			}
			break;
		case OPC_MUL:
			{
				var v1 = state.stack.top(); state.stack.pop();
				var v0 = state.stack.top(); state.stack.pop();
				VARTYPE ct = CombinedType( v0.type, v1.type );
				var &newItem = state.stack.push();
				newItem.type = ct;
				if ( ct == INTEGER )
				{
					newItem.i = ToInt( v0 ) * ToInt( v1 );
				}
				else
				if ( ct == FLOATINGPOINT )
				{
					newItem.f = ToFloat( v0 ) * ToFloat( v1 );
				}
				else
				{
					newItem.i = 0;
				}
			}
			break;
		case OPC_ADD:
			{
				var v1 = state.stack.top(); state.stack.pop();
				var v0 = state.stack.top(); state.stack.pop();
				VARTYPE ct = CombinedType( v0.type, v1.type );
				var &newItem = state.stack.push();
				newItem.type = ct;
				if ( ct == INTEGER )
				{
					newItem.i = ToInt( v0 ) + ToInt( v1 );
				}
				else
				if ( ct == FLOATINGPOINT )
				{
					newItem.f = ToFloat( v0 ) + ToFloat( v1 );
				}
				else
				{
					newItem.i = 0;
				}
			}
			break;
		case OPC_SUB:
			{
				var v1 = state.stack.top(); state.stack.pop();
				var v0 = state.stack.top(); state.stack.pop();
				VARTYPE ct = CombinedType( v0.type, v1.type );
				var &newItem = state.stack.push();
				newItem.type = ct;
				if ( ct == INTEGER )
				{
					newItem.i = ToInt( v0 ) - ToInt( v1 );
				}
				else
				if ( ct == FLOATINGPOINT )
				{
					newItem.f = ToFloat( v0 ) - ToFloat( v1 );
				}
				else
				{
					newItem.i = 0;
				}
			}
			break;
		case OPC_DIV:
			{
				var v1 = state.stack.top(); state.stack.pop();
				var v0 = state.stack.top(); state.stack.pop();
				VARTYPE ct = CombinedType( v0.type, v1.type );
				var &newItem = state.stack.push();
				newItem.type = ct;
				if ( ct == INTEGER )
				{
					newItem.i = ToInt( v0 ) / ToInt( v1 );
				}
				else
				if ( ct == FLOATINGPOINT )
				{
					newItem.f = ToFloat( v0 ) / ToFloat( v1 );
				}
				else
				{
					newItem.i = 0;
				}
			}
			break;
		case OPC_LSS:
			{
				var v1 = state.stack.top(); state.stack.pop();
				var v0 = state.stack.top(); state.stack.pop();
				VARTYPE ct = CombinedType( v0.type, v1.type );
				var &newItem = state.stack.push();
				newItem.type = INTEGER;
				if ( ct == INTEGER )
				{
					newItem.i = ToInt( v0 ) < ToInt( v1 );
				}
				else
				if ( ct == FLOATINGPOINT )
				{
					newItem.i = ToFloat( v0 ) < ToFloat( v1 );
				}
				else
				{
					newItem.i = 0;
				}
			}
			break;
		case OPC_GTR:
			{
				var v1 = state.stack.top(); state.stack.pop();
				var v0 = state.stack.top(); state.stack.pop();
				VARTYPE ct = CombinedType( v0.type, v1.type );
				var &newItem = state.stack.push();
				newItem.type = INTEGER;
				if ( ct == INTEGER )
				{
					newItem.i = ToInt( v0 ) > ToInt( v1 );
				}
				else
				if ( ct == FLOATINGPOINT )
				{
					newItem.i = ToFloat( v0 ) > ToFloat( v1 );
				}
				else
				{
					newItem.i = 0;
				}
			}
			break;
		case OPC_NEG:
			{
				var v0 = state.stack.top(); state.stack.pop();
				var &newItem = state.stack.push();
				newItem.type = v0.type;
				if ( v0.type == INTEGER )
				{
					newItem.i = -v0.i;
				}
				else
				if ( v0.type == FLOATINGPOINT )
				{
					newItem.f = -v0.f;
				}
				else
				{
					newItem.type = INTEGER;
					newItem.i = 0;
				}
			}
			break;
		case OPC_JF:
			{
				int dest = ops[pc++];
				var v0 = state.stack.top(); state.stack.pop();
				CEXCEPTION_ERROR_CONDITION( v0.type == INTEGER, "var is not integer" );
				if ( !v0.i )
					pc = dest;
			}
			break;
		case OPC_JMP:
			{
				int dest = ops[pc++];
				pc = dest;
			}
			break;
		case OPC_POPR:
			{
				state.rv = state.stack.top(); state.stack.pop();
			}
			break;
		case OPC_PUSHR:
			{
				state.stack.push( state.rv );
			};
			break;
		case OPC_POPITEM:
			{
				var v0 = state.stack.top(); state.stack.pop();

				int entries = ops[pc++]-1;
				int index = ops[pc++];
				var &item = state.stack[state.envStack.top()+index];
				if ( item.type == OBJECT && entries )
				{
					object *o = item.o;
					o = CreateEntries( ops, pc, o, entries-1 );
					int key = ops[pc++];
					o->m_tbl[key] = v0;
				}
				else
				if ( entries )
				{
					CEXCEPTION_ERROR("unexpected entries on type");
					pc+=entries;
				}
				else
				{
					if ( item.type == INTEGER )
					{
						item.i = ToInt( v0 );
					}
					else
					if ( item.type == FLOATINGPOINT )
					{
						item.f = ToFloat( v0 );
					}
					else
					if ( item.type == OBJECT && v0.type == OBJECT)
					{
						item.o = v0.o;
					}
					else
					{
						CEXCEPTION_ERROR("bad type");
					}
				}
			}
			break;
		case OPC_PUSHITEM:
			{
				int entries = ops[pc++]-1;
				int index = ops[pc++];
				var &item = state.stack[state.envStack.top()+index];
				if ( item.type == OBJECT && entries )
				{
					object *o = item.o;
					PushEntry( ops, pc, o, entries-1, state );
				}
				else
				if ( entries )
				{
					CEXCEPTION_ERROR_CONDITION( entries == 1, "unexpected entries on type");
					int lookup = ops[pc];
					int hash = (lookup) ^ item.type;
					pc+=entries;
					state.stack.push( state.globals[hash] );
				}
				else
				{
					state.stack.push( item );
				}
			}
			break;
		case OPC_PUSHSUPER:
			{
				int entries = ops[pc++]-1;
				int index = ops[pc++];
				const var &item = state.stack[state.envStack.top()+index];
				if ( item.type == OBJECT && entries )
				{
					object *o = item.o;
					CEXCEPTION_ERROR_CONDITION( o->prototype != -1, "object should have prototype" );
					const var &proto_item = state.globals[o->prototype];
					CEXCEPTION_ERROR_CONDITION( proto_item.type == OBJECT, "prototype is not object" );
					o = proto_item.o;
					CEXCEPTION_ERROR_CONDITION( o->prototype != -1, "prototype should have prototype" );
					const var &super_proto_item = state.globals[o->prototype];
					CEXCEPTION_ERROR_CONDITION( super_proto_item.type == OBJECT, "super prototype is not object" );
					o = super_proto_item.o;

					PushEntry( ops, pc, o, entries-1, state );
				}
				else
				if ( entries )
				{
					CEXCEPTION_ERROR("bad number of entries on type");
					pc+=entries;
				}
				else
				{
					CEXCEPTION_ERROR("err");
				}
			}
			break;
		case OPC_POPITEMG:
			{
				var v0 = state.stack.top(); state.stack.pop();

				int entries = ops[pc++]-1;
				int index = ops[pc++];
				var &item = state.globals[index];
				if ( item.type == OBJECT && entries )
				{
					object *o = item.o;
					o = CreateEntries( ops, pc, o, entries-1 );
					int key = ops[pc++];
					o->m_tbl[key] = v0;
				}
				else
				if ( entries )
				{
					CEXCEPTION_ERROR("bad number of entries on type");
					pc+=entries;
				}
				else
				{
					if ( item.type == INTEGER )
					{
						item.i = ToInt( v0 );
					}
					else
					if ( item.type == FLOATINGPOINT )
					{
						item.f = ToFloat( v0 );
					}
					else
					if ( item.type == OBJECT && v0.type == OBJECT)
					{
						item.o = v0.o;
					}
					else
					{
						CEXCEPTION_ERROR("err");
					}
				}
			}
			break;
		case OPC_PUSHITEMG:
			{
				int entries = ops[pc++]-1;
				int index = ops[pc++];
				var &item = state.globals[index];
				if ( item.type == OBJECT && entries )
				{
					object *o = item.o;
					PushEntry( ops, pc, o, entries-1, state );
				}
				else
				if ( entries )
				{
					CEXCEPTION_ERROR_CONDITION( entries == 1, "unexpected entries on type");
					int lookup = ops[pc];
					int hash = (lookup) ^ item.type;
					pc+=entries;
					state.stack.push( state.globals[hash] );
				}
				else
				{
					state.stack.push( item );
				}
			}
			break;
		case OPC_PUSHF:
			{
				float v;
				memcpy( &v, &ops[pc++], 4 );
				var &item = state.stack.push();
				item.type = FLOATINGPOINT;
				item.f = v;
			}
			break;
		case OPC_POPITEMGARRAY:
			{
				var v1 = state.stack.top(); state.stack.pop();
				var v0 = state.stack.top(); state.stack.pop();
				int lookup = ops[pc++];
				int index = ToInt( v1 );
				var &v = state.globals[lookup];
				if ( v.type == INTEGERARRAY )
				{
					v.iArrayPtr->m_items[index] = ToInt( v0 );
				}
				else
				if ( v.type == FLOATINGPOINTARRAY )
				{
					v.fArrayPtr->m_items[index] = ToFloat( v0 );
				}
				else
				if ( v.type == OBJECTARRAY && v0.type == OBJECT )
				{
					v.oArrayPtr->m_items[index] = v0.o;
				}
				else
				if ( v.type == USERPTRARRAY && v0.type == USERPTR )
				{
					v.uArrayPtr->m_items[index] = v0.u;
				}
				else
				{
					CEXCEPTION_ERROR( "err" );
				}
			}
			break;
		case OPC_POPITEMARRAY:
			{
				var v1 = state.stack.top(); state.stack.pop();
				var v0 = state.stack.top(); state.stack.pop();
				int lookup = ops[pc++];
				int index = ToInt( v1 );
				var &v = state.stack[state.envStack.top()+lookup];
				if ( v.type == INTEGERARRAY )
				{
					v.iArrayPtr->m_items[index] = ToInt( v0 );
				}
				else
				if ( v.type == FLOATINGPOINTARRAY )
				{
					v.fArrayPtr->m_items[index] = ToFloat( v0 );
				}
				else
				if ( v.type == OBJECTARRAY && v0.type == OBJECT )
				{
					v.oArrayPtr->m_items[index] = v0.o;
				}
				else
				if ( v.type == USERPTRARRAY && v0.type == USERPTR )
				{
					v.uArrayPtr->m_items[index] = v0.u;
				}
				else
				{
					CEXCEPTION_ERROR( "err" );
				}
			}
			break;
		case OPC_PUSHITEMGARRAY:
			{
				var v1 = state.stack.top(); state.stack.pop();
				int index = ToInt( v1 );
				int lookup = ops[pc++];
				var &v = state.globals[lookup];
				if ( v.type == INTEGERARRAY )
				{
					var &v0 = state.stack.push();
					v0.type = INTEGER;
					v0.i = v.iArrayPtr->m_items[index];
				}
				else
				if ( v.type == FLOATINGPOINTARRAY )
				{
					var &v0 = state.stack.push();
					v0.type = FLOATINGPOINT;
					v0.f = v.fArrayPtr->m_items[index];
				}
				else
				if ( v.type == OBJECTARRAY )
				{
					var &v0 = state.stack.push();
					v0.type = OBJECT;
					v0.o = v.oArrayPtr->m_items[index];
				}
				else
				if ( v.type == USERPTRARRAY )
				{
					var &v0 = state.stack.push();
					v0.type = USERPTR;
					v0.u = v.uArrayPtr->m_items[index];
				}
				else
				{
					CEXCEPTION_ERROR( "err" );
				}
			}
			break;
		case OPC_PUSHITEMARRAY:
			{
				var v1 = state.stack.top(); state.stack.pop();
				int index = ToInt( v1 );
				int lookup = ops[pc++];
				var &v = state.stack[state.envStack.top()+lookup];
				if ( v.type == INTEGERARRAY )
				{
					var &v0 = state.stack.push();
					v0.type = INTEGER;
					v0.i = v.iArrayPtr->m_items[index];
				}
				else
				if ( v.type == FLOATINGPOINTARRAY )
				{
					var &v0 = state.stack.push();
					v0.type = FLOATINGPOINT;
					v0.f = v.fArrayPtr->m_items[index];
				}
				else
				if ( v.type == OBJECTARRAY )
				{
					var &v0 = state.stack.push();
					v0.type = OBJECT;
					v0.o = v.oArrayPtr->m_items[index];
				}
				else
				if ( v.type == USERPTRARRAY )
				{
					var &v0 = state.stack.push();
					v0.type = USERPTR;
					v0.u = v.uArrayPtr->m_items[index];
				}
				else
				{
					CEXCEPTION_ERROR( "err" );
				}
			}
			break;
		default:
			CEXCEPTION_ERROR( "err" );
			break;
		}
	}
}

bool RunVMExp( int const *ops, int numOps, int loc, vmstate &state )
{
	SetJmpChain_s jmpchain;
	cexception_push( &jmpchain );
	if ( setjmp( jmpchain.env ) == 0 )
	{
		RunVM( ops, numOps, loc, state );
		cexception_pop();
		return true;
	}
	else
	{
		cexception_pop();
		return false;
	}
}
