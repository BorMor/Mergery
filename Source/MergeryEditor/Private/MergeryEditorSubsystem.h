#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Merge/Public/Merge.h"
#include "MergeryEditorSubsystem.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UMergeryEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
public:
	bool ShouldCreateSubsystem(UObject* Outer) const override;
	void Initialize(FSubsystemCollectionBase& Collection) override;
protected:
	void OnMergeResolved(UPackage* Package, EMergeResult::Type Type);

	FString Output;
};
