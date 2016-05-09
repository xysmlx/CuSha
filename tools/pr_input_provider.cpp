/*
 * pr_input_provider.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: Farzad Khorasani
 */

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <cstdlib>

template< typename idxT>
void PR_maker( std::ifstream& inFile, std::ofstream& outFile, const bool nonDirectedGraph ) {

	/*
	 * Declare variables and functions required.
	 */

	std::unordered_map<idxT, idxT> mapper;
	std::string line;

	auto addEntryF = [&]( idxT key ) {
		auto pos_index = mapper.find( key );
		if( pos_index == mapper.end() ) {	// The key does not exist. We need to add it.
			std::pair<idxT, idxT> kvp( key, 1 );
			mapper.insert( kvp );
		}
		else	// The key does exist.
			++( pos_index->second );
	};

	auto readLineIndices = [&]( idxT& firstIdx, idxT& secondIdx ) {
		std::size_t pos = 2015;
		firstIdx = static_cast< idxT >( std::stoul( line, &pos ) );
		std::string sub_line;
		sub_line = line.substr( ++pos );
		secondIdx = static_cast< idxT >( std::stoul( sub_line, &pos ) );
	};

	auto entryToFileInserter = [&]( idxT lIdx, idxT rIdx ) {
		auto pos_index = mapper.find( lIdx );
		outFile << lIdx << "\t" << rIdx << "\t" << ( ( pos_index != mapper.end() ) ? ( pos_index->second ) : 0 ) << "\n";
	};

	/*
	 * Start the work.
	 */

	// Read the input graph line-by-line to collect data.
	while( std::getline( inFile, line ) ) {
		if( line[0] < '0' || line[0] > '9' )	// Skipping any line blank or starting with a character rather than a number.
			continue;
		idxT firstIndex, secondIndex;
		readLineIndices( firstIndex, secondIndex );
		addEntryF( secondIndex );
		if( nonDirectedGraph ) addEntryF( firstIndex );
	}

	// Restart the file pointer to its beginning.
	inFile.clear();
	inFile.seekg(0, std::ios::beg);

	// Read the input graph line-by-line again and insert the third-column for each entry to the output file.
	while( std::getline( inFile, line ) ) {
		if( line[0] < '0' || line[0] > '9' ) {	// Skipping any line blank or starting with a character rather than a number.
			outFile << line << "\n";
			continue;
		}
		idxT firstIndex, secondIndex;
		readLineIndices( firstIndex, secondIndex );
		entryToFileInserter( firstIndex, secondIndex );
//		if( nonDirectedGraph ) entryToFileInserter( secondIndex, firstIndex );
	}

}

// Opening files safely.
template <typename T_file>
void openFileToAccess( T_file& inputF, const char* fileName ) {
	inputF.open( fileName );
	if( !inputF )
		throw std::runtime_error( "Failed to open specified file." );
}

int main ( int argc, char ** argv )
{

	const std::string usage =
	"\tRequired command line arguments:\n\
		-Input file: E.g., --input in.txt\n\
	Additional arguments:\n\
		-Output file (default: out.txt). E.g., --output myout.txt\n\
		-Is the input graph directed (default:yes). To specify it as undirected: --undirected\n";

	std::ofstream outFile;
	std::ifstream inFile;
	bool nonDirectedGraph = false;		// By default, the graph is directed.

	try{

		// Getting required input parameters.
		for( int iii = 1; iii < argc; ++iii )
			if( !strcmp( argv[iii], "--input" ) && iii != argc-1 /*is not the last one*/)
				openFileToAccess< std::ifstream >( inFile, argv[iii+1] );
			else if( !strcmp( argv[iii], "--output" ) && iii != argc-1 /*is not the last one*/)
				openFileToAccess< std::ofstream >( outFile, argv[iii+1] );
			else if( !strcmp(argv[iii], "--undirected"))
				nonDirectedGraph = true;

		if( !inFile.is_open() )
			throw std::runtime_error( "The input file has not been specified." );
		if( !outFile.is_open() )
			openFileToAccess< std::ofstream >( outFile, "out.txt" );

		// Start the work.
		std::cout << "Starting the work ..." << std::endl;
		using vertexIndexType = unsigned int;
		PR_maker<vertexIndexType>( inFile, outFile, nonDirectedGraph );
		std::cout << "Done.\n";

	}
	catch( const std::exception& strException ) {
		std::cerr << "Initialization Error: " << strException.what() << "\n";
		std::cerr << "Usage: " << usage << std::endl << "Exiting." << std::endl;
		return( EXIT_FAILURE );
	}
	catch(...) {
		std::cerr << "An exception has occurred." << std::endl;
		return( EXIT_FAILURE );
	}
	return( EXIT_SUCCESS );
}

