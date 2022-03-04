#include "Psql.h"



PsqlReader* PsqlCommand::getReader()const { 
	return new PsqlReader(*this); 
}


std::vector<std::string> PsqlCommand::getColumnNames()const {
	std::vector<std::string> columnNames(getColumnCount());
	for (int i = 0; i < getColumnCount(); ++i) {
		columnNames[i] = getColumnName(i);
	}
	return columnNames;
}

bool PsqlReader::read() { 
	if (++currentRow < rowCount){ 		
		return true; 
	}
	return false; }