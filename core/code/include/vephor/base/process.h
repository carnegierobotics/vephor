/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

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