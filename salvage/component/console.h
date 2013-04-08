#ifndef TESTING_CONSOLE_H
#define TESTING_CONSOLE_H

#include <string>
#include <vector>

class Font;
class Material;

typedef void (*console_function)(const std::vector<std::string> &);

struct console_item_t
{
	std::string name;
	console_function function;
};

class Console
{

	const Material *background;
	const Font *font;
	std::vector< std::string > history;
	std::vector< console_item_t > consoleFunctions;
	std::string commandLine;
	bool enabled;

	void parseCommandLine();
	void autoComplete();

public:

	Console();
	~Console();

	bool Toggle();
	void KeyPress( char c );
	bool IsEnabled();

	int	 AddRenderCommands( struct RenderCommand *cmd );

	void RegisterFuntion( const char *name, console_function f );

};

#endif
