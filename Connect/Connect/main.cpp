#include <cstdlib>
#include <cstdio>
#include <string>
#include <memory>
#include <cassert>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include "YamlProcessor.h"

#include "Psql.h"

const char* yamlConfig =
"HOST: 127.0.0.1\n"
"USER: postgres\n"
"DB: postgres\n"
"PASS: your-password\n"
"PORT: 5433";

int main() {
	int exitCode = EXIT_SUCCESS;
	
	YamlProcessor y(yamlConfig);
	std::unique_ptr<PsqlConnection> conn = std::make_unique<PsqlConnection>(PsqlConnectionBuilder::begin()
		.setUser(y["USER"])
		.setPass(y["PASS"])
		.setHost(y["HOST"])
		.setDB(y["DB"])
		.setPort(y["PORT"])
		.build());
	if (*conn == CONNECTION_OK) {
		
		std::unique_ptr<PsqlCommand> cmd = std::make_unique<PsqlCommand>(*conn,PsqlCommandBuilder::begin(*conn)
			//.setCmd("SELECT version();")
			.setCmd("SELECT 1 AS FOO, 2 AS \"BAR\"")
			.build());
		fprintf(stdout, "Result status: %s, rows: %d, columns: %d\n", cmd->getResStatus(),cmd->getRowCount(),cmd->getColumnCount());
		fprintf(stdout, "\t");
		for (int i = 0; i < cmd->getColumnCount(); i++) {
			fprintf(stdout, "%s(%d)\t",cmd->getColumnName(i),cmd->getColumnFormatCode(i));
		}
		
		fprintf(stdout,"\n");
		for (int i = 0; i < cmd->getRowCount(); i++) {
			fprintf(stdout, "%d:\t", i);
			for (int j = 0; j < cmd->getColumnCount(); j++) {
				fprintf(stdout, "%s\t", cmd->getColumnValue(i, j));
			}
			fprintf(stdout, "\n");
		}
		int row = 0;
		std::cout << "Row#";
		for (int i = 0; i < cmd->getColumnCount(); i++) {
			std::cout << "\t" << cmd->getColumnName(i);
		}
		std::cout << std::endl;

		

		std::unique_ptr<PsqlReader> reader = std::make_unique<PsqlReader>(*cmd);
		while (reader->read()) {
			std::cout << row++;
			for (int i = 0; i < cmd->getColumnCount(); i++) {
				std::string colName = cmd->getColumnName(i);
				std::cout << "\t" << reader->operator[](colName);
			}
			std::cout << std::endl;
		}

		
	}
	else {
		std::cerr << "Error connecting: " << conn->getErrorMessage() << std::endl;
		exitCode = EXIT_FAILURE;
	}
	return exitCode;
}