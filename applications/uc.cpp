#include <boost/program_options.hpp>
#define UNITCONVERT_NO_BACKWARD_COMPATIBLE_NAMESPACE
#include <UnitConvert.hpp>
#include <UnitConvert/GlobalUnitRegistry.hpp>

#include <iostream>
#include <fstream>

namespace po = boost::program_options;
using namespace boost::units;

int main(int argc, const char** argv)
{

  // define command line options
  po::options_description po_opts("Options");
  po_opts.add_options()("help,h", "print help message.");

  // now define our arguments.
  po::options_description po_args("Arguments");
  po_args.add_options()
    ("from-quantity"  , po::value<std::string>(), "Quantity to convert.")
    ("to-unit"  , po::value<std::string>(), "Unit to convert to.")
    ("value-only"  , "Only print value, not unit string.")
    ;

  // combine the options and arguments into one option list.
  // this is what we will use to parse the command line, but
  // when we output a description of the options, we will just use
  // po_opts
  po::options_description all_options("Options and Arguments");
  all_options.add(po_opts).add(po_args);

  // tell boost how to translate positional options to named options
  po::positional_options_description args;
  args.add("config-files", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
      .options(all_options)
      .positional(args)
      .run(),
      vm);
  po::notify(vm);

  if (argc == 1 || vm.count("help")) {
    std::cout << R"EOL(A command line application for doing unit conversions.
)EOL";
    return 0;
  }


  auto &ureg = UnitConvert::getGlobalUnitRegistry();

  auto qf = ureg.makeQuantity<double>( vm["from-quantity"].as<std::string>() );
  try{
    auto qt = qf.to( vm["to-unit"].as<std::string>() );
    std::cout << qt.value();
    if( ! vm.count("value-only") ) std::cout << vm["to-unit"].as<std::string>();
    std::cout << std::endl;



  }
  catch(std::runtime_error e)
  {
    std::cerr << "There was an error converting " << vm["from-quantity"].as<std::string>()
      << " to " << vm["to-unit"].as<std::string>() << "\n";
    std::cerr << "Error Message: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "There was an unknown error. Sorry." << std::endl;
  }


  return 0;
}
