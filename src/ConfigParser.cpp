/**
* @Author: Marco Valente <marco>
* @Date:   2016-04-05T11:57:48+02:00
* @Email:  marco.valente@cern.ch
* @Last modified by:   marco
* @Last modified time: 2016-04-06T21:48:33+02:00
*/



#include "ConfigParser.h"

#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// Fuctions

void RemoveSpaces(string & str) {
    std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
    str.erase(end_pos, str.end());
}

void RemoveTabs(string & str) {
    std::string::iterator end_pos = std::remove(str.begin(), str.end(), '\t');
    str.erase(end_pos, str.end());
}

// Class implementation

ClassImp(ConfigParser)

ConfigParser::ConfigParser(): _options(), _verbose(true) {}

ConfigParser::ConfigParser(const char * nameconfigfile, bool verbose): _options(), _verbose(verbose) {
    if(_verbose) std::cout << " --- Parsing configuration options from: " << nameconfigfile << std::endl;
    Fill(nameconfigfile);
}

ConfigParser::~ConfigParser() {}

void                ConfigParser::Fill(const char * nameconfigfile) {
    Clear();
    vector<char> separators;

    separators.push_back('=');
    separators.push_back(':');
    separators.push_back('\t');

    std::ifstream file(nameconfigfile,ios::in);
    if(file.is_open()) {
      string                  buffer;
         while(getline(file,buffer)) {
        if (buffer.find("#") == 0 || buffer.empty()) {
            continue;
        }
        else if(buffer.find("#") > 0) {
            buffer = buffer.substr(0,buffer.find("#"));
        }
          std::stringstream   linestream(buffer);
          std::string         to;
          char                sep = '=';
        for(size_t s(0); s < separators.size();++s) {
            if(buffer.find(separators[s])<999999999.) {
                sep = separators[s];
                break;
            }
        }

        vector<string> val;
        while(std::getline(linestream,to,sep)){
            RemoveSpaces(to);
            RemoveTabs(to);
            val.push_back(to);
        }
        Option<string> tmp(val[0],val[1]);
        if(_verbose) std::cout << " --- Option: " << val[0] << " = " << val[1] << std::endl;
        _options.push_back(tmp);
        _names.push_back(val[0]);
     }
     file.close();
    }
    else {
      std::cerr << "Unable to open file " << nameconfigfile << endl;
    }
}

string              ConfigParser::GetString(string name) {
    string r;
    if(std::find(_names.begin(), _names.end(), name) != _names.end()){
        int index = find(_names.begin(), _names.end(), name) - _names.begin();
        r = _options[index].Value();
    }
    return r;
}

const char *        ConfigParser::GetConstChar(string name) {
    string r(GetString(name));
    return r.c_str();
}

int                 ConfigParser::GetInt(string name) {
    int ret;
    string val = GetString(name);
    ret = atoi(val.c_str());
    return ret;
}

double              ConfigParser::GetDouble(string name) {
    double ret;
    string val = GetString(name);
    ret = atof(val.c_str());
    return ret;
}

float               ConfigParser::GetFloat(string name) {
    float ret;
    string val = GetString(name);
    ret = atof(val.c_str());
    return ret;
}

int *               ConfigParser::GetIntArray(string name) {
    int * ret;
    string val = GetString(name);
    if(val.find("{") == 0 && val.find("}") > 0) {
        val.erase(0,1);
        val.erase(val.find("}"),val.find("}")+1);

        std::stringstream   valstream(val);
        std::string         to;
        char                sep = ',';
        vector<int> vec;
        while(std::getline(valstream,to,sep)){
            vec.push_back(atoi(to.c_str()));
        }
        int *r = new int[vec.size()];
        for (size_t k = 0; k < vec.size(); k++) {
            r[k] = vec[k];
        }
        ret = r;
    }
    else {
        std::cerr << "WARNING: impossible to find '{','}' in the option: " << name << std::endl;
        ret = 0;
    }

    return ret;
}

float *             ConfigParser::GetFloatArray(string name) {
    float * ret;
    string val = GetString(name);
    if(val.find("{") == 0 && val.find("}") > 0) {
        val.erase(0,1);
        val.erase(val.find("}"),val.find("}")+1);

        std::stringstream   valstream(val);
        std::string         to;
        char                sep = ',';
        vector<float> vec;
        while(std::getline(valstream,to,sep)){
            vec.push_back(atof(to.c_str()));
        }
        float *r = new float[vec.size()];
        for (size_t k = 0; k < vec.size(); k++) {
            r[k] = vec[k];
        }
        ret = r;
    }
    else {
        std::cerr << "WARNING: impossible to find '{','}' in the option: " << name << std::endl;
        ret = 0;
    }

    return ret;
}

