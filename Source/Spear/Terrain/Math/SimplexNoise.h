#pragma once

class  SimplexNoise
{
public:
	SimplexNoise(int seed = 0);

	float noise2DEx(float x, float y, float lacunarity, float persistance, int octaves) const;
	float noise2D(float x, float y) const;
	
private:
	unsigned char perm[512];
};

