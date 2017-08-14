// Fill out your copyright notice in the Description page of Project Settings.

#include "FuryFunTaleProd.h"
#include "FFTP_DataBase.h"
#include <string>

FFTP_DataBase::FFTP_DataBase()
{
	//_connectionString = "DRIVER=SQL Server;UID=Admin;PWD=start1;DATABASE=MultiplayerUE4;WSID=DESKTOP-H18RUHM;SERVER=DESKTOP-H18RUHM\\SQLEXPRESS"
	//WCHAR* wCS = (WCHAR*) "DRIVER=SQL Server;UID=Admin;PWD=start1;DATABASE=MultiplayerUE4;WSID=DESKTOP-H18RUHM;SERVER=DESKTOP-H18RUHM\\SQLEXPRESS";
	_connectionString = (SQLWCHAR*) L"DRIVER=SQL Server;UID=Admin;PWD=start1;DATABASE=MultiplayerUE4;WSID=DESKTOP-H18RUHM;SERVER=DESKTOP-H18RUHM\\SQLEXPRESS";

	bConnectedToTheServer = true;

	sqlConnHandle = NULL;
	sqlStmtHandle = NULL;

	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
		bConnectedToTheServer = ConnectError();
	if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
		bConnectedToTheServer = ConnectError();
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
		bConnectedToTheServer = ConnectError();

	if (!bConnectedToTheServer)
		return;

	bConnectedToTheServer = ConnectTest();
}
//FFTP_DataBase::FFTP_DataBase(const FString* connectionString = new FString("DRIVER=SQL Server;UID=Admin;PWD=start1;DATABASE=MultiplayerUE4;WSID=DESKTOP-H18RUHM;SERVER=DESKTOP-H18RUHM\\SQLEXPRESS"))
//{

//}

FFTP_DataBase::~FFTP_DataBase()
{
}

const bool FFTP_DataBase::ConnectError()
{
	UE_LOG(LogTemp, Error, TEXT("Could not connect to the Server"));
	bConnectedToTheServer = false;
	return false;
}

const bool FFTP_DataBase::ConnectCorrect()
{
	UE_LOG(LogTemp, Log, TEXT("You are connected to the server"));
	bConnectedToTheServer = true;
	return true;
}

bool FFTP_DataBase::ConnectTest()
{
		UE_LOG(LogTemp, Log, TEXT("Attempting connection to SQL Server..."));
	
		switch (SQLDriverConnect(sqlConnHandle,
			GetDesktopWindow(),
			_connectionString,
			SQL_NTS,
			NULL,
			0,
			NULL,
			SQL_DRIVER_COMPLETE)) {
		case SQL_SUCCESS:
			return ConnectCorrect();
		case SQL_SUCCESS_WITH_INFO:
			return ConnectCorrect();
		case SQL_INVALID_HANDLE:
			return ConnectError();
		case SQL_ERROR:
			return ConnectError();
		default:
			break;
		}
		return true;
}


bool FFTP_DataBase::Select(FFTP_DB_Rows* data, FString command)
{
	return true;
}

bool FFTP_DataBase::Insert(FString command)
{
	return true;
}

bool FFTP_DataBase::Delete(FString command)
{
	return true;
}

FFTP_DataBase::FFTP_Account* FFTP_DataBase::GetAccount()
{
	return &_account;
}


bool FFTP_DataBase::Login(FString login, FString password)
{
	FFTP_DB_Rows data;
	FString command;
	if(Select(&data, command))
	{
		
		return true;
	}
	return false;
}

FFTP_DataBase::FFTP_Account::FFTP_Account()
	:ID(0), Login(""), LastLoginTime(FDateTime()), AccessToAccount(false), AccessRight(EAccType::None)
{
}

FFTP_DataBase::FFTP_Account::FFTP_Account(int id, FString login, FDateTime lastLoginTime, bool accessToAccount, EAccType accessRight)
	:ID(id), Login(login), LastLoginTime(lastLoginTime), AccessToAccount(accessToAccount), AccessRight(accessRight)
{
}
