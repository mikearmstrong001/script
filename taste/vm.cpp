#include "vm.h"
#include <memory.h>
#include <stdio.h>
#include <malloc.h>

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
	"OPC_PUSHSTR",
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
	"OPC_CALLTYPED",
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
	"OPC_MAKESTRUCT",
	"OPC_MAKESTRUCTG",
};

static bool pushbackFloatingPoint( vmstate &state )
{
	const var &arg1 = state.GetArg( -2 );
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == FLOATINGPOINTARRAY && arg1.type == FLOATINGPOINT, "Expecting FloatingPoint in builtin array pushback" );

	arg0.fArrayPtr->m_items.push_back( arg1.f );

	return false;
}

static bool pushbackInteger( vmstate &state )
{
	const var &arg1 = state.GetArg( -2 );
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == INTEGERARRAY && arg1.type == INTEGER, "Expecting Integer in builtin array pushback" );

	arg0.iArrayPtr->m_items.push_back( arg1.i );

	return false;
}

static bool pushbackUserPtr( vmstate &state )
{
	const var &arg1 = state.GetArg( -2 );
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == USERPTRARRAY && arg1.type == USERPTR, "Expecting userptr in builtin array pushback" );

	arg0.uArrayPtr->m_items.push_back( arg1.u );

	return false;
}

static bool pushbackString( vmstate &state )
{
	const var &arg1 = state.GetArg( -2 );
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == STRINGARRAY && arg1.type == STRING, "Expecting string in builtin array pushback" );

	arg0.strArrayPtr->m_items.push_back( arg1.str );

	return false;
}

static bool pushbackStruct( vmstate &state )
{
	const var &arg1 = state.GetArg( -2 );
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == STRUCTARRAY && arg1.type == STRUCT, "Expecting struct in builtin array pushback" );

	arg0.sArrayPtr->m_items.push_back( arg1.s );

	return false;
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

static bool sizeString( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 1 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == STRINGARRAY, "Expecting String in builtin array size" );

	state.SetReturn( arg0.strArrayPtr->m_items.size() );

	return true;
}

static bool sizeStruct( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	state.stack.pop( 1 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == STRUCTARRAY, "Expecting Struct in builtin array size" );

	state.SetReturn( arg0.sArrayPtr->m_items.size() );

	return true;
}

static bool resizeFloatingPoint( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	int arg1 = state.GetArgAsInt( -2 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == FLOATINGPOINTARRAY, "Expecting Integer in builtin array size" );
	
	arg0.fArrayPtr->m_items.resize( arg1 );

	return false;
}

static bool resizeInteger( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	int arg1 = state.GetArgAsInt( -2 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == INTEGERARRAY, "Expecting Integer in builtin array size" );

	arg0.iArrayPtr->m_items.resize( arg1 );

	return false;
}

static bool resizeUserPtr( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	int arg1 = state.GetArgAsInt( -2 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == USERPTRARRAY, "Expecting Integer in builtin array size" );

	arg0.uArrayPtr->m_items.resize( arg1 );

	return false;
}

static bool resizeString( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	int arg1 = state.GetArgAsInt( -2 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == STRINGARRAY, "Expecting String in builtin array size" );

	arg0.strArrayPtr->m_items.resize( arg1 );

	return false;
}

static bool resizeStruct( vmstate &state )
{
	const var &arg0 = state.GetArg( -1 );
	int arg1 = state.GetArgAsInt( -2 );
	state.stack.pop( 2 );

	CEXCEPTION_ERROR_CONDITION( arg0.type == STRUCTARRAY, "Expecting Struct in builtin array size" );

	arg0.sArrayPtr->m_items.resize( arg1 );

	return false;
}

bool testcfunc( vmstate &state )
{
	int i0 = state.GetArgAsInt( -1 );
	state.rv.type = INTEGER;
	state.rv.i = i0 * 2;
	state.stack.pop(1);
	return true;
}

