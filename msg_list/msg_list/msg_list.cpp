// msg_list.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>

void err( std::string s)
{
	std::cout << "Error: " << s << std::endl;
	exit( -1);
}

std::string replaceAll( std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while( ( start_pos = str.find( from, start_pos)) != std::string::npos) {
        str.replace( start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void r() {

}

int _tmain( int argc, _TCHAR* argv[])
{
	/* @peterg variables */
	std::string inputFileName;
	std::string outputFileName;
	int tenantCount;

	std::cout << "Please specify input file name:";
	std::cin >> inputFileName;
	if ( inputFileName.empty()) err( "You didn't specify input file name. Terminating now");

	/* @peterg open file with mesage list sceleton */
	std::ifstream msgTemplate;
	msgTemplate.open( inputFileName.c_str());
	if ( !msgTemplate.is_open()) err( "Cannot open input file");

	std::cout << "Please specify output file name:";
	std::cin >> outputFileName;
	if ( outputFileName.empty()) err( "You didn't specify output file name. Terminating now");

	std::ofstream ofs;
	ofs.open( outputFileName.c_str(), std::ios_base::trunc);
	if ( !ofs.is_open()) err( "Cannot open output file");

	/* @peterg fill in vector message template list*/
	std::vector<std::string> vMsgTplList;
	std::copy( std::istream_iterator<std::string>( msgTemplate),
					std::istream_iterator<std::string>(), std::back_inserter( vMsgTplList));
	msgTemplate.close();
	vMsgTplList.erase( std::remove_if( vMsgTplList.begin(), vMsgTplList.end(),
						[]( std::string const& s) { return s.empty(); }), vMsgTplList.end());
	std::cout << "Input file lines count:" << vMsgTplList.size() << std::endl;

	std::cout << "number of tenants:";
	std::cin >> tenantCount;
	if ( tenantCount < 1) err( "Number of tenants has to be greater than 0");

	/* @peterg create messages */
	//std::vector<std::string> out;
	//out.reserve( tenantCount * vMsgTplList.size());
	int id = 0;
	while ( id < tenantCount)
	{
		std::string tenantStr = "TD" + std::to_string( id);
		std::for_each( vMsgTplList.begin(), vMsgTplList.end(), [ tenantStr, &ofs]( std::string s)
		{
			std::string::size_type f = s.find( "TD");
			if ( f == std::string::npos) return;
			std::string::size_type end = s.find( '\\', f);

			if ( end == std::string::npos)
			{
				/* @peterg there is only one occurence, replace this */
				s.replace( f, s.size() - f, tenantStr);
			} else {
			    /* @peterg replace all occurences */
			    std::string toReplace = s.substr( f, end - f);
			    s = replaceAll( s, toReplace, tenantStr);
			}

			//out.push_back( s);
			ofs << s << std::endl;
		});
	}

	ofs.close();
	std::cout << "Done" << std::endl;
	std::cout << "Saved file " << outputFileName << " into working dir. File lines count " << tenantCount * vMsgTplList.size() << "." << std::endl;

	return 0;
}

