#pragma once
#include <string>
#include <unordered_map>
#include <exception>
#include <variant>
#define YAML_DECLARE_STATIC
#include <yaml.h>

struct valueTypeStruct {


	std::variant<std::string, std::vector<std::string>, std::unordered_map<std::string, valueTypeStruct>>valueType;
	void setScalar(std::string& str) {

		valueType = str;
	}
	void setSequence(std::vector<std::string >& sequence) {

		valueType = sequence;
	}
	void setMap(std::unordered_map<std::string, valueTypeStruct>& map) {

		valueType = map;
	}
	operator std::string()const { return std::get<std::string>(valueType); }
	operator const char* ()const { return std::get<std::string>(valueType).c_str(); }
	operator int()const { return (int)std::stoi(std::get<std::string>(valueType)); }
	operator double()const { return (double)std::stod(std::get<std::string>(valueType)); }
	operator std::vector<std::string>()const { return std::get<std::vector<std::string>>(valueType); }
	operator std::unordered_map<std::string, valueTypeStruct >()const { return std::get<std::unordered_map<std::string, valueTypeStruct>>(valueType); }
	valueTypeStruct operator [](std::string key)const {
		std::unordered_map<std::string, valueTypeStruct> map = std::get<std::unordered_map<std::string, valueTypeStruct>>(valueType);
		return map[key];
		//return (std::get<std::unordered_map<std::string, valueTypeStruct>>(valueType))[key]; }
	}
};


class YamlProcessor {
	std::unordered_map<std::string, valueTypeStruct> yamlkeyValueMap;
	void parseMapping(yaml_parser_t& parser, yaml_event_t& event, std::unordered_map<std::string, valueTypeStruct>& keyValueMap); /*{
		if (event.type != YAML_MAPPING_START_EVENT) {
			throw std::exception("Expected mapping start event!");
		}
		bool done = false;
		bool parseKey = true;
		bool parseValue = false;
		bool parseSequence = false;
		std::string keyStr;
		std::string valueStr;
		std::string sequenceItemStr;
		std::vector<std::string> sequence;
		valueTypeStruct valueType;
		do {
			yaml_event_delete(&event);
			if (!yaml_parser_parse(&parser, &event))
				throw std::exception("Unable to parse ");
			switch (event.type) {
			case YAML_SCALAR_EVENT:
				if (parseKey) {
					parseKey = false;
					parseValue = true;
					keyStr = (char*)event.data.scalar.value;
					if (keyStr == "xmas-fifth-day") {
						int z = 0;
					}
				}
				else if (parseValue) {
					valueStr = (char*)event.data.scalar.value;
					parseValue = false;
					parseKey = true;
					valueType.setScalar(valueStr);
					keyValueMap.insert(std::pair<std::string, valueTypeStruct>(keyStr, valueType));


				}
				else if (parseSequence) {
					sequenceItemStr = (char*)event.data.scalar.value;
					sequence.push_back(sequenceItemStr);
				}
				break;
			case YAML_SEQUENCE_START_EVENT:
				parseSequence = true;
				parseValue = false;
				break;
			case YAML_SEQUENCE_END_EVENT:
				valueType.setSequence(sequence);
				keyValueMap.insert(std::pair < std::string, valueTypeStruct>(keyStr, valueType));
				parseSequence = false;
				parseKey = true;
				break;
			case YAML_MAPPING_START_EVENT:
			{
				std::unordered_map<std::string, valueTypeStruct> keyValueMapChild;
				parseMapping(parser, event, keyValueMapChild);
				valueType.setMap(keyValueMapChild);
				keyValueMap.insert(std::pair<std::string, valueTypeStruct>(keyStr, valueType));
				parseKey = true;
				yaml_event_delete(&event);
				continue;
			}
			break;
			}

			done = (event.type == YAML_MAPPING_END_EVENT);
		} while (!done);
	}*/
public:
	YamlProcessor(const char* yamlStr);/* {
		yaml_parser_t parser;
		yaml_event_t event;

		yaml_parser_initialize(&parser);
		yaml_parser_set_input_string(&parser, (const unsigned char*)yamlStr, strlen(yamlStr));
		bool done = false;
		while (!done) {
			if (!yaml_parser_parse(&parser, &event))
				throw std::exception("Unable to parse ");
			switch (event.type) {
			case YAML_MAPPING_START_EVENT:
				parseMapping(parser, event, yamlkeyValueMap);
				break;
			}
			done = (event.type == YAML_STREAM_END_EVENT);
			yaml_event_delete(&event);
		}
	}*/
	operator std::unordered_map<std::string, valueTypeStruct>()const { return yamlkeyValueMap; }
	valueTypeStruct operator[](std::string key) { return yamlkeyValueMap[key]; }
};





