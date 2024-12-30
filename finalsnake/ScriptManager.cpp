#include "ScriptManager.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include "v8helpers.h"


std::string read_script(std::filesystem::path path) // non-member helper (move to helpers?)
{
    std::ifstream in_file(path, std::ios::in);
    const auto script_size = std::filesystem::file_size(path);

    std::string script(script_size, '\0');
    in_file.read(script.data(), script_size);

    return script;
}

/** Note: function signature is very important */
ScriptManager::ScriptManager(v8::Isolate *i, v8::Local<v8::Context> &c) : isolate(i), context(c), scripts()
{
}

// TODO: Free scripts
ScriptManager::~ScriptManager()
{
    /*
    this->isolate->Dispose();
    delete this->create_params.array_buffer_allocator;
    */
}

void ScriptManager::addLocalScript(std::string script_id, std::string file_name)
{
	ScriptMetaData smd;
	smd.script_id = script_id;
	smd.isLocal = true;
	smd.file_name = file_name;
	smd.script = compile(read_script(file_name));
	scripts.emplace(std::make_pair(script_id, smd));
#if SM_DEBUG
	std::cout << "Added script:\n"
			  << "\tScript_ID: " << smd.script_id
			  << "\tFile_Name: " << smd.file_name
			  << std::endl;
#endif
}
void ScriptManager::addStringScript(std::string script_id, std::string scr)
{
	ScriptMetaData smd;
	smd.script_id = script_id;
	smd.isLocal = false;
	//smd.file_name = "";
	smd.script = compile(scr);
	scripts.emplace(std::make_pair(script_id, smd));
#if SM_DEBUG
	std::cout << "Added script:\n"
			  << "\tScript_ID: " << smd.script_id
			  << "\tFile_Name: " << smd.file_name
			  << std::endl;
#endif
}

void ScriptManager::runOne(std::string script_id, bool reload)
{
    // Note: You must enter the context prior to compiling the script
    v8::Context::Scope context_scope(context);

	ScriptMetaData &smd = scripts[script_id];

	v8::TryCatch try_catch(isolate);

	if(reload && smd.isLocal) // file_name is a local file
	{
		// kinda doesn't work, only lasts for local context
		smd.script = compile(read_script(smd.file_name));
	}

	/**
	 * This is how you do error checking in a script.
	 */
	v8::Local<v8::Value> result;
	if(!smd.script->Run(context).ToLocal(&result))
	{
		v8::String::Utf8Value error(isolate, try_catch.Exception());
		std::cerr << "**********************************************************" << std::endl;
		std::cerr << "Error while executing script: " << script_id << std::endl;
		std::cerr << "Exception: " << *error << std::endl;
		std::cerr << "**********************************************************" << std::endl;
		return;
	}

    v8::String::Utf8Value utf8(isolate, result);
    if(strcmp(*utf8, "undefined") != 0)
        printf("%s\n", *utf8);
}

void ScriptManager::runAll(bool reload)
{
	for( std::pair<std::string, ScriptMetaData> i : scripts )
	{
		runOne(i.second.script_id, reload);
	}
}

void ScriptManager::reloadAll()
{
	for( std::pair<std::string, ScriptMetaData> p : scripts )
	{
		p.second.script = compile(p.second.file_name);
	}
}

v8::Local<v8::Script> ScriptManager::compile(std::string script_src_string)
{
    v8::Context::Scope context_scope(context);

    v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, script_src_string.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
    v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
	return script;
}

/**
 * This function demonstrates how you can get an object by using the string
 * associated with the object when added to the Global proxy. Note, despite the
 * return type of Global()->Get(...) being v8::Local<v8::Value>, this is indeed
 * a v8::Local<v8::Object>.
 */
/* I don't really like this implementation, it relies on the handle already existing in global scope
void ScriptManager::getHandleFromScript(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate *isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::Local<v8::Value> object = context->Global()->Get(context, args[0]->ToString()).ToLocalChecked();
	args.GetReturnValue().Set(object);
}
*/

