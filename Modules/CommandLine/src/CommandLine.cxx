// Copyright Vladimir Prus 2002-2004.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

/* The simplest usage of the library.
*/

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std;

template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
  return os;
}

int main(int ac, char* av[])
{
  try {

    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      ("levels", po::value<int>(), "number of resolution levels")
      ("iterations", po::value<vector<int>>()->multitoken(), "number of iterations per level")
      ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << desc << "\n";
      return 0;
    }

    if (vm.count("levels")) {
      cout << "number of resolution levels was set to "
        << vm["levels"].as<int>() << ".\n";
    }
    else {
      cout << "number of resolution levels was not set.\n";
    }

    if (vm.count("iterations")) {
      cout << "number of iterations per level was set to "
        << vm["iterations"].as< vector<int> >() << "\n";
    }
    else {
      cout << "number of iterations per level was not set.\n";
    }

  }
  catch (exception& e) {
    cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch (...) {
    cerr << "Exception of unknown type!\n";
  }

  return 0;
}
