# RootUtils
This package provides a library with a set of utils for ROOT (CERN). This release include the following objects:
* __ConfigParser:__ for the parsing of configuration files.
* __PlotUtils:__ for helping in figures and root graphics editing.

#### ConfigParser
Parsing a configuration file is very simple by using the class ConfigParser of RootUtils. Characteristics and utilisation:
1. *Comments* are provided by the character `#`.
2. An option in loaded with the syntax: ```<name_option>=<option> ```.
Other separation character than `=` are implemented (*e.g.* `:` and `TAB`).
3. ```<name_option>``` represent the identifier of the option and it can be used through every class method to get the related option ```<option>```.

Configuration file example:
```
# This line is a comment
item1  =  15.0
item2  =  {1.,2.,3.,4.} #A list of values can be initialized by '{','}'
item3  = a random string
#end
```
