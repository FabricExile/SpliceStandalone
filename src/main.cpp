#include "SpliceStandalone.h"
#include "SpliceGraphWrapper.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/filesystem.hpp>

#if __linux
# include <X11/Xlib.h>
#endif

using namespace FabricSplice;

int main(int argc, char *argv[])
{
#if __linux
  XInitThreads();
#endif

  std::string spliceFilePath;

  po::options_description desc("Splice Standalone");
  desc.add_options()
   ("help,h", "produce help message")
   ("spliceFile,f", po::value<std::string>(&spliceFilePath)->required(), "filePath of the splice file to execute");

  po::positional_options_description p;
  p.add("spliceFile", 1);

  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(argc, argv, desc, po::command_line_style::unix_style ^ po::command_line_style::allow_short), vm);
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  }
  catch( std::exception &e )
  {
    std::cout << desc << std::endl;
    return false;
  }
  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    return EXIT_SUCCESS;
  }
  try
  {
    po::notify(vm);
  }
  catch( std::exception &e )
  {
    std::cout << desc << std::endl;
    return false;
  }

  const char * FABRIC_DIR = getenv("FABRIC_DIR");
  if(!FABRIC_DIR)
  {
    std::cout << "The environment variable FABRIC_DIR is not specified.\nIt should point to the Fabric Engine installation folder." << std::endl;
    return false;
  }

  boost::filesystem::path appDir = FABRIC_DIR;

  FILE * f = fopen(spliceFilePath.c_str(), "rb");
  if(!f)
  {
    std::cout << "Splice File " << spliceFilePath << "does not exist." << std::endl;
    return false;
  }
  fclose(f);

	SpliceStandalone app(argc, argv, appDir, spliceFilePath);

	// Blocks until application decides to quit.
	app.exec();
	
	return EXIT_SUCCESS;
}

