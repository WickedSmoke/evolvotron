#include <boost/program_options.hpp>
#include <iostream>

namespace boost {
  namespace program_options {
    
    BOOST_PROGRAM_OPTIONS_DECL typed_value<std::pair<int,int> >* intpair(std::pair<int,int>*);
    
  }
}

using namespace boost::program_options;

int main(int argc,char** argv)
{
  bool flag;
  int number;
  std::pair<int,int> pair;

  options_description options_desc("General options");
  options_desc.add_options()
    ("flag,f"
     ,bool_switch(&flag)
     ,"Control a bool")
    ("number,n"
     ,value<int>(&number)->default_value(23)
     ,"Specify a number")
    ("pair,p"
     ,intpair(&pair)->default_value(std::make_pair(2,3))
     ,"Specify 2 numbers")
    ;

  variables_map options;
  store(parse_command_line(argc,argv,options_desc),options);
  notify(options);

  std::cout 
    << "Flag:   " << flag << "\n"
    << "Number: " << number << "\n"
    << "Pair:   " << pair.first << " " << pair.second << "\n";

  return 0;
}
