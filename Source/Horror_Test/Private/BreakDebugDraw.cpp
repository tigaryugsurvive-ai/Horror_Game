#include "BreakDebugDraw.h"
#include "DrawDebugHelpers.h"

namespace BreakDebugDraw
{
	static bool bEnabled = false;

	void Toggle()
	{
		bEnabled = !bEnabled;
	}

	bool IsEnabled()
	{
		return bEnabled;
	}

	void Sphere(UWorld* World, const FVector& Center, float Radius, float LifeTime, float Thickness)
	{
		if (!bEnabled || !World) return;

		DrawDebugSphere(
			World,
			Center,
			Radius,
			24,
			FColor::Cyan,
			false,
			LifeTime,
			0,
			Thickness
		);
	}

	void Line(UWorld* World, const FVector& A, const FVector& B, float LifeTime, float Thickness)
	{
		if (!bEnabled || !World) return;

		DrawDebugLine(
			World,
			A,
			B,
			FColor::Green,
			false,
			LifeTime,
			0,
			Thickness
		);
	}

	void Circle(UWorld* World, const FVector& Center, float Radius, float LifeTime, float Thickness, int32 Segments)
	{
		if (!bEnabled || !World) return;
		
		DrawDebugCircle(
			World,
			Center,
			Radius,
			Segments,
			FColor::Cyan,
			false,
			LifeTime,
			0,
			Thickness,
			FVector(1, 0, 0),   
			FVector(0, 1, 0),   
			false
		);
	}


	void DrawBreakRadius(UWorld* World, const FVector& Center, float Radius, float LifeTime, float Thickness)
	{
		if (!bEnabled || !World) return;

		DrawDebugSphere(
			World,
			Center,
			Radius,
			24,           
			FColor::Cyan,
			false,       
			LifeTime,
			0,
			Thickness
		);

		DrawDebugPoint(World, Center, 12.0f, FColor::Yellow, false, LifeTime);

		const FString Msg = FString::Printf(TEXT("Break Radius: %.1f"), Radius);
		DrawDebugString(World, Center + FVector(0, 0, 30), Msg, nullptr, FColor::White, LifeTime);
	}

	void DrawText(UWorld* World, const FVector& Location, const FString& Text, float LifeTime)
	{
		if (!bEnabled || !World) return;
		DrawDebugString(World, Location, Text, nullptr, FColor::White, LifeTime);
	}
}