bool implements( vmstate &state )
{
	const var &arg0 = state.GetArg( -2 );
	int arg1 = state.GetArgAsInt( -1 );
	int typehash = (arg0.type == STRUCT) ? arg0.s.type : arg0.type;
	vminterface &iface = state.ifaces[ arg1 ];
	Map<int>::ConstIterator f = iface.typeCache.cfind( typehash );
	int found = 1;
	if ( !f )
	{
		for ( int i=0; i<iface.interfaceFunctions.size(); i++)
		{
			Map<var>::ConstIterator it = state.globals.cfind( iface.interfaceFunctions[i]^typehash );
			if ( !it )
			{
				found = 0;
				break;
			}
		}
		iface.typeCache[typehash] = found;
	}
	else
	{
		found = *f.second;
	}
	state.rv.type = INTEGER;
	state.rv.i = found;
	state.stack.pop(2);
	return true;
}

bool printffloat( vmstate &state )
{
	float f = state.GetArgAsFloat( -1 );
	printf( "%f\n", f );
	state.stack.pop(1);
	return false;
}


struct functionreg_s
{
	int hash;
	bool (*func_f)( vmstate & );
};

static functionreg_s globalFuncs[] =
{
	{ Hash(L"cfunc"), testcfunc },
	{ Hash(L"printffloat"), printffloat },
	{ Hash(L"implements"), implements },
	{ Hash(L"pushback")^INTEGERARRAY, pushbackInteger },
	{ Hash(L"pushback")^FLOATINGPOINTARRAY, pushbackFloatingPoint },
	{ Hash(L"pushback")^USERPTRARRAY, pushbackUserPtr },
	{ Hash(L"pushback")^STRINGARRAY, pushbackString },
	{ Hash(L"pushback")^STRUCTARRAY, pushbackStruct },
	{ Hash(L"size")^INTEGERARRAY, sizeInteger },
	{ Hash(L"size")^FLOATINGPOINTARRAY, sizeFloatingPoint },
	{ Hash(L"size")^USERPTRARRAY, sizeUserPtr },
	{ Hash(L"size")^STRINGARRAY, sizeString },
	{ Hash(L"resize")^INTEGERARRAY, resizeInteger },
	{ Hash(L"resize")^FLOATINGPOINTARRAY, resizeFloatingPoint },
	{ Hash(L"resize")^USERPTRARRAY, resizeUserPtr },
	{ Hash(L"resize")^STRINGARRAY, resizeString },
	0, NULL
};

void vmRegister( vmstate &state, functionreg_s reg[] )
{
	for (int i=0; reg[i].func_f; i++)
	{
		var &v = state.globals[reg[i].hash];
		v.type = CFUNCTION;
		v.cfunc = 	reg[i].func_f;
	}
}

static void PopItem( var &item, var &v0, int const *ops, int &pc, int entries, vmstate &state )
{
	if ( item.type == STRUCT )
	{
		CEXCEPTION_ERROR_CONDITION( entries==1, "struct type expects single entry" );
		int eleIndex = ops[pc++];
		int eleType = ops[pc++];
		if ( eleIndex & 0x80000000 )
		{
			vmstructprops *s = state.structProps[eleType];
			CEXCEPTION_ERROR_CONDITION( s!=NULL, "unknown vmstruct" );
			CEXCEPTION_ERROR_CONDITION( v0.type == STRUCT, "expected struct" );
			int num = s->properties.size();
			int offset = eleIndex & 0x7fffffff;
			CEXCEPTION_ERROR_CONDITION( eleType == v0.s.type, "unepected type" );
			for (int i=0; i<num; i++)
			{
				item.s.s->m_data[item.s.offset+offset+i] = v0.s.s->m_data[ v0.s.offset+i ];
			}
		}
		else
		{
			CEXCEPTION_ERROR_CONDITION((eleIndex & 0x80000000) == 0, "unsupported" );
			item.s.s->m_data[item.s.offset+eleIndex] = v0;
		}
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
		{
			CEXCEPTION_ERROR("err");
		}
	}
}

