/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor.h"
#include "vephor_ogl.h"
#include <filesystem>
#include <unistd.h>
namespace fs = std::filesystem;

using namespace vephor;
using namespace vephor::ogl;

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
	bool debug = false;
	bool profile = false;
	string screenshot_path;
	
	int opt;
	int pos_arg_ind = 0;
	while (optind < argc)
	{
		v4print "optind:", optind;
		if((opt = getopt(argc, argv, "dDFhi:m:o:p:P:rR:S:v:")) != -1) 
		{
			switch(opt)
			{
			case 'd':
				daemonize = true;
				break;
			case 'D':
				debug = true;
				break;
			case 'F':
				profile = true;
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
			case 'S': 
				screenshot_path = optarg;
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
	else if (mode.empty())
		mode = "client";
	
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

	if (debug)
		Window::setDebug(true);

	ShowRecord show;
	show.record_path = record_path;
	show.video_path = video_path;
	show.playback_speed = playback_speed;

	if (!screenshot_path.empty())
	{
		show.exit_counter = 2;
		show.keep_windows_hidden = false;
	}

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
    
	show.spin(use_server || daemonize, debug, profile);

	if (!screenshot_path.empty())
	{
		std::filesystem::create_directory(screenshot_path);
		show.saveScreenshots(screenshot_path);
	}

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