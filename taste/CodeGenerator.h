#if !defined(TASTE_CODEGENERATOR_H__)
#define TASTE_CODEGENERATOR_H__

#include "Scanner.h"
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

namespace Taste {

class CodeGenerator
{
public:
	// opcodes
	int
	  ADD,  SUB,   MUL,   DIV,   EQU,  LSS, GTR, NEG,
	  LOAD, LOADG, STO,   STOG,  CONST,
	  CALL, RET,   ENTER, LEAVE,
	  JMP,  FJMP,
	  CONSTF, ADDF, SUBF, MULF, DIVF, NEGF,
	  GETTABLE, ADDO, SUBO, MULO, DIVO, NEGO, LOADOBJECT, STOOBJECT,
	  INITINT, INITFP, INITOBJ;


	wchar_t* opcode[36];

	int progStart;		// address of first instruction of main program
	int pc;				// program counter
	char *code;

	// data for Interpret
	int *globals;
	int *stack;
	int top;	// top of stack
	int bp;		// base pointer


	CodeGenerator() {
		// opcodes
		ADD  =  0; SUB   =  1; MUL   =  2; DIV   =  3; EQU   =  4; LSS = 5; GTR = 6; NEG = 7;
		LOAD =  8; LOADG =  9; STO   = 10; STOG  = 11; CONST = 12;
		CALL = 13; RET   = 14; ENTER = 15; LEAVE = 16;
		JMP  = 17; FJMP  = 18;
		CONSTF = 19; ADDF = 20; SUBF = 21; DIVF  = 22; MULF  = 23; NEGF = 24;
		GETTABLE = 25; ADDO = 26; SUBO = 27; DIVO  = 28; MULO  = 29; NEGO = 30; LOADOBJECT = 31; STOOBJECT = 32;
		INITINT = 33; INITFP = 34; INITOBJ = 35;

		opcode[ 0] = coco_string_create("ADD  ");
		opcode[ 1] = coco_string_create("SUB  ");
		opcode[ 2] = coco_string_create("MUL  ");
		opcode[ 3] = coco_string_create("DIV  ");
		opcode[ 4] = coco_string_create("EQU  ");
		opcode[ 5] = coco_string_create("LSS  ");
		opcode[ 6] = coco_string_create("GTR  ");
		opcode[ 7] = coco_string_create("NEG  ");
		opcode[ 8] = coco_string_create("LOAD ");
		opcode[ 9] = coco_string_create("LOADG");
		opcode[10] = coco_string_create("STO  ");
		opcode[11] = coco_string_create("STOG ");
		opcode[12] = coco_string_create("CONST");
		opcode[13] = coco_string_create("CALL ");
		opcode[14] = coco_string_create("RET  ");
		opcode[15] = coco_string_create("ENTER");
		opcode[16] = coco_string_create("LEAVE");
		opcode[17] = coco_string_create("JMP  ");
		opcode[18] = coco_string_create("FJMP ");
		opcode[19] = coco_string_create("CNSTF");
		opcode[20] = coco_string_create("ADDF ");
		opcode[21] = coco_string_create("SUBF ");
		opcode[22] = coco_string_create("MULF ");
		opcode[23] = coco_string_create("DIVF ");
		opcode[24] = coco_string_create("NEGF ");
		opcode[25] = coco_string_create("GTTBL");
		opcode[26] = coco_string_create("ADDO ");
		opcode[27] = coco_string_create("SUBO ");
		opcode[28] = coco_string_create("MULO ");
		opcode[29] = coco_string_create("DIVO ");
		opcode[30] = coco_string_create("NEGO ");
		opcode[31] = coco_string_create("LOBJ ");
		opcode[32] = coco_string_create("SOBJ ");
		opcode[33] = coco_string_create("IINT ");
		opcode[34] = coco_string_create("IFP  ");
		opcode[35] = coco_string_create("IOBJ ");

		code    = new char[3000];
		globals = new int[100];
		stack   = new int[100];

		progStart = 0;

		pc = 1;
	}

	~CodeGenerator() {
		coco_string_delete(opcode[ 0]);
		coco_string_delete(opcode[ 1]);
		coco_string_delete(opcode[ 2]);
		coco_string_delete(opcode[ 3]);
		coco_string_delete(opcode[ 4]);
		coco_string_delete(opcode[ 5]);
		coco_string_delete(opcode[ 6]);
		coco_string_delete(opcode[ 7]);
		coco_string_delete(opcode[ 8]);
		coco_string_delete(opcode[ 9]);
		coco_string_delete(opcode[10]);
		coco_string_delete(opcode[11]);
		coco_string_delete(opcode[12]);
		coco_string_delete(opcode[13]);
		coco_string_delete(opcode[14]);
		coco_string_delete(opcode[15]);
		coco_string_delete(opcode[16]);
		coco_string_delete(opcode[17]);
		coco_string_delete(opcode[18]);
		coco_string_delete(opcode[19]);
		coco_string_delete(opcode[20]);
		coco_string_delete(opcode[21]);
		coco_string_delete(opcode[22]);
		coco_string_delete(opcode[23]);
		coco_string_delete(opcode[24]);
		coco_string_delete(opcode[25]);
		coco_string_delete(opcode[26]);
		coco_string_delete(opcode[27]);
		coco_string_delete(opcode[28]);
		coco_string_delete(opcode[29]);
		coco_string_delete(opcode[30]);
		coco_string_delete(opcode[31]);
		coco_string_delete(opcode[32]);
		coco_string_delete(opcode[33]);
		coco_string_delete(opcode[34]);
		coco_string_delete(opcode[35]);
	}

