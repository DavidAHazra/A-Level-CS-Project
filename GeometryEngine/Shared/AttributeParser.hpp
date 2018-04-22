#pragma once
#include "EngineHeader.hpp"

class AttributeParser {
    /*
     Able to parse files with the following structure:
     
     [attribute_name]:[attribute_value]
     [attribute_name]:[attribute_value]
     etc...
     
     */
    
public:
    AttributeParser(const std::string& full_path);
    AttributeParser(const AttributeParser& other);
    void operator=(const AttributeParser& other);
    
    void make_file() const;
    void save() const;
    void parse_all_attributes();
    
    void add_attribute(const std::string& attribute_name, const std::string& attribute_value);
    void change_attribute(const std::string& attribute_name, const std::string& new_value);
	std::string get_attribute(const std::string& attribute_name) const;
    inline std::string get_path() const { return path; }
    inline std::map<std::string, std::string> get_attributes() const { return attribute_list; }
    
protected:
    std::string path;
    std::map<std::string, std::string> attribute_list;
};
