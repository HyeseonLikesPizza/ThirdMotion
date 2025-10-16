// ServerManager.cpp
// Copyright Epic Games, Inc. All Rights Reserved.

#include "Network/ServerManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/NetDriver.h"

// ============================================================================
// UCreateServerCommand Implementation
// ============================================================================
void UCreateServerCommand::Execute(UWorld* World)
{
	if (!World) return;

	UE_LOG(LogTemp, Log, TEXT("CreateServerCommand: Creating listen server on map: %s"), *ServerURL);

	// Open level as listen server
	UGameplayStatics::OpenLevel(World, FName(*ServerURL), true, "listen");
}

bool UCreateServerCommand::CanExecute(UWorld* World) const
{
	return World != nullptr && !ServerURL.IsEmpty();
}

// ============================================================================
// UConnectToServerCommand Implementation
// ============================================================================
void UConnectToServerCommand::Execute(UWorld* World)
{
	if (!World) return;

	UE_LOG(LogTemp, Log, TEXT("ConnectToServerCommand: Connecting to server: %s"), *ServerAddress);

	// Connect to server
	UGameplayStatics::OpenLevel(World, FName(*ServerAddress), true);
}

bool UConnectToServerCommand::CanExecute(UWorld* World) const
{
	return World != nullptr && !ServerAddress.IsEmpty();
}

// ============================================================================
// UDisconnectCommand Implementation
// ============================================================================
void UDisconnectCommand::Execute(UWorld* World)
{
	if (!World) return;

	UE_LOG(LogTemp, Log, TEXT("DisconnectCommand: Disconnecting from network"));

	// Return to main menu or disconnect
	UGameplayStatics::OpenLevel(World, FName("MainMenu"), false);
}

bool UDisconnectCommand::CanExecute(UWorld* World) const
{
	return World != nullptr;
}

// ============================================================================
// UServerManager Implementation
// ============================================================================
void UServerManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentNetworkRole = ENetworkRole::None;
	CurrentServerURL = TEXT("");

	// Bind network failure delegate
	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UServerManager::OnNetworkFailure);
	}

	UE_LOG(LogTemp, Log, TEXT("ServerManager initialized"));
}

void UServerManager::Deinitialize()
{
	// Unbind delegates
	if (GEngine)
	{
		GEngine->OnNetworkFailure().RemoveAll(this);
	}

	CommandHistory.Empty();

	Super::Deinitialize();

	UE_LOG(LogTemp, Log, TEXT("ServerManager deinitialized"));
}

void UServerManager::ExecuteNetworkCommand(UNetworkCommand* Command)
{
	if (!Command)
	{
		UE_LOG(LogTemp, Error, TEXT("ServerManager: Command is null!"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("ServerManager: World is null!"));
		return;
	}

	// Check if command can execute
	if (!Command->CanExecute(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("ServerManager: Command cannot execute"));
		return;
	}

	// Execute command
	Command->Execute(World);

	// Add to history
	CommandHistory.Add(Command);

	// Update network role
	UpdateNetworkRole();

	UE_LOG(LogTemp, Log, TEXT("ServerManager: Command executed successfully"));
}

void UServerManager::CreateServer(const FString& MapName)
{
	UCreateServerCommand* Command = NewObject<UCreateServerCommand>(this);
	if (Command)
	{
		Command->ServerURL = MapName;
		ExecuteNetworkCommand(Command);

		CurrentServerURL = MapName;
		BroadcastRoleChange(ENetworkRole::Server);
		OnServerCreated.Broadcast();
	}
}

void UServerManager::ConnectToServer(const FString& ServerAddress)
{
	UConnectToServerCommand* Command = NewObject<UConnectToServerCommand>(this);
	if (Command)
	{
		Command->ServerAddress = ServerAddress;
		ExecuteNetworkCommand(Command);

		CurrentServerURL = ServerAddress;
		BroadcastRoleChange(ENetworkRole::Client);
		OnClientConnected.Broadcast();
	}
}

void UServerManager::Disconnect()
{
	UDisconnectCommand* Command = NewObject<UDisconnectCommand>(this);
	if (Command)
	{
		ExecuteNetworkCommand(Command);

		CurrentServerURL = TEXT("");
		BroadcastRoleChange(ENetworkRole::None);
	}
}

bool UServerManager::IsServer() const
{
	UWorld* World = GetWorld();
	if (!World) return false;

	// Check if this is the authority (server or listen server)
	return World->GetAuthGameMode() != nullptr;
}

bool UServerManager::IsClient() const
{
	UWorld* World = GetWorld();
	if (!World) return false;

	// Check if this is a client (not server)
	return World->GetNetMode() == NM_Client;
}

bool UServerManager::HasAuthority() const
{
	UWorld* World = GetWorld();
	if (!World) return false;

	// Check authority using GetNetMode
	ENetMode NetMode = World->GetNetMode();
	return NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer;
}

ENetworkRole UServerManager::GetCurrentNetworkRole() const
{
	return CurrentNetworkRole;
}

bool UServerManager::IsConnected() const
{
	return CurrentNetworkRole != ENetworkRole::None;
}

void UServerManager::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogTemp, Error, TEXT("ServerManager: Network failure - %s"), *ErrorString);

	// Broadcast error event
	OnNetworkError.Broadcast(ErrorString);

	// Reset network role
	CurrentNetworkRole = ENetworkRole::None;
	CurrentServerURL = TEXT("");
	BroadcastRoleChange(ENetworkRole::None);
}

void UServerManager::UpdateNetworkRole()
{
	ENetworkRole NewRole = ENetworkRole::None;

	if (IsServer())
	{
		NewRole = ENetworkRole::Server;
	}
	else if (IsClient())
	{
		NewRole = ENetworkRole::Client;
	}

	if (NewRole != CurrentNetworkRole)
	{
		CurrentNetworkRole = NewRole;
		BroadcastRoleChange(NewRole);
	}
}

void UServerManager::BroadcastRoleChange(ENetworkRole NewRole)
{
	OnNetworkRoleChanged.Broadcast(NewRole);

	FString RoleString = NewRole == ENetworkRole::Server ? TEXT("Server") :
	                     NewRole == ENetworkRole::Client ? TEXT("Client") : TEXT("None");

	UE_LOG(LogTemp, Log, TEXT("ServerManager: Network role changed to %s"), *RoleString);
}
