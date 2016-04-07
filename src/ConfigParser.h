/**
* @Author: Marco Valente <marco>
* @Date:   2016-04-05T11:53:45+02:00
* @Email:  marco.valente@cern.ch
* @Last modified by:   marco
* @Last modified time: 2016-04-06T21:48:22+02:00
*/


#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <iostream>
#include <vector>
#include <string>

#include "TString.h"

using namespace std;

// Option parser class
class NamedOption{
public:
    NamedOption(): m_name() {}
    NamedOption(const NamedOption & opt): m_name(opt.m_name) {}
    NamedOption(const std::string& name) //note: we don't lightly copy strings in C++
      : m_name(name) {}
    std::string Name() {return m_name;}
    virtual ~NamedOption() {}
protected:
    std::string m_name;
};

template< typename T >
class Option: public NamedOption {
public:
    Option(): NamedOption(),m_data() {}
    Option(const Option<T> & opt): NamedOption(opt.m_name), m_data(opt.m_data) {}
    Option(const std::string& name, const T& data): NamedOption(name), m_data(data) {}
    T Value() {return m_data;}
private:
    T m_data;
};

class ConfigParser {
    public:
        ConfigParser();
        ConfigParser(const char * nameconfigfile, bool verbose = false);
        virtual ~ConfigParser();

        void            Fill(const char * nameconfigfile);
        string          GetString(string name);
        const char *    GetConstChar(string name);
        int             GetInt(string name);
        double          GetDouble(string name);
        float           GetFloat(string name);

        int *           GetIntArray(string name);
        float *         GetFloatArray(string name);
        double *        GetDoubleArray(string name);
        string *        GetStringArray(string name);
        vector<int>     GetIntVector(string name);
        vector<float>   GetFloatVector(string name);
        vector<double>  GetDoubleVector(string name);
        vector<string>  GetStringVector(string name);
        vector<TString> GetTStringVector(string name);

        void            Clear();
        void            SetVerbose(bool verbose);

        ClassDef(ConfigParser,1)
    private:
        vector<string>         _names;
        vector<Option<string>> _options;
        bool                   _verbose;
};


#endif