static void PushItem( var &item, int const *ops, int &pc, int entries, vmstate &state )
{
	if ( item.type == STRUCT )
	{
		if ( entries == 0 )
		{
			state.stack.push( item );
		}
		else
		{
			CEXCEPTION_ERROR_CONDITION( entries==1, "struct type expects single entry" );
			int eleIndex = ops[pc++];
			int eleType = ops[pc++];
			if ( eleIndex & 0x80000000 )
			{
				var &v = state.stack.push();
				v.type = STRUCT;
				v.s.offset = eleIndex & 0x7fffffff;
				v.s.s = item.s.s;
				v.s.type = eleType;
			}
			else
			{
				CEXCEPTION_ERROR_CONDITION((eleIndex & 0x80000000) == 0, "unsupported" );
				state.stack.push( item.s.s->m_data[item.s.offset + eleIndex] );
			}
		}
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

static void MakeStruct( var &item, int type, vmstate &state )
{
	vmstructprops *props = state.structProps[type];
	vmstruct *structmem = (vmstruct *)malloc( sizeof(vmstruct) + sizeof(var)*props->properties.size() );
	item.type = STRUCT;
	item.s.offset = 0;
	item.s.type = type;
	item.s.s = structmem;
	item.s.s->m_creationType = type;
	item.s.s->m_size = props->properties.size();
	for ( int i=0; i<props->properties.size(); i++)
	{
		var &curitem = item.s.s->m_data[i];
		curitem.type = (VARTYPE)props->properties[i].itemType;
		if ( curitem.type == INTEGERARRAY )
		{
			curitem.iArrayPtr = new vmarrayvar<int>;
		}
		else
		if ( curitem.type == FLOATINGPOINTARRAY )
		{
			curitem.fArrayPtr = new vmarrayvar<float>;
		}
		else
		if ( curitem.type == USERPTRARRAY )
		{
			curitem.uArrayPtr = new vmarrayvar<void*>;
		}
		else
		if ( curitem.type == STRINGARRAY )
		{
			curitem.strArrayPtr = new vmarrayvar<vmstring*>;
		}
		else
		if ( curitem.type == STRUCTARRAY )
		{
			curitem.sArrayPtr = new vmarrayvar<vmstructref>;
		}
		else
		{
			curitem.i = 0;
		}
	}
}

static void PushItemArray( var &v, int index, vmstate &state )
{
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
	if ( v.type == USERPTRARRAY )
	{
		var &v0 = state.stack.push();
		v0.type = USERPTR;
		v0.u = v.uArrayPtr->m_items[index];
	}
	else
	if ( v.type == STRINGARRAY )
	{
		var &v0 = state.stack.push();
		v0.type = STRING;
		v0.str = v.strArrayPtr->m_items[index];
	}
	else
	if ( v.type == STRUCTARRAY )
	{
		var &v0 = state.stack.push();
		v0.type = STRUCT;
		v0.s = v.sArrayPtr->m_items[index];
	}
	else
	{
		CEXCEPTION_ERROR( "err" );
	}
}

static void PopItemArray( var &v, int index, var &v0 )
{
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
	if ( v.type == USERPTRARRAY && v0.type == USERPTR )
	{
		v.uArrayPtr->m_items[index] = v0.u;
	}
	else
	if ( v.type == STRINGARRAY && v0.type == STRING )
	{
		v.strArrayPtr->m_items[index] = v0.str;
	}
	else
	if ( v.type == STRUCTARRAY && v0.type == STRUCT )
	{
		v.sArrayPtr->m_items[index] = v0.s;
	}
	else
	{
		CEXCEPTION_ERROR( "err" );
	}
}

static void MakeVar( var &item, VARTYPE type )
{
	item.type = type;
	if ( type == INTEGERARRAY )
	{
		item.iArrayPtr = new vmarrayvar<int>;
	}
	else
	if ( type == FLOATINGPOINTARRAY )
	{
		item.fArrayPtr = new vmarrayvar<float>;
	}
	else
	if ( type == USERPTRARRAY )
	{
		item.uArrayPtr = new vmarrayvar<void*>;
	}
	else
	if ( type == STRINGARRAY )
	{
		item.strArrayPtr = new vmarrayvar<vmstring*>;
	}
	else
	if ( type == STRUCTARRAY )
	{
		item.sArrayPtr = new vmarrayvar<vmstructref>;
	}
	else
	{
		item.i = 0;
	}
}

static void Call( const var &val, int pc, vmstate &state )
{
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

void RunVM( int const *ops, int numOps, int loc, vmstate &state )
{
	vmRegister( state, globalFuncs );
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
				MakeVar( item, type );
			}
			break;
		case OPC_MAKEVARG:
			{
				VARTYPE type = (VARTYPE)ops[pc++];
				int index = ops[pc++];
				int prototype = ops[pc++];
				var &item = state.globals[index];
				MakeVar( item, type );
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
				int index = ops[pc++];
				const var &val = state.globals[index];
				Call( val, pc, state );
			}
			break;
		case OPC_CALLTYPED:
			{
				const var &valtyped = state.stack.top();
				int index = ops[pc++];
				int typehash = (valtyped.type == STRUCT) ? valtyped.s.type : valtyped.type;
				const var &val = state.globals[index^typehash];
				Call( val, pc, state );
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
				PopItem( item, v0, ops, pc, entries, state );
			}
			break;
		case OPC_PUSHITEM:
			{
				int entries = ops[pc++]-1;
				int index = ops[pc++];
				var &item = state.stack[state.envStack.top()+index];
				PushItem( item, ops, pc, entries, state ); 
			}
			break;
		case OPC_POPITEMG:
			{
				var v0 = state.stack.top(); state.stack.pop();

				int entries = ops[pc++]-1;
				int index = ops[pc++];
				var &item = state.globals[index];
				PopItem( item, v0, ops, pc, entries, state );
			}
			break;
		case OPC_PUSHITEMG:
			{
				int entries = ops[pc++]-1;
				int index = ops[pc++];
				var &item = state.globals[index];
				PushItem( item, ops, pc, entries, state ); 
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
				PopItemArray( v, index, v0 );
			}
			break;
		case OPC_POPITEMARRAY:
			{
				var v1 = state.stack.top(); state.stack.pop();
				var v0 = state.stack.top(); state.stack.pop();
				int lookup = ops[pc++];
				int index = ToInt( v1 );
				var &v = state.stack[state.envStack.top()+lookup];
				PopItemArray( v, index, v0 );
			}
			break;
		case OPC_PUSHITEMGARRAY:
			{
				var v1 = state.stack.top(); state.stack.pop();
				int index = ToInt( v1 );
				int lookup = ops[pc++];
				var &v = state.globals[lookup];
				PushItemArray( v, index, state );
			}
			break;
		case OPC_PUSHITEMARRAY:
			{
				var v1 = state.stack.top(); state.stack.pop();
				int index = ToInt( v1 );
				int lookup = ops[pc++];
				var &v = state.stack[state.envStack.top()+lookup];
				PushItemArray( v, index, state );
			}
			break;
		case OPC_PUSHSTR:
			{
				int len = ops[pc++];
				wchar_t *s = (wchar_t*)&ops[pc];
				vmstring *str = new vmstring( s, len );
				var &v0 = state.stack.push();
				v0.type = STRING;
				v0.str = str;
				pc += (len+1)/2;
			}
			break;
		case OPC_MAKESTRUCT:
			{
				int type = ops[pc++];
				int index = ops[pc++];
				var &item = state.stack[state.envStack.top()+index];
				MakeStruct( item, type, state );
			}
			break;
		case OPC_MAKESTRUCTG:
			{
				int type = ops[pc++];
				int index = ops[pc++];
				var &item = state.globals[index];
				MakeStruct( item, type, state );
			}
			break;
		default:
			CEXCEPTION_ERROR( "unimplemented OP" );
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
