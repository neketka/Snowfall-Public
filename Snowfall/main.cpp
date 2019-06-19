#include <Windows.h>

#include <Snowfall.h>


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	EngineSettings settings;

	settings.TextureUnits = 84;
	settings.ImageUnits = 32;
	settings.MaxMeshMemoryBytes = 1000000000;
	settings.MaxMeshCommands = 200000;
	settings.ShadowMapResolution = 512;
	settings.CloseShadowMapResolution = 2048;

	Snowfall::InitGlobalInstance(settings);

	Snowfall::GetGameInstance().GetAssetManager().EnumerateUnpackedFolder(".\\Assets");
	Snowfall::GetGameInstance().LoadModules({ "SnowfallGame.dll" });
	
	Snowfall::GetGameInstance().StartGame();

	return 0;
}

