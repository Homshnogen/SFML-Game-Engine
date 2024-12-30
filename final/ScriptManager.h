#pragma once

#define SM_DEBUG 0
#define RELOAD 0

#include <v8.h>
#include <map>
#include <filesystem>

// static keyword here (not in class member) has different semantics...it
// prevents this functions from being included in multiple compilation units. 
static std::string read_script(std::filesystem::path path); 

/**
 * Container for details of scripts. 
 */
struct ScriptMetaData
{
	std::string script_id;
	bool isLocal;
	std::string file_name;
	v8::Local<v8::Script> script;
};

/**
 * Class for handling assignment of javascript scripts to v8 containers, and
 * facilitating execution of those scripts. 
 */
class ScriptManager
{
    private:
		/** map to keeping track of context information */
		v8::Isolate *isolate;
		v8::Local<v8::Context> context;
		std::map<std::string, ScriptMetaData> scripts;

    public:
		/**
		 * v8 is a bit picky about parameter types...make sure to use Isolate*
		 * and Context& when receiving these parameters from where they are
		 * created (i.e., in main() in this example. When forwarding these
		 * variables by making another function call with this class' variable,
		 * use Isolate* and Context (without the &). 
		 */
        ScriptManager(v8::Isolate *isolate, v8::Local<v8::Context> &context);
        ~ScriptManager();

		inline v8::Isolate *getIsolate() {return isolate;};
		inline v8::Local<v8::Context> &getContext() {return context;};
		/** Add the given script to the given context */
        void addLocalScript(std::string script_id, std::string file_name);
		void addStringScript(std::string script_id, std::string scr);

		/**
		 * Run a single script by name in the given context
		 *
		 * NOTE: The script must have been added to that context first.
		 * */
        void runOne(std::string script_id, bool reload=RELOAD);

		/** Run all the scripts that have been exposed in the given context. */
        void runAll(bool reload=RELOAD);

		/**
		 * Build the intermediate representation of the script. 
		 *
		 * IMPORTANT: Please keep in mind that compiling scripts has the effect
		 * of running them, so if a script makes a chance to a native object,
		 * that change will occur both when the script is compiled and when it
		 * is run. 
		 *
		 * You may STRONGLY consider adjusing how scripts are stores (as
		 * strings vs. complied objects) so that you can adjust the timing of
		 * when compliation occurs (i.e., you may want to modify the
		 * ScriptMetaData struct to store both a string of the script as well
		 * as the complied object and only complie if needed or upon reload
		 * request. 
		 */
		v8::Local<v8::Script> compile(std::string script_src_string);

		/** complie all of the scripts in the given context again. 
		 *
		 * IMPORTANT: Please keep in mind that compiling a script has the
		 * effect of running it. See more detailed comment for compile
		 * function.
		 */
		void reloadAll();

		/**
		 * Wrapper function for retrieving a stored handle for the given context.
		 */
		//static void getHandleFromScript(const v8::FunctionCallbackInfo<v8::Value>& args);

};

