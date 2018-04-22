#include "AttributeParser.hpp"

AttributeParser::AttributeParser(const std::string& full_path) : path(full_path){
    parse_all_attributes();
}

AttributeParser::AttributeParser(const AttributeParser& other) : path(other.path){
    parse_all_attributes();
}

void AttributeParser::operator=(const AttributeParser& other){
    this->path = other.path;
    parse_all_attributes();
}

void AttributeParser::parse_all_attributes(){
    attribute_list.clear();
    if (!boost::filesystem::exists(path)){ return; }

    std::ifstream input_stream(path);
    std::string line_to_write;
    while (std::getline(input_stream, line_to_write)){
        bool is_parsing_value = false;
        
        std::string attribute_key;
        std::string attribute_value;
        
        for (size_t character_index = 0; character_index < line_to_write.size(); character_index++) {
            char current_char = line_to_write.at(character_index);
            
            if (!is_parsing_value) {
                if (line_to_write.at(character_index) == ':') { is_parsing_value = true; continue; }
                attribute_key.push_back(current_char);
            } else {
                attribute_value.push_back(current_char);
            }
        }
        
        if (attribute_key.empty() || attribute_value.empty()) { continue; }
        attribute_list.insert(std::pair<std::string, std::string>{attribute_key, attribute_value});
    }
}

void AttributeParser::make_file() const {
    if (boost::filesystem::exists(path)){ return; }
	boost::filesystem::create_directories(boost::filesystem::path(path).parent_path());

    std::ofstream output_stream;
    output_stream.open(path, std::ios::out);
    output_stream << "";
	output_stream.close();
}

void AttributeParser::save() const {
	make_file();

	std::ofstream output_stream;
	output_stream.open(path, std::ios::out);

	for (std::map<std::string, std::string>::const_iterator iter = attribute_list.begin(); iter != attribute_list.end(); iter++) {
		output_stream << iter->first << ":" << iter->second << '\n';
	}

    output_stream.close();
}

void AttributeParser::add_attribute(const std::string& attribute_name, const std::string& attribute_value){
    attribute_list.insert(std::pair<std::string, std::string>(attribute_name, attribute_value));
    save();
    parse_all_attributes();
}

void AttributeParser::change_attribute(const std::string& attribute_name, const std::string& new_value){
    for (std::map<std::string, std::string>::iterator iter = attribute_list.begin(); iter != attribute_list.end(); iter++) {
        if (iter->first == attribute_name){
            iter->second = new_value;
            save();
            parse_all_attributes();
            return;
        }
    }

	attribute_list.insert(std::pair<std::string, std::string>(attribute_name, new_value));
	save();
	parse_all_attributes();
}

std::string AttributeParser::get_attribute(const std::string& attribute_name) const {
	if (attribute_list.find(attribute_name) == attribute_list.end()) {
		// Key not found
		return "";
	} else {
		return attribute_list.at(attribute_name);
	}
}
