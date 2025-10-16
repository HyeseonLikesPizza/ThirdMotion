// ServerManager.h - Network server management with Command Pattern
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/NetDriver.h"
#include "ServerManager.generated.h"

// Network role enum
UENUM(BlueprintType)
enum class ENetworkRole : uint8
{
	None UMETA(DisplayName = "None"),
	Server UMETA(DisplayName = "Server"),
	Client UMETA(DisplayName = "Client")
};

/**
 * Base Command Interface for Network Operations (Command Pattern)
 */
UCLASS(Abstract)
class THIRDMOTION_API UNetworkCommand : public UObject
{
	GENERATED_BODY()

public:
	// Execute the command
	virtual void Execute(UWorld* World) PURE_VIRTUAL(UNetworkCommand::Execute, );

	// Undo the command (optional)
	virtual void Undo(UWorld* World) {}

	// Check if command can execute
	virtual bool CanExecute(UWorld* World) const { return true; }
};

/**
 * Command: Create Server (Listen Server)
 */
UCLASS()
class THIRDMOTION_API UCreateServerCommand : public UNetworkCommand
{
	GENERATED_BODY()

public:
	virtual void Execute(UWorld* World) override;
	virtual bool CanExecute(UWorld* World) const override;

	UPROPERTY()
	FString ServerURL;
};

/**
 * Command: Connect to Server (Client)
 */
UCLASS()
class THIRDMOTION_API UConnectToServerCommand : public UNetworkCommand
{
	GENERATED_BODY()

public:
	virtual void Execute(UWorld* World) override;
	virtual bool CanExecute(UWorld* World) const override;

	UPROPERTY()
	FString ServerAddress;
};

/**
 * Command: Disconnect from Network
 */
UCLASS()
class THIRDMOTION_API UDisconnectCommand : public UNetworkCommand
{
	GENERATED_BODY()

public:
	virtual void Execute(UWorld* World) override;
	virtual bool CanExecute(UWorld* World) const override;
};

/**
 * Server Manager - Manages network connections using Command Pattern
 * Handles server/client role determination and replication
 */
UCLASS()
class THIRDMOTION_API UServerManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Execute network command (Command Pattern)
	UFUNCTION(BlueprintCallable, Category = "Network")
	void ExecuteNetworkCommand(UNetworkCommand* Command);

	// Convenience functions
	UFUNCTION(BlueprintCallable, Category = "Network")
	void CreateServer(const FString& MapName);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void ConnectToServer(const FString& ServerAddress);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void Disconnect();

	// Network role queries
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Network")
	bool IsServer() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Network")
	bool IsClient() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Network")
	bool HasAuthority() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Network")
	ENetworkRole GetCurrentNetworkRole() const;

	// Network state
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Network")
	bool IsConnected() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Network")
	FString GetServerURL() const { return CurrentServerURL; }

	// Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetworkRoleChanged, ENetworkRole, NewRole);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnServerCreated);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClientConnected);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetworkError, const FString&, ErrorMessage);

	UPROPERTY(BlueprintAssignable, Category = "Network Events")
	FOnNetworkRoleChanged OnNetworkRoleChanged;

	UPROPERTY(BlueprintAssignable, Category = "Network Events")
	FOnServerCreated OnServerCreated;

	UPROPERTY(BlueprintAssignable, Category = "Network Events")
	FOnClientConnected OnClientConnected;

	UPROPERTY(BlueprintAssignable, Category = "Network Events")
	FOnNetworkError OnNetworkError;

protected:
	// Current network role
	UPROPERTY()
	ENetworkRole CurrentNetworkRole;

	// Server URL
	UPROPERTY()
	FString CurrentServerURL;

	// Command history (for undo if needed)
	UPROPERTY()
	TArray<UNetworkCommand*> CommandHistory;

	// Network callbacks
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	// Helper functions
	void UpdateNetworkRole();
	void BroadcastRoleChange(ENetworkRole NewRole);
};
