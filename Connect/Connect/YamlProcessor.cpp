#include "YamlProcessor.h"

YamlProcessor::YamlProcessor(const char* yamlStr) {
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
	yaml_parser_delete(&parser);
}

void YamlProcessor::parseMapping(yaml_parser_t& parser, yaml_event_t& event, std::unordered_map<std::string, valueTypeStruct>& keyValueMap) {
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
}