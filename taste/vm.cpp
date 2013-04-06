#include "vm.h"
#include <memory.h>

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
	state.stack.pop(1);
	return false;
}

const var &FindVar( object const *top, int key, vmstate &state )
{
	// first check object;
	const object *o = top;
	while ( o )
	{
		Map<var>::Iterator f = o->m_tbl.Find( key );
		if ( f )
		{
			return *f.second;
		}
		// not found check prototype
		if ( o->prototype != -1 )
		{
			const var &po = state.globals.Get(o->prototype);
			if ( po.type == OBJECT )
			{
				o = po.o;
			}
			else
			{
				o = 0;
			}
		}
		else
		{
			o = 0;
		}
	}
	cexception_error( "failed to find key" );
	static var NULLVAR;
	return NULLVAR;
}

void RunVM( int const *ops, int numOps, int loc, vmstate &state )
{
	{
	var &v = state.globals.Add(Hash(L"cfunc"));
	v.type = CFUNCTION;
	v.cfunc = testcfunc;
	}
	{
	var &v = state.globals.Add(Hash(L"printffloat"));
	v.type = CFUNCTION;
	v.cfunc = printffloat;
	}
	int pc = loc;
	bool done = false;
	state.envStack.reset( 0 );
	//state.envStack.push( state.stack.size() );
	state.stack.reset( 0 );
	while ( !done )
	{
		int op = ops[pc++];
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
				cexception_error( obj.type == OBJECT, "type expected to be object" );
				var &item = obj.o->m_tbl.Add(index);

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
				cexception_error( obj.type == OBJECT, "type expected to be object" );
				var &item = state.globals.Add(gindex);
				obj.o->m_tbl.Set(index, item);
			}
			break;
		case OPC_MAKEVARG:
			{
				VARTYPE type = (VARTYPE)ops[pc++];
				int index = ops[pc++];
				int prototype = ops[pc++];
				var &item = state.globals.Add(index);
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
					cexception_error( "unexpected type for call" );
				}
			}
			break;
		case OPC_CALLG:
			{
				int index = ops[pc++];
				const var &val = state.globals.Get(index);
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
					cexception_error( "unexpected type for call" );
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
				cexception_error( v0.type == INTEGER, "type expected to be integer" );
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
					for (int i=0; i<entries-1; i++)
					{
						int key = ops[pc++];

						int f = o->m_tbl.Find(key);
						if ( f == -1 )
						{
							var &tv = o->m_tbl.Add(key);
							tv.type = OBJECT;
							tv.o = new object;
							o = tv.o;
						}
						else
						{
							var &tv = o->m_tbl.FindOrAdd(f);
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
					{
						int key = ops[pc++];
						o->m_tbl.Set(key, v0);
					}
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
						cexception_error( "bad type" );
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
					for (int i=0; i<entries-1; i++)
					{
						int key = ops[pc++];
						const var &v = FindVar( o, key, state );
						if ( v.type == OBJECT )
						{
							o = v.o;
						}
						else
						{
							cexception_error( "bad type" );
						}
#if 0
						std::map<int,var>::iterator f = o->m_tbl.find(key);
						if ( f == o->m_tbl.end() )
						{
							bool found = false;
							if ( o->prototype != -1 )
							{
								var &pv = state.globals[o->prototype];
								if ( pv.type == OBJECT )
								{
									const object *po = pv.o;
									std::map<int,var>::const_iterator cf = po->m_tbl.find(key);
									if ( cf != po->m_tbl.cend() )
									{
										const var &tv = cf->second;
										if ( tv.type == OBJECT )
										{
											o  = tv.o;
											found = true;
										}
									}
								}
							}
							
							if ( !found )
							{
								var &tv = o->m_tbl[key];
								tv.type = OBJECT;
								tv.o = new object;
								o = tv.o;
							}
						}
						else
						{
							var &tv = f->second;
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
#endif
					}
					{
						int key = ops[pc++];
						int f = o->m_tbl.Find(key);
						bool found = false;
						if ( f == -1 )
						{
							if ( o->prototype != -1 )
							{
								const var &pv = state.globals.Get(o->prototype);
								if ( pv.type == OBJECT )
								{
									const object *po = pv.o;
									Map<var>::Iterator cf = po->m_tbl.Find(key);
									if ( cf )
									{
										state.stack.push( *cf.second );
										found = true;
									}
								}
							}
						}
						if ( !found )
						{
							state.stack.push( o->m_tbl.FindOrAdd(key) );
						}
					}
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
					if ( o->prototype != -1 )
					{
						const var &proto_item = state.globals.Get(o->prototype);
						if ( proto_item.type == OBJECT )
						{
							o = proto_item.o;
							if ( o->prototype != -1 )
							{
								const var &super_proto_item = state.globals.Get(o->prototype);
								if ( super_proto_item.type == OBJECT )
								{
									o = super_proto_item.o;
								}
							}
						}
					}
					for (int i=0; i<entries-1; i++)
					{
						int key = ops[pc++];
						Map<var>::Iterator f = o->m_tbl.Find(key);
						if ( f )
						{
							bool found = false;
							if ( o->prototype != -1 )
							{
								const var &pv = state.globals.Get(o->prototype);
								if ( pv.type == OBJECT )
								{
									const object *po = pv.o;
									Map<var>::Iterator cf = po->m_tbl.Find(key);
									if ( cf )
									{
										const var &tv = *cf.second;
										if ( tv.type == OBJECT )
										{
											o  = tv.o;
											found = true;
										}
									}
								}
							}
							
							if ( !found )
							{
								var &tv = o->m_tbl.FindOrAdd(key);
								tv.type = OBJECT;
								tv.o = new object;
								o = tv.o;
							}
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
					{
						int key = ops[pc++];
						Map<var>::Iterator f = o->m_tbl.Find(key);
						bool found = false;
						if ( f )
						{
							if ( o->prototype != -1 )
							{
								const var &pv = state.globals.Get(o->prototype);
								if ( pv.type == OBJECT )
								{
									const object *po = pv.o;
									Map<var>::Iterator cf = po->m_tbl.Find(key);
									if ( cf )
									{
										const var &tv = *cf.second;
										state.stack.push( tv );
										found = true;
									}
								}
							}
						}
						if ( !found )
						{
							state.stack.push( o->m_tbl.FindOrAdd(key) );
						}
					}
				}
				else
				{
					cexception_error( "eek" );
				}
			}
			break;
		case OPC_POPITEMG:
			{
				var v0 = state.stack.top(); state.stack.pop();

				int entries = ops[pc++]-1;
				int index = ops[pc++];
				var &item = state.globals.Get(index);
				if ( item.type == OBJECT && entries )
				{
					object *o = item.o;
					for (int i=0; i<entries-1; i++)
					{
						int key = ops[pc++];
						Map<var>::Iterator f = o->m_tbl.Find(key);
						if ( f )
						{
							var &tv = o->m_tbl.Add(key);
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
					{
						int key = ops[pc++];
						o->m_tbl.Set(key, v0);
					}
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
						cexception_error( "unknown type" );
					}
				}
			}
			break;
		case OPC_PUSHITEMG:
			{
				int entries = ops[pc++]-1;
				int index = ops[pc++];
				const var &item = state.globals.Get(index);
				if ( item.type == OBJECT && entries )
				{
					object *o = item.o;
					for (int i=0; i<entries-1; i++)
					{
						int key = ops[pc++];
						Map<var>::Iterator f = o->m_tbl.Find(key);
						if ( f )
						{
							bool found = false;
							if ( o->prototype != -1 )
							{
								var &pv = state.globals.Get(o->prototype);
								if ( pv.type == OBJECT )
								{
									const object *po = pv.o;
									Map<var>::Iterator cf = po->m_tbl.Find(key);
									if ( cf )
									{
										const var &tv = *cf.second;
										if ( tv.type == OBJECT )
										{
											o = tv.o;
											found = true;
										}
									}
								}
							}
							
							if ( !found )
							{
								var &tv = o->m_tbl.FindOrAdd(key);
								tv.type = OBJECT;
								tv.o = new object;
								o = tv.o;
							}
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
					{
						int key = ops[pc++];
						Map<var>::Iterator f = o->m_tbl.Find(key);
						bool found = false;
						if ( f )
						{
							if ( o->prototype != -1 )
							{
								const var &pv = state.globals.Get(o->prototype);
								if ( pv.type == OBJECT )
								{
									const object *po = pv.o;
									Map<var>::Iterator cf = po->m_tbl.Find(key);
									if ( cf )
									{
										const var &tv = *cf.second;
										state.stack.push( tv );
										found = true;
									}
								}
							}
						}
						if ( !found )
						{
							state.stack.push( o->m_tbl.FindOrAdd(key) );
						}
					}
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
				var v0 = state.stack.top(); state.stack.pop();
				int lookup = ops[pc++];
				int index = ops[pc++];
				var &v = state.globals.Get(lookup);
				if ( v.type == INTEGERARRAY )
				{
					if ( (int)v.iArrayPtr->m_items.size() <= index )
					{
						v.iArrayPtr->m_items.resize( index+1 );
					}
					v.iArrayPtr->m_items[index] = ToInt( v0 );
				}
				else
				if ( v.type == FLOATINGPOINTARRAY )
				{
					if ( (int)v.fArrayPtr->m_items.size() <= index )
					{
						v.fArrayPtr->m_items.resize( index+1 );
					}
					v.fArrayPtr->m_items[index] = ToFloat( v0 );
				}
				else
				if ( v.type == OBJECTARRAY && v0.type == OBJECT )
				{
					if ( (int)v.oArrayPtr->m_items.size() <= index )
					{
						v.oArrayPtr->m_items.resize( index+1 );
					}
					v.oArrayPtr->m_items[index] = v0.o;
				}
				else
				{
					cexception_error( "bad type" );
				}
			}
			break;
		case OPC_POPITEMARRAY:
			{
				var v0 = state.stack.top(); state.stack.pop();
				int lookup = ops[pc++];
				int index = ops[pc++];
				var &v = state.stack[state.envStack.top()+lookup];
				if ( v.type == INTEGERARRAY )
				{
					if ( (int)v.iArrayPtr->m_items.size() <= index )
					{
						v.iArrayPtr->m_items.resize( index+1 );
					}
					v.iArrayPtr->m_items[index] = ToInt( v0 );
				}
				else
				if ( v.type == FLOATINGPOINTARRAY )
				{
					if ( (int)v.fArrayPtr->m_items.size() <= index )
					{
						v.fArrayPtr->m_items.resize( index+1 );
					}
					v.fArrayPtr->m_items[index] = ToFloat( v0 );
				}
				else
				if ( v.type == OBJECTARRAY && v0.type == OBJECT )
				{
					if ( (int)v.oArrayPtr->m_items.size() <= index )
					{
						v.oArrayPtr->m_items.resize( index+1 );
					}
					v.oArrayPtr->m_items[index] = v0.o;
				}
				else
				{
					assert( 0 );
				}
			}
			break;
		case OPC_PUSHITEMGARRAY:
			{
				int lookup = ops[pc++];
				int index = ops[pc++];
				var &v = state.globals[lookup];
				if ( v.type == INTEGERARRAY )
				{
					if ( (int)v.iArrayPtr->m_items.size() <= index )
					{
						v.iArrayPtr->m_items.resize( index+1 );
					}
					var &v0 = state.stack.push();
					v0.type = INTEGER;
					v0.i = v.iArrayPtr->m_items[index];
				}
				else
				if ( v.type == FLOATINGPOINTARRAY )
				{
					if ( (int)v.fArrayPtr->m_items.size() <= index )
					{
						v.fArrayPtr->m_items.resize( index+1 );
					}
					var &v0 = state.stack.push();
					v0.type = FLOATINGPOINT;
					v0.f = v.fArrayPtr->m_items[index];
				}
				else
				if ( v.type == OBJECTARRAY )
				{
					if ( (int)v.oArrayPtr->m_items.size() <= index )
					{
						v.oArrayPtr->m_items.resize( index+1 );
					}
					var &v0 = state.stack.push();
					v0.type = OBJECT;
					v0.o = v.oArrayPtr->m_items[index];
				}
				else
				{
					assert( 0 );
				}
			}
			break;
		case OPC_PUSHITEMARRAY:
			{
				int lookup = ops[pc++];
				int index = ops[pc++];
				var &v = state.stack[state.envStack.top()+lookup];
				if ( v.type == INTEGERARRAY )
				{
					if ( (int)v.iArrayPtr->m_items.size() <= index )
					{
						v.iArrayPtr->m_items.resize( index+1 );
					}
					var &v0 = state.stack.push();
					v0.type = INTEGER;
					v0.i = v.iArrayPtr->m_items[index];
				}
				else
				if ( v.type == FLOATINGPOINTARRAY )
				{
					if ( (int)v.fArrayPtr->m_items.size() <= index )
					{
						v.fArrayPtr->m_items.resize( index+1 );
					}
					var &v0 = state.stack.push();
					v0.type = FLOATINGPOINT;
					v0.f = v.fArrayPtr->m_items[index];
				}
				else
				if ( v.type == OBJECTARRAY )
				{
					if ( (int)v.oArrayPtr->m_items.size() <= index )
					{
						v.oArrayPtr->m_items.resize( index+1 );
					}
					var &v0 = state.stack.push();
					v0.type = OBJECT;
					v0.o = v.oArrayPtr->m_items[index];
				}
				else
				{
					assert( 0 );
				}
			}
			break;
		default:
			assert( 0 );
			break;
		}
	}
}
