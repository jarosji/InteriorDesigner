// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MathLibrary.generated.h"

/**
 * 
 */
UCLASS()
class JJCORE_API UMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static bool GetLineIntersection(const FVector& SegmentStartA, const FVector& SegmentEndA, const FVector& SegmentStartB,
	const FVector& SegmentEndB, FVector& OutIntersection);

	UFUNCTION(BlueprintPure)
	static FVector ExtendLineSegment(const FVector& SegmentStart, const FVector& SegmentEnd, float Length);
};
