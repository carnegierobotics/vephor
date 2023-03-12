#pragma once

#include "thirdparty/sheredom/subprocess.h"

namespace vephor
{

class Process
{
public:
	Process(const vector<string>& command_line)
	{
		vector<const char*> command_line_raw;
		command_line_raw.reserve(command_line.size()+1);
		for (const auto& c : command_line)
		{
			command_line_raw.push_back(c.c_str());
		}
		command_line_raw.push_back(NULL);
		int result = subprocess_create(command_line_raw.data(), 
			subprocess_option_inherit_environment | subprocess_option_search_user_path/*| subprocess_option_enable_async*/, 
			&proc);
		if (0 != result) {
			throw std::runtime_error("Could not start process.");
		}
		
		proc_stdout = subprocess_stdout(&proc);
		proc_stderr = subprocess_stderr(&proc);
	}
	bool isAlive()
	{
		return subprocess_alive(&proc);
	}
	void printOutput()
	{
		char buf[1024];
		
		v4print "Proc stdout";
		v4print "===========";
		
		while (true)
		{
			auto result = fgets(buf, 1024, proc_stdout);
			if (result)
			{
				std::cout << buf;
			}
			else
				break;
		}
		
		v4print "";
		v4print "Proc stderr";
		v4print "===========";
		
		while (true)
		{
			auto result = fgets(buf, 1024, proc_stderr);
			if (result)
			{
				std::cout << buf;
			}
			else
				break;
		}
	}
	int join()
	{
		int process_return;
		int result = subprocess_join(&proc, &process_return);
		if (0 != result) {
			throw std::runtime_error("Could not join process.");
		}
		return process_return;
	}
	void terminate()
	{
		subprocess_terminate(&proc);
	}
private:
	subprocess_s proc;
	FILE* proc_stdout;
	FILE* proc_stderr;
};

}