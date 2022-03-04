#pragma once

#include <cassert>
#include <string>
#include <vector>
#include <unordered_map>
#include <libpq-fe.h>
class PsqlConnection {
	PGconn* conn;

public:
	PsqlConnection(PGconn* conn_) :conn(conn_) { assert(conn); };

	PsqlConnection() = delete;
	const PsqlConnection& operator=(const PsqlConnection& rhs) = delete;
	~PsqlConnection() { PQfinish(conn); }
	ConnStatusType getConnectionStatus()const {
		ConnStatusType status = PQstatus(conn);
		return status;
	}
	char* getDB()const { return PQdb(conn); }
	char* getUser()const { return PQuser(conn); }
	char* getPass()const { return PQpass(conn); }
	char* getHost()const { return PQhost(conn); }
	char* getPort()const { return PQport(conn); }
	char* getOptions()const { return PQoptions(conn); }
	int getServerVersion()const { return PQserverVersion(conn); }
	int getProtocolVersion()const { return PQprotocolVersion(conn); }
	char* getErrorMessage()const { return PQerrorMessage(conn); }
	int getSocket()const { return PQsocket(conn); }
	bool getConnectionNeedsPassword()const { return PQconnectionNeedsPassword(conn) ? true : false; }
	bool getConnectionUsedPassword()const { return PQconnectionUsedPassword(conn) ? true : false; }
	bool getSsqlInUse()const { return PQsslInUse(conn) ? true : false; }
	const char* getSsqlAttribute(const char* attrName_)const { return PQsslAttribute(conn, attrName_); }
	PGresult* execute(const char* command_)const { return PQexec(conn, command_); }
	operator PGconn* ()const { return conn; }
	operator ConnStatusType()const { return getConnectionStatus(); }

};

class PsqlConnectionBuilder {
	std::string host;
	std::string user;
	std::string db;
	std::string pass;
	int port{ 0 };
public:
	static PsqlConnectionBuilder begin() {
		PsqlConnectionBuilder builder;
		return builder;
	}
	PsqlConnectionBuilder& setHost(const char* pHost_) {
		host = pHost_;
		return *this;
	}
	PsqlConnectionBuilder& setUser(const char* pUser_) {
		user = pUser_;
		return *this;
	}
	PsqlConnectionBuilder& setDB(const char* pDB_) {
		db = pDB_;
		return *this;
	}
	PsqlConnectionBuilder& setPass(const char* pPass_) {
		pass = pPass_;
		return *this;
	}
	PsqlConnectionBuilder& setPort(int port_) {
		port = port_;
		return *this;
	}
	PGconn* build() {
		char conninfo[512];
		sprintf_s(conninfo, sizeof(conninfo), "user=%s password=%s dbname=%s hostaddr=%s port=%d", user.c_str(), pass.c_str(), db.c_str(), host.c_str(), port);
		PGconn* conn = PQconnectdb(conninfo);
		assert(conn);
		return conn;
	}

};


class PsqlReader;


class PsqlCommand {
	friend PsqlReader;
	PGconn* conn{ nullptr };
	PGresult* result{ nullptr };
public:
	PsqlCommand(PGconn* conn_, PGresult* result_) :conn(conn_), result(result_) {
		assert(conn);
	}
	PsqlCommand(PGconn* conn_, const char* cmd_) :conn(conn_) {
		assert(conn);
		result = PQexec(conn, cmd_);
	}
	PsqlCommand(PGconn* conn_) :conn(conn_) { assert(conn); }
	PsqlCommand(PGconn* conn_, const char* cmd_, int nParams_, const Oid* paramTypes_, const char* const* paramValues_, const int* paramLengths_, const int* paramFormats_, int resultFormat_) :conn(conn_) {
		assert(conn);
		result = PQexecParams(conn, cmd_, nParams_, paramTypes_, paramValues_, paramLengths_, paramFormats_, resultFormat_);
	}
	PsqlCommand(PGconn* conn_, const char* stmtName_, const char* query_, int nParams_, const Oid* paramTypes_) :conn(conn) {
		assert(conn);
		result = PQprepare(conn, stmtName_, query_, nParams_, paramTypes_);
	}
	~PsqlCommand() {

	}
	void execute(const char* cmd_) {
		result = PQexec(conn, cmd_);
	}
	void execute(const char* cmd_, int nParams_, const Oid* paramTypes_, const char* const* paramValues_, const int* paramLengths_, const int* paramFormats_, int resultFormat_) {
		result = PQexecParams(conn, cmd_, nParams_, paramTypes_, paramValues_, paramLengths_, paramFormats_, resultFormat_);
	}
	void executePrepared(const char* stmtName_, int nParams_, const char* const* paramValues_, const int* paramLengths_, const int* paramFormats_, int resultFormat_) {
		result = PQexecPrepared(conn, stmtName_, nParams_, paramValues_, paramLengths_, paramFormats_, resultFormat_);
	}
	operator PGconn* ()const { return conn; }
	operator PGresult* ()const { return result; }
	operator ExecStatusType()const { return getResultStatus(); }
	operator char* ()const { return getResStatus(); }
	PGresult* getResult()const { return result; }
	ExecStatusType getResultStatus()const { return PQresultStatus(result); }
	char* getResStatus()const { return PQresStatus(getResultStatus()); }
	char* getResultErrorMessage()const { return PQresultErrorMessage(result); }
	char* getResultErrorField(int fieldCode_)const { return PQresultErrorField(result, fieldCode_); }
	int getRowCount()const { return PQntuples(result); }
	int getColumnCount()const { return PQnfields(result); }
	char* getColumnName(int columnNumber_)const { return PQfname(result, columnNumber_); }
	int getColumnNumber(const char* columnName_)const { return PQfnumber(result, columnName_); }
	Oid getColumnTableOid(int columnNumber_)const { return PQftable(result, columnNumber_); }
	int getColumnTableColumn(int columnNumber_)const { return PQftablecol(result, columnNumber_); }
	int getColumnFormatCode(int columnNumber_)const { return PQfformat(result, columnNumber_); }
	Oid getColumnOid(int columnNumber_)const { return PQftype(result, columnNumber_); }
	int getColumnModifier(int columnNumber_)const { return PQfmod(result, columnNumber_); }
	int getColumnSize(int columnNumber_)const { return PQfsize(result, columnNumber_); }
	bool getIsBinaryData()const { return PQbinaryTuples(result) ? true : false; }
	char* getColumnValue(int rowNumber_, int columnNumber_)const {
		return PQgetvalue(result, rowNumber_, columnNumber_);
	}
	PsqlReader* getReader()const; // return new PsqlReader(*this); }
	std::vector<std::string> getColumnNames()const;/* {
		std::vector<std::string> columnNames(getColumnCount());
		for (int i = 0; i < getColumnCount(); ++i) {
			columnNames[i] = getColumnName(i);
		}
		return columnNames;
	}*/
};


class PsqlCommandBuilder {
	PGconn* conn{ nullptr };
	PGresult* result{ nullptr };

	PsqlCommandBuilder(PGconn* conn_, PGresult* result_) :conn(conn_), result(result_) {
		assert(conn);
	}

public:
	static PsqlCommandBuilder begin(PGconn* conn_) {
		PsqlCommandBuilder builder(conn_, nullptr);
		return builder;
	}
	PsqlCommandBuilder& setCmd(const char* cmd_) {
		result = PQexec(conn, cmd_);
		return *this;
	}
	PsqlCommandBuilder& setParams(const char* cmd_, int nParams_, const Oid* paramTypes_, const char* const* paramValues_, const int* paramLengths_, const int* paramFormats_, int resultFormat_) {
		result = PQexecParams(conn, cmd_, nParams_, paramTypes_, paramValues_, paramLengths_, paramFormats_, resultFormat_);
		return *this;
	}
	PsqlCommandBuilder& prepare(const char* stmtName_, const char* query_, int nParams_, const Oid* paramTypes_) {
		result = PQprepare(conn, stmtName_, query_, nParams_, paramTypes_);
	}
	PGresult* build() {
		return result;
	}

};

class PsqlReader {
	const PsqlCommand& cmd;
	PGresult* result{ nullptr };
	int rowCount{ 0 };
	int columnCount{ 0 };
	int currentRow{ -1 };
	std::unordered_map<std::string, int> columnMap;
public:
	PsqlReader(const PsqlCommand& cmd_) :cmd(cmd_), result(cmd.result) {
		rowCount = PQntuples(result);
		columnCount = PQnfields(result);
		for (int i = 0; i < columnCount; i++) {
			char* columnName = PQfname(result, i);
			columnMap.insert(std::pair<std::string, int>(columnName, i));
		}
	}
	PsqlReader() = delete;
	const PsqlReader& operator=(const PsqlReader& rhs) = delete;
	~PsqlReader() {}
	int getColumnCount()const { return columnCount; }
	char* operator[](int column)const {
		assert(column < columnCount);
		return PQgetvalue(result, currentRow, column);
	}
	char* operator[](const char* pcol) {
		int column = columnMap[pcol];
		return PQgetvalue(result, currentRow, column);
	}
	std::string operator[](std::string colName) {
		int column = columnMap[colName];
		return PQgetvalue(result, currentRow, column);
	}
	
	bool read();// { if (currentRow < rowCount) { currentRow++; return true; }return false; }
	std::vector<std::string> getRowValues()const {
		assert(currentRow < rowCount);
		std::vector<std::string> rowValues(rowCount);
		for (int i = 0; i < rowCount; ++i) {
			rowValues[i] = PQgetvalue(result, currentRow, i);
		}
		return rowValues;
	}
	std::vector<std::string> getRowValues(int row)const {
		assert(row < rowCount);
		std::vector<std::string> rowValues(rowCount);
		for (int i = 0; i < rowCount; ++i) {
			rowValues[i] = PQgetvalue(result, row, i);
		}
		return rowValues;
	}
	std::unordered_map<std::string, std::string> getRowKeyValuePairs()const {
		assert(currentRow < rowCount);
		std::vector<std::string> columnNames = cmd.getColumnNames();
		std::unordered_map<std::string, std::string> keyValuePairs;
		for (int i = 0; i < cmd.getColumnCount(); i++) {
			std::string value = PQgetvalue(result, currentRow, i);
			keyValuePairs.insert(std::pair<std::string, std::string>(columnNames[i], value));
		}
		return keyValuePairs;
	}
	std::unordered_map<std::string, std::string> getRowKeyValuePairs(int row)const {
		assert(row < rowCount);
		std::vector<std::string> columnNames = cmd.getColumnNames();
		std::unordered_map<std::string, std::string> keyValuePairs;
		for (int i = 0; i < cmd.getColumnCount(); i++) {
			std::string value = PQgetvalue(result, currentRow, i);
			keyValuePairs.insert(std::pair<std::string, std::string>(columnNames[i], value));
		}
		return keyValuePairs;
	}
};


