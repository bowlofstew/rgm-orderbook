#include <stdio.h>
#include <fstream>
#include <cstring>
#include <string>
#include <iomanip>
#include <iostream>

#include "FeedHandler.hpp"

using namespace RgmInterview::OrderBook;

int main ( int argc, char **argv )
{
	try
	{
		char foo[250];
		std::cout.precision ( 8 );
		if ( argc < 2 )
		{
			std::cerr << "Have to supply a valid target-size" << std::endl;
			return 1; // failure
		}
		const std::string sz ( argv[1] );
		FeedHandler feed ( atoi ( sz.c_str() ) );
		while ( fgets(foo,250,stdin) )
		{
			foo [ strlen(foo) -1 ] = '\0';
			std::string line ( foo );
			feed.processMessage ( line, std::cout );
		}
		if ( !feed.errors().empty() )
			feed.printErrorSummary ( std::cout );
		return feed.errors().empty();
	}
	catch ( std::exception & ex )
	{
		std::cout << "Exception caught: " << ex.what() << std::endl;
		return 1;
	}
}
