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

#include "vephor.h"
#include "vephor_ogl.h"
#include <filesystem>
#include <unistd.h>
namespace fs = std::filesystem;

using namespace vephor;

#include "show_record.h"

void usage(char* argv[])
{
	v4print "Usage:", argv[0];
	v4print "\t-d                  - daemonizes a client, allowing it to survive window close";
	v4print "\t-h                  - shows usage";
	v4print "\t-i <input dir>      - specifies path to saved viz";
	v4print "\t-m <mode>           - sets show mode, can be server or client";
	v4print "\t-o <host>           - sets host for client to connect to";
	v4print "\t-p <port>           - sets port to use for client or server";
	v4print "\t-P <playback speed> - sets playback speed as a multiplier, with 1 being normal";
	v4print "\t-r                  - redirects console output to a temporary dir";
	v4print "\t-R <record dir>     - sets dir to record inputs into for later playback";
}

int main(int argc, char* argv[])
{
	string input_dir;
	string mode;
	string host = "localhost";
	int port = VEPHOR_DEFAULT_PORT;
	bool redirect = false;
	string record_path;
	string video_path;
	float playback_speed = 1;
	bool daemonize = false;
	
	int opt;
	int pos_arg_ind = 0;
	while (optind < argc)
	{
		v4print "optind:", optind;
		if((opt = getopt(argc, argv, "dhi:m:o:p:P:rR:v:")) != -1) 
		{
			switch(opt)
			{
			case 'd':
				daemonize = true;
				break;
			case 'h':
				usage(argv);
				std::exit(0);
				break;
			case 'i': 
				input_dir = optarg;
				break;
			case 'm': 
				mode = optarg;
				break;
			case 'o': 
				host = optarg;
				break;
			case 'p': 
				port = std::atoi(optarg);
				break;
			case 'P': 
				playback_speed = std::atof(optarg);
				break;
			case 'r':
				redirect = true;
				break;
			case 'R': 
				record_path = optarg;
				break;
			case 'v': 
				video_path = optarg;
				break;
			default:
				usage(argv);
				std::exit(1);
				break;
			}			
		}
		else {
			if (pos_arg_ind == 0) // Input dir
			{
				input_dir = argv[optind];
			}
			else
			{
				v4print "Too many positional arguments.";
				usage(argv);
				std::exit(1);
			}
			optind++;  // Skip to the next argument
			pos_arg_ind++;
		}
	}

	if (optind < argc)
		input_dir = argv[optind];
	
	if (!input_dir.empty())
	{
		input_dir = fs::absolute(input_dir).string();
	}
	
	if (!record_path.empty())
	{
		record_path = fs::absolute(record_path).string();
		if (fs::exists(record_path))
			throw std::runtime_error("Record path already exists.");
		fs::create_directory(record_path);
	}
	
	string temp_dir = getTempDir();
	string orig_dir = fs::current_path().string();
	fs::current_path(temp_dir);
	
	FILE* stdout_fp = NULL;
	FILE* stderr_fp = NULL;
	if (redirect)
	{
		stdout_fp = freopen("stdout.txt", "w", stdout);
		stderr_fp = freopen("stderr.txt", "w", stderr);
	}
	
	bool use_client = mode == string("client");
	bool use_server = mode == string("server");
	bool use_server_no_daemon = mode == string("server_no_daemon");
	bool use_net = use_client || use_server || use_server_no_daemon;

	ShowRecord show;
	show.record_path = record_path;
	show.video_path = video_path;
	show.playback_speed = playback_speed;

    if (use_net)
    {
		if (use_client)
			show.connectClient(host, port);
		else
			show.connectServer(port);
    }
    else
    {
		if (input_dir.empty())
		{
			v4print "Input dir empty.";
			usage(argv);
			std::exit(1);
		}
		show.setupFromPath(input_dir);
    }
    
	show.spin(use_server || daemonize);

	v4print "Show: Exiting successfully.";
	
	if (redirect)
	{
		fclose(stdout_fp);
		fclose(stderr_fp);
	}
	
	fs::current_path(orig_dir);
	
	fs::remove_all(temp_dir);

    return 0;
}