#include "MergeryEditorSubsystem.h"

#include "BlueprintEditor.h"
#include "Editor.h"
#include "LevelEditor.h"
#include "PackageHelperFunctions.h"
#include "Subsystems/AssetEditorSubsystem.h"

bool UMergeryEditorSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (FParse::Param(FCommandLine::Get(), TEXT("Mergery")))
		return FModuleManager::Get().IsModuleLoaded(TEXT("LevelEditor"));
	
	return false;
}

void UMergeryEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.OnLevelEditorCreated().AddLambda(
		[this](auto)
		{
			FString Base, Local, Remote;
			FParse::Value(FCommandLine::Get(), TEXT("MergeBase="), Base, false);
			FParse::Value(FCommandLine::Get(), TEXT("MergeLocal="), Local, false);
			FParse::Value(FCommandLine::Get(), TEXT("MergeRemote="), Remote, false);
			FParse::Value(FCommandLine::Get(), TEXT("MergeOutput="), Output, false);
		
			Base.ReplaceInline(TEXT("\\"), TEXT("/"));
			Local.ReplaceInline(TEXT("\\"), TEXT("/"));
			Remote.ReplaceInline(TEXT("\\"), TEXT("/"));
			Output.ReplaceInline(TEXT("\\"), TEXT("/"));

			const FString ObjectName = FPaths::GetBaseFilename(Output, true);
			
			UPackage*	BasePackage = LoadPackage(nullptr, *Base, 0);
			UBlueprint*	BaseObject = FindObject<UBlueprint>(BasePackage, *ObjectName);
		
			UPackage*	LocalPackage = LoadPackage(nullptr, *Local, 0);
			UBlueprint*	LocalObject = FindObject<UBlueprint>(LocalPackage, *ObjectName);
		
			UPackage*	RemotePackage = LoadPackage(nullptr, *Remote, 0);
			UBlueprint*	RemoteObject = FindObject<UBlueprint>(RemotePackage, *ObjectName);
			
			if (GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(LocalObject))
			{
				FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
		
				FBlueprintEditor* BlueprintEditor = static_cast<FBlueprintEditor*>(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(LocalObject, /*bFocusIfOpen =*/false));
				BlueprintEditor->CreateMergeToolTab(BaseObject, RemoteObject, FOnMergeResolved::CreateUObject(this, &ThisClass::OnMergeResolved));
			}
		}
	);
}

void UMergeryEditorSubsystem::OnMergeResolved(UPackage* Package, EMergeResult::Type Type)
{
	if (Type == EMergeResult::Completed)
	{
		if (SavePackageHelper(Package, Output))
		{
			FGenericPlatformMisc::RequestExitWithStatus(false, 0);
			return;
		}
	}

	FGenericPlatformMisc::RequestExitWithStatus(true, 1);
}
