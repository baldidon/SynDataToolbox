// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Networking.h"
#include <string>

/**
 * 
 */
class SYNDATATOOLBOX_API UnrealSocketSDT
{
public:
	~UnrealSocketSDT();

	FString IP = "0.0.0.0";
	int32 Port;
	FSocket* ListenerSocket = nullptr;
	FSocket* ConnectionSocket = nullptr;
	FIPv4Endpoint RemoteAddressForConnection;

	int SendBytes(const void* Buffer, int32 N);
	int SendObsBytes(const void* Buffer, int32 N);
	bool StartTCPReceiver(const int32 SocketPort);
	FSocket* CreateTCPConnectionListener(const FString& SocketName, const int32 ReceiveBufferSize = 2 * 1024 * 1024);
	bool TCPConnectionListener(); 	//can thread this eventually
	const FString TCPSocketListener();		//can thread this eventually
	void CloseConnectionSocket();
	void CloseListenerSocket();
	bool FormatIP4ToNumber(uint8(&Out)[4]);
	FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);
};
