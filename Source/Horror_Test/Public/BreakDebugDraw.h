#pragma once

#include "CoreMinimal.h"

// デバッグ描画をまとめたユーティリティ
namespace BreakDebugDraw
{
	// 有効/無効を切り替え（Bキー）
	void Toggle();

	bool IsEnabled();

	void Sphere(
		UWorld* World,
		const FVector& Center,
		float Radius,
		float LifeTime = 0.0f,
		float Thickness = 2.0f
	);

	void Line(
		UWorld* World,
		const FVector& A,
		const FVector& B,
		float LifeTime = 0.0f,
		float Thickness = 2.0f
	);

	void Circle(
		UWorld* World,
		const FVector& Center,
		float Radius,
		float LifeTime = 0.0f,
		float Thickness = 2.0f,
		int32 Segments = 64
	);


	void DrawBreakRadius(
		UWorld* World,
		const FVector& Center,
		float Radius,
		float LifeTime = 0.0f,
		float Thickness = 2.0f
	);

	void DrawText(
		UWorld* World,
		const FVector& Location,
		const FString& Text,
		float LifeTime = 0.0f
	);
}
