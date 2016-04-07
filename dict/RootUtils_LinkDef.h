#ifdef __CINT__
#pragma link off all classes;
#pragma link off all functions;
// --- add here the pragma links for your classes ---
#pragma link C++ class PlotUtils+;
#pragma link C++ class ConfigParser+;
// --- add here the pragma links for your functions ---
//#pragma link C++ function RooDataSetToTree;

// --- add here the pragma link for your instantations ---
//#pragma link C++ class std::map<RooAbsCategory*, std::string>;
//#pragma link C++ class std::map<int, MultiHistEntry*>::iterator;
#pragma link C++ class std::map<std::string, std::string >;
#pragma link C++ class std::vector< std::string >;
#pragma link C++ class std::map<std::string, bool >;
#pragma link C++ class std::map<std::string, std::string >::iterator;
#pragma link C++ class std::vector< std::string >::iterator;
#pragma link C++ class std::map<std::string, bool >::iterator;
#pragma link C++ class std::string;

#endif
