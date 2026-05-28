using UnrealBuildTool;

public class Horror_Test : ModuleRules
{
    public Horror_Test(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Chaos",
            "FieldSystemEngine",
            "GeometryCollectionEngine",
            "AIModule",
            "GameplayTasks",
            "NavigationSystem"
        });
    }
}
