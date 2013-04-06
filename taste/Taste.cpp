#include "Stdio.h"

#include "Parser.h"
#include "Scanner.h"
#include <sys/timeb.h>
#include <wchar.h>

using namespace Taste;

int main (int argc, char *argv[]) {

	if (argc == 2) {
		wchar_t *fileName = coco_string_create(argv[1]);
		Taste::Scanner *scanner = new Taste::Scanner(fileName);
		Taste::Parser *parser = new Taste::Parser(scanner);
		parser->InitDeclarations();
		parser->Parse();
		if (parser->errors->count == 0) {
			parser->package->Generate();
		}

		coco_string_delete(fileName);
		delete parser;
		delete scanner;
	} else
		printf("-- No source file specified\n");

	return 0;

}