double *            ConfigParser::GetDoubleArray(string name) {
    double * ret;
    string val = GetString(name);
    if(val.find("{") == 0 && val.find("}") > 0) {
        val.erase(0,1);
        val.erase(val.find("}"),val.find("}")+1);

        std::stringstream   valstream(val);
        std::string         to;
        char                sep = ',';
        vector<double> vec;
        while(std::getline(valstream,to,sep)){
            vec.push_back(atof(to.c_str()));
        }
        double *r = new double[vec.size()];
        for (size_t k = 0; k < vec.size(); k++) {
            r[k] = vec[k];
        }
        ret = r;
    }
    else {
        std::cerr << "WARNING: impossible to find '{','}' in the option: " << name << std::endl;
        ret = 0;
    }

    return ret;
}

string *            ConfigParser::GetStringArray(string name) {
    string * ret;
    string val = GetString(name);
    if(val.find("{") == 0 && val.find("}") > 0) {
        val.erase(0,1);
        val.erase(val.find("}"),val.find("}")+1);

        std::stringstream   valstream(val);
        std::string         to;
        char                sep = ',';
        vector<string> vec;
        while(std::getline(valstream,to,sep)){
            vec.push_back(to);
        }
        string *r = new string[vec.size()];
        for (size_t k = 0; k < vec.size(); k++) {
            r[k] = vec[k];
        }
        ret = r;
    }
    else {
        std::cerr << "WARNING: impossible to find '{','}' in the option: " << name << std::endl;
        ret = 0;
    }

    return ret;
}

vector<int>         ConfigParser::GetIntVector(string name) {
    vector<int> ret;
    string val = GetString(name);
    if(val.find("{") == 0 && val.find("}") > 0) {
        val.erase(0,1);
        val.erase(val.find("}"),val.find("}")+1);

        std::stringstream   valstream(val);
        std::string         to;
        char                sep = ',';
        while(std::getline(valstream,to,sep)){
            ret.push_back(atoi(to.c_str()));
        }
    }
    else {
        std::cerr << "WARNING: impossible to find '{','}' in the option: " << name << std::endl;
    }

    return ret;
}

vector<float>       ConfigParser::GetFloatVector(string name) {
    vector<float> ret;
    string val = GetString(name);
    if(val.find("{") == 0 && val.find("}") > 0) {
        val.erase(0,1);
        val.erase(val.find("}"),val.find("}")+1);

        std::stringstream   valstream(val);
        std::string         to;
        char                sep = ',';
        while(std::getline(valstream,to,sep)){
            ret.push_back(atof(to.c_str()));
        }
    }
    else {
        std::cerr << "WARNING: impossible to find '{','}' in the option: " << name << std::endl;
    }

    return ret;
}

vector<double>      ConfigParser::GetDoubleVector(string name) {
    vector<double> ret;
    string val = GetString(name);
    if(val.find("{") == 0 && val.find("}") > 0) {
        val.erase(0,1);
        val.erase(val.find("}"),val.find("}")+1);

        std::stringstream   valstream(val);
        std::string         to;
        char                sep = ',';
        while(std::getline(valstream,to,sep)){
            ret.push_back(atof(to.c_str()));
        }
    }
    else {
        std::cerr << "WARNING: impossible to find '{','}' in the option: " << name << std::endl;
    }

    return ret;
}

vector<string>      ConfigParser::GetStringVector(string name) {
    vector<string> vec;
    string val = GetString(name);
    if(val.find("{") == 0 && val.find("}") > 0) {
        val.erase(0,1);
        val.erase(val.find("}"),val.find("}")+1);

        std::stringstream   valstream(val);
        std::string         to;
        char                sep = ',';
        while(std::getline(valstream,to,sep)){
            vec.push_back(to);
        }
    }
    else {
        std::cerr << "WARNING: impossible to find '{','}' in the option: " << name << std::endl;
    }

    return vec;
}

vector<TString>     ConfigParser::GetTStringVector(string name) {
    vector<TString> vec;
    string val = GetString(name);
    if(val.find("{") == 0 && val.find("}") > 0) {
        val.erase(0,1);
        val.erase(val.find("}"),val.find("}")+1);

        std::stringstream   valstream(val);
        std::string         to;
        char                sep = ',';
        while(std::getline(valstream,to,sep)){
            vec.push_back(TString(to));
        }
    }
    else {
        std::cerr << "WARNING: impossible to find '{','}' in the option: " << name << std::endl;
    }

    return vec;
}

void                ConfigParser::Clear() {
    _names   = vector<string> ();
    _options = vector<Option<string>> ();
}

void                ConfigParser::SetVerbose(bool verbose) {
    _verbose = verbose;
}