	//----- code generation methods -----

	void Emit (int op) {
		code[pc++] = (char)op;
	}

	void Emit (int op, int val) {
		Emit(op); Emit(val>>8); Emit(val);
	}

	void Emit4(int op, int val) {
		Emit(op); Emit(val>>24); Emit(val>>16); Emit(val>>8); Emit(val);
	}

	void Emit (int op, float val) {
		unsigned char c[4];
		memcpy( c, &val, 4 );
		Emit(op); Emit(c[0]); Emit(c[1]); Emit(c[2]); Emit(c[3]);
	}

	void Patch (int adr, int val) {
		code[adr] = (char)(val>>8); code[adr+1] = (char)val;
	}

	void Decode() {
		int maxPc = pc;
		pc = 1;
		while (pc < maxPc) {
			int code = Next();
			printf("%3d: %S ", pc-1, opcode[code]);
			if (code == LOAD || code == LOADG || code == STO || code == STOG ||
				code == CALL || code == ENTER || code == JMP   || code == FJMP ||
				code == INITINT || code == INITFP || code == INITOBJ )
					printf("%d\n", Next2());
			else
			if (code == ADD  || code == SUB || code == MUL || code == DIV || code == NEG ||
				code == EQU  || code == LSS || code == GTR || 
				code == RET || code == LEAVE ||
				code == ADDF  || code == SUBF || code == MULF  || code == DIVF || code == NEGF ||
				code == ADDO  || code == SUBO || code == MULO  || code == DIVO || code == NEGO || code == LOADOBJECT || code == STOOBJECT
				)
					printf("\n");
			else
			if (code == CONST || code == GETTABLE)
					printf("%d\n", Next4() );
			else
			if (code == CONSTF )
					printf("%f\n", NextFloat() );
			
		}
	}

	//----- interpreter methods -----

	int Next () {
		return code[pc++];
	}

	int Next2 () {
		int x,y;
		x = (char)code[pc++]; y = (unsigned char)code[pc++];
		return (x << 8) + y;
	}

	int Next4 () {
		int x,y,z,w;
		x = (char)code[pc++]; y = code[pc++];
		z = (char)code[pc++]; w = code[pc++];
		return (x << 24) + (y << 16) + (z << 8) + w;
	}

	float NextFloat() {
		float f;
		memcpy( &f, &code[pc], 4 );
		pc+=4;
		return f;
	}

	int Int (bool b) {
		if (b) return 1; else return 0;
	}

	void Push (int val) {
		stack[top++] = val;
	}

	int Pop() {
		return stack[--top];
	}

	float PopF() {
		return asfloat( Pop() );
	}

	inline int asint( float f )
	{
		int v;
		memcpy( &v, &f, 4 );
		return v;
	}

	inline float asfloat( int v )
	{
		float f;
		memcpy( &f, &v, 4 );
		return f;
	}

	void Interpret (char* data) {
		int val;
		float valf;
		FILE* s;
		if ((s = fopen(data, "r")) == NULL) {
			printf("--- Error accessing file %s\n", (char*)data);
			exit(1);
		}
		printf("\n");
		pc = progStart; stack[0] = 0; top = 1; bp = 0;
		for (;;) {
			int nxt = Next();
			if (nxt == CONST)
				Push(Next4());
			if (nxt == CONSTF)
				Push(asint(NextFloat()));
			else if (nxt == LOAD)
				Push(stack[bp+Next2()]);
			else if (nxt == LOADG)
				Push(globals[Next2()]);
			else if (nxt == STO)
				stack[bp+Next2()] = Pop();
			else if (nxt == STOG)
				globals[Next2()] = Pop();
			else if (nxt == ADD)
				Push(Pop()+Pop());
			else if (nxt == SUB)
				Push(-Pop()+Pop());
			else if (nxt == DIV)
				{val = Pop(); Push(Pop()/val);}
			else if (nxt == MUL)
				Push(Pop()*Pop());
			else if (nxt == NEG)
				Push(-Pop());
			else if (nxt == ADDF)
				Push(PopF()+PopF());
			else if (nxt == SUB)
				Push(-PopF()+PopF());
			else if (nxt == DIVF)
				{valf = PopF(); Push(PopF()/valf);}
			else if (nxt == MULF)
				Push(PopF()*PopF());
			else if (nxt == NEGF)
				Push(-PopF());
			else if (nxt == EQU)
				Push(Int(Pop()==Pop()));
			else if (nxt == LSS)
				Push(Int(Pop()>Pop()));
			else if (nxt == GTR)
				Push(Int(Pop()<Pop()));
			else if (nxt == JMP)
				pc = Next2();
			else if (nxt == FJMP)
				{ val = Next2(); if (Pop()==0) pc = val;}
			else if (nxt == CALL)
				{Push(pc+2); pc = Next2();}
			else if (nxt == RET)
				{pc = Pop(); if (pc == 0) return;}
			else if (nxt == ENTER)
				{Push(bp); bp = top; top = top + Next2();}
			else if (nxt == LEAVE)
			{int args = Next2(); top = bp; bp = Pop(); }
			else {
				printf("illegal opcode\n");
				exit(1);
			}
		}
	}

};

}; // namespace

#endif // !defined(TASTE_CODEGENERATOR_H__)
