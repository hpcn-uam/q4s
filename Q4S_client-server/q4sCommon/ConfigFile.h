#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>

void exitWithError(const std::string &error);

class Convert
{
public:
        // Convert T, which should be a primitive, to a std::string.
	template <typename T>
	static std::string T_to_string(T const &val) 
	{
		std::ostringstream ostr;
		ostr << val;

		return ostr.str();
	}
	
        // Convert a std::string to T.	
	template <typename T>
	static T string_to_T(std::string const &val) 
	{
		std::istringstream istr(val);
		T returnVal;
		if (!(istr >> returnVal))
			exitWithError("CFG: Not a valid " + (std::string)typeid(T).name() + " received!\n");

		return returnVal;
	}
	// 
	//template <>
	static std::string string_to_T(std::string const &val)
	{
		return val;
	}
};

class ConfigFile
{
public:
	ConfigFile(const std::string &fName);
	bool keyExists(const std::string &key) const;

    template <typename ValueType>
    ValueType getValueOfKey(const std::string &key, ValueType const &defaultValue = ValueType()) const
    {
	    if (!keyExists(key))
		    return defaultValue;

	    return Convert::string_to_T<ValueType>(contents.find(key)->second);
    }

private:
	std::map<std::string, std::string> contents;
	std::string fName;

	void removeComment(std::string &line) const;
	bool onlyWhitespace(const std::string &line) const;
	bool validLine(const std::string &line) const;
	void extractKey(std::string &key, size_t const &sepPos, const std::string &line) const;
	void extractValue(std::string &value, size_t const &sepPos, const std::string &line) const;
	void extractContents(const std::string &line);
	void parseLine(const std::string &line, size_t const lineNo);
	void ExtractKeys();
};

#endif //_CONFIG_FILE_H_