#include "PFE_5JV.h"
#include "Modules/ModuleManager.h"
#include "RenderUtils.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FPFE_5JVModule, PFE_5JV, "PFE_5JV" );

void FPFE_5JVModule::StartupModule()
{
	FString ShaderDir = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders")));
    
	if (FPaths::DirectoryExists(ShaderDir))
	{
		AddShaderSourceDirectoryMapping(TEXT("/PFE_5JV"), ShaderDir);
	}
}

void FPFE_5JVModule::ShutdownModule()
{
	
}
