#include <string>
#include <iostream>
#include <vector>
#include "main_interface.h"
using std::string;
using std::vector;
FILE* debug_out;

void test_compute_state_cost();
void test_back_predict();
void test_rule_parse();
void test_c_interface();
bool test_PNGraph();
bool test_doc_parser();
void test_rule_factory();


int test_case()
{
	// test_compute_state_cost();
	// test_back_predict();
	// test_PNGraph();
	test_doc_parser();

	return 0;
}

#ifndef _STATICLIB
#ifndef _TEST_
#include <boost/program_options.hpp>
namespace po = boost::program_options;
int main(int argc, char** argv)
{
	debug_out = fopen("h:/heimdallr_debug.txt", "w");
	printf("PCFG debug program\n");


	try{
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "help message")
			("subcommand", "sub-command")
			("infile", "input file path");
		po::positional_options_description p;
		p.add("subcommand", 1);
		p.add("infile", 1);

		po::variables_map vm;
		
		auto parser = po::command_line_parser(argc, argv).
			options(desc).positional(p).run();
		po::store(parser, vm);
		po::notify(vm);

		if (vm.count("help")){
			std::cout << desc << "\n";
			return 1;
		} else if (vm.count("subcommand")) {
			const string& scmd =  vm["subcommand"].as<string>();

			if (scmd == "parse") {
				if (vm.count("infile")) {
					parse_rule_file(vm["infile"].as<string>());
				} else {
					printf("Missing input file\n");
				}
			} else if (scmd == "test") {
				test_case();
				// dep_rule_parse();
				// test_rule_parse();
				// test_c_interface();
				// test_rule_factory();
			}
		}

	} catch (std::exception& e){
		printf("error: %s\n", e.what());
	}

	
	// test_case();
	// dep_rule_parse();
	test_rule_parse();
	// test_c_interface();

	// doc_parse();
	// doc_parse_prop();
	// test_case();

	fflush(debug_out);
	fclose(debug_out);
	getchar();
}
#endif // _TEST
#endif // _STATICLIB


