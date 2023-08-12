// Fill out your copyright notice in the Description page of Project Settings.


#include "Utility/Math/MathLibrary.h"

bool UMathLibrary::GetLineIntersection(const FVector& SegmentStartA, const FVector& SegmentEndA, const FVector& SegmentStartB,
	const FVector& SegmentEndB, FVector& OutIntersection)
{
	return FMath::SegmentIntersection2D(SegmentStartA, SegmentEndA, SegmentStartB, SegmentEndB, OutIntersection);
}

FVector UMathLibrary::ExtendLineSegment(const FVector& SegmentStart, const FVector& SegmentEnd, float Length)
{
	FVector Result;
	
	float lenAB = sqrt(pow(SegmentStart.X - SegmentEnd.X, 2.0) + pow(SegmentStart.Y - SegmentEnd.Y, 2.0));
	Result.X = SegmentEnd.X + (SegmentEnd.X - SegmentStart.X) / lenAB * Length;
	Result.Y = SegmentEnd.Y + (SegmentEnd.Y - SegmentStart.Y) / lenAB * Length;

	return Result;
}
