// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>


class FURYFUNTALEPROD_API FFTP_DataBase
{

#define SQL_RESULT_LEN 256 //240 by³o
#define SQL_RETURN_CODE_LEN 1024 // 1000 by³o 

public:

	struct FFTP_DB_Rows
	{

	};

	struct FFTP_Account
	{
	public:
		enum EAccType
		{
			None,
			User,
			Admin
		};

		FFTP_Account();
		FFTP_Account(int id, FString login, FDateTime lastLoginTime, bool accessToAccount, EAccType accessRight);

	private:
		const int ID; 
		const FString Login;
		const FDateTime LastLoginTime;
		const bool AccessToAccount;
		const EAccType AccessRight;		
	};

	/*Database constructor with default connection string*/
	FFTP_DataBase();
	//FFTP_DataBase(const FString* connectionString = new FString("DRIVER=SQL Server;UID=Admin;PWD=start1;DATABASE=MultiplayerUE4;WSID=DESKTOP-H18RUHM;SERVER=DESKTOP-H18RUHM\\SQLEXPRESS"));
	~FFTP_DataBase();
	
	/*Check if client is able to connect to sql server*/
	bool ConnectTest();

	FFTP_Account* GetAccount();
	bool Login(FString login, FString password);
	
private:

	FFTP_Account _account;

	bool bConnectedToTheServer;

	SQLHANDLE sqlConnHandle;
	SQLHANDLE sqlStmtHandle;
	SQLHANDLE sqlEnvHandle;
	SQLWCHAR retconstring[SQL_RETURN_CODE_LEN];

	SQLWCHAR* _connectionString;

	const bool ConnectError();
	const bool ConnectCorrect();

	/*SQL Select*/
	bool Select(FFTP_DB_Rows* data, FString command);
	/*SQL Insert*/
	bool Insert(FString command);
	/*SQL Delete*/
	bool Delete(FString command);
};
