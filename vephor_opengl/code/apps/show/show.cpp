#include "vephor.h"
#include "vephor_ogl.h"
#include <filesystem>
#include <unistd.h>
namespace fs = std::filesystem;

using namespace vephor;

#include "show_record.h"

void usage(char* argv[])
{
	v4print "Usage:", argv[0], "\n";
	v4print "\t-i <input dir>";
	v4print "\t-m <mode>";
	v4print "\t-o <host>";
	v4print "\t-p <port>";
}

int main(int argc, char* argv[])
{
	string input_dir;
	string mode;
	string host = "localhost";
	int port = VEPHOR_DEFAULT_PORT;
	bool redirect = false;
	string record_path;
	float playback_speed = 1;
	
	int opt;
	int pos_arg_ind = 0;
	while (optind < argc)
	{
		v4print "optind:", optind;
		if((opt = getopt(argc, argv, "hi:m:o:p:P:rR:")) != -1) 
		{
			switch(opt)
			{
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
			std::exit(1);
		}
		show.setupFromPath(input_dir);
    }
    
	show.spin(use_server || use_client);

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