// Fill out your copyright notice in the Description page of Project Settings.
#include "Actor/Include/Socket/UnrealSocketSDT.h"

//Destructor, free memory
UnrealSocketSDT::~UnrealSocketSDT()
{
	if (ConnectionSocket){
		CloseConnectionSocket();
	}
	if (ListenerSocket){
		CloseListenerSocket();
	}
}

int UnrealSocketSDT::SendBytes(const void* Buffer, int32 N)
{
	const uint8* Data = static_cast<const uint8*>(Buffer);

	int32 Sent = 0;
	int32 TotalSent = 0;
	int TimeOut = 0;


	if (N != 1) 
	{
		uint8 const* DataLength;
		DataLength = reinterpret_cast<uint8 const*>(&N);
		ConnectionSocket->Send(DataLength, 4, Sent);
	}

	Sent = 0;
	while ((TotalSent < N) && (TimeOut < 5000))
	{
		TimeOut++;
		int32 Count = N - TotalSent;
		if (ConnectionSocket->Send(Data + TotalSent, Count, Sent))
		{
			TotalSent += Sent;
		}
	}

	if (TimeOut == 5000)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to send data"));
	}

	return TotalSent;
}

int UnrealSocketSDT::SendObsBytes(const void* Buffer, int32 N)
{
	const uint8* Data = static_cast<const uint8*>(Buffer);

	int32 Sent = 0;
	int32 TotalSent = 0;
	int TimeOut = 0;

	//uint8 const* DataLength;
	//DataLength = reinterpret_cast<uint8 const*>(&N);
	//ConnectionSocket->Send(DataLength, 4, Sent);

	Sent = 0;
	while ((TotalSent < N) && (TimeOut < 3000))
	{
		TimeOut++;
		int32 Count = N - TotalSent;
		if (ConnectionSocket->Send(Data + TotalSent, Count, Sent))
		{
			TotalSent += Sent;
		}
	}

	if (TimeOut == 3000)
	{
		int i = 3;
		UE_LOG(LogTemp, Error, TEXT("Failed to send data"));
	}

	return TotalSent;
}

bool UnrealSocketSDT::StartTCPReceiver(const int32 SocketPort)
{
	const FString Name = "SocketIsar";
	Port = SocketPort;
	ListenerSocket = CreateTCPConnectionListener(Name);

	//Not created?
	if (!ListenerSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("Listen socket cannot be created on %s:%i"), *IP, Port);
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Server waiting on %s:%i"), *IP, Port);

	return true;
}

FSocket* UnrealSocketSDT::CreateTCPConnectionListener(const FString& SocketName, const int32 ReceiveBufferSize)
{
	FSocket* ListenSocket = nullptr;

	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(IP4Nums))
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid IP! Expecting 4 parts separated by ."));
	}
	else
	{
		//Create Socket
		FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), Port);
		ListenSocket = FTcpSocketBuilder(*SocketName).AsReusable().BoundToEndpoint(Endpoint).Listening(8);

		//Set Buffer Size
		int32 NewSize = 0;
		ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);
	}

	return ListenSocket;
}

bool UnrealSocketSDT::TCPConnectionListener()
{
	bool Check = false;

	//~~~~~~~~~~~~~
	if (!ListenerSocket) return Check;
	//~~~~~~~~~~~~~

	//Remote address
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Pending;

	// handle incoming connections
	if (ListenerSocket->HasPendingConnection(Pending) && Pending)
	{
		//Already have a Connection? destroy previous
		if (ConnectionSocket)
		{
			CloseConnectionSocket();
		}

		//New Connection received
		ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("Received Socket Connection"));

		if (ConnectionSocket != NULL)
		{
			//Global cache of current Remote Address
			RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);
			UE_LOG(LogTemp, Warning, TEXT("New connection accepted on %s:%i"), *IP, Port);

			Check = true;
		}
	}

	return Check;
}

void UnrealSocketSDT::CloseConnectionSocket()
{
	ConnectionSocket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
	UE_LOG(LogTemp, Warning, TEXT("ConnectionSocket closed"));
	ConnectionSocket = nullptr;
}

void UnrealSocketSDT::CloseListenerSocket()
{
	ListenerSocket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenerSocket);
	UE_LOG(LogTemp, Warning, TEXT("ListenerSocket closed"));
	ListenerSocket = nullptr;
}

const FString UnrealSocketSDT::TCPSocketListener()
{
	//~~~~~~~~~~~~~
	if (!ConnectionSocket) return "";
	//~~~~~~~~~~~~~

	//Binary Array
	TArray<uint8> ReceivedData;

	uint32 Size;
	while (ConnectionSocket->HasPendingData(Size))
	{
		ReceivedData.Init(0, FMath::Min(Size, 65507u));

		int32 Read = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
	}

	if (ReceivedData.Num() <= 0)
	{
		//No Data Received
		return "";
	}

	return StringFromBinaryArray(ReceivedData);
}

bool UnrealSocketSDT::FormatIP4ToNumber(uint8(&Out)[4])
{
	//IP Formatting
	IP = IP.Replace(TEXT(" "), TEXT(""));

	//String Parts
	TArray<FString> Parts;
	IP.ParseIntoArray(Parts, TEXT("."), true);
	if (Parts.Num() != 4)
	{
		return false;
	}

	//String to Number Parts
	for (int32 i = 0; i < 4; ++i)
	{
		Out[i] = FCString::Atoi(*Parts[i]);
	}

	return true;
}

FString UnrealSocketSDT::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array
	const std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}
