#include "ChunkConfig.h"
#include "Spear/Logging.h"

void ValidateChunkConfig(FTerrainChunkConfig& terrainConfig, TArray<FInstancesChunkConfig>& instancesConfigs)
{
	for (auto& config : instancesConfigs)
	{
		if (config.chunk.size > terrainConfig.chunk.size)
		{
			PRINT_N_LOG(LogSpearDebug, Error, 
				TEXT("InstancesChunkConfig size %.2f greater than terrain size %.2f"),
				config.chunk.size, terrainConfig.chunk.size);
			config.chunk.size = terrainConfig.chunk.size;
		}

		if (config.chunk.chunksFromPlayer > terrainConfig.chunk.chunksFromPlayer)
		{
			PRINT_N_LOG(LogSpearDebug, Error,
				TEXT("InstancesChunkConfig chunksFromPlayer %i greater than terrain chunksFromPlayer %i"),
				config.chunk.chunksFromPlayer, terrainConfig.chunk.chunksFromPlayer);
			config.chunk.chunksFromPlayer = terrainConfig.chunk.chunksFromPlayer;
		}

		if (config.chunk.chunksFromEachPlayerServer > terrainConfig.chunk.chunksFromEachPlayerServer)
		{
			PRINT_N_LOG(LogSpearDebug, Error, 
				TEXT("InstancesChunkConfig chunksFromPlayerServer %i greater than terrain chunksFromPlayerServer %i"),
				config.chunk.chunksFromEachPlayerServer, terrainConfig.chunk.chunksFromEachPlayerServer);
			config.chunk.chunksFromEachPlayerServer = terrainConfig.chunk.chunksFromEachPlayerServer;
		}

		if (config.spacing > config.chunk.size)
		{
			PRINT_N_LOG(LogSpearDebug, Error, 
				TEXT("InstancesChunkConfig spacing %.2f greater than size %.2f"),
				config.spacing, config.chunk.size);
			config.spacing = config.chunk.size;
		}
	}
}