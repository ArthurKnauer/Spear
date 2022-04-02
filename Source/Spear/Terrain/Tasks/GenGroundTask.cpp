#include "GenGroundTask.h"

#include "../ChunkConfig.h"
#include "../Math/SimplexNoise.h"

FGenGroundTask::FGenGroundTask(
	FChunkCoords coords,
	const SimplexNoise* noise,
	const FTerrainChunkConfig* ground,
	TArray<FVector>& groundVertices) :
	coords(coords),
	noise(noise),
	ground(ground),
	vertices(groundVertices)
{
}

void FGenGroundTask::DoWork()
{
	const int quadsPlus1 = ground->quads + 1;
	const int vertexNum = quadsPlus1 * quadsPlus1;
	vertices.Reserve(vertexNum);
	normals.Reserve(vertexNum);
	tangents.Reserve(vertexNum);
	triangles.Reserve(ground->quads * ground->quads * 2);

	const auto noiseX = [&](int x) {return ground->coordScale * (coords.x + (static_cast<float>(x) / ground->quads)); };
	const auto noiseY = [&](int y) {return ground->coordScale * (coords.y + (static_cast<float>(y) / ground->quads)); };

	const float size_per_cell = ground->chunk.size / ground->quads;
	for (int y = 0; y <= ground->quads; ++y)
	{
		for (int x = 0; x <= ground->quads; ++x)
		{
			if (abort)
				return;

			float height = noise->noise2DEx(noiseX(x), noiseY(y),
				ground->lacunarity, ground->persistance, ground->octaves) * ground->amplitude + ground->offset;
			vertices.Add(FVector(x * size_per_cell, y * size_per_cell, height));
		}
	}

	for (int y = 0; y <= ground->quads; ++y)
	{
		const int y_offset = y * quadsPlus1;
		const int y_offset_prev = (y - 1) * quadsPlus1;
		const int y_offset_next = (y + 1) * quadsPlus1;

		for (int x = 0; x <= ground->quads; ++x)
		{
			if (abort)
				return;

			float center_height = vertices[x + y_offset].Z;

			float x_neg_height;
			if (x > 0)
				x_neg_height = vertices[x + y_offset - 1].Z;
			else
				x_neg_height = noise->noise2DEx(noiseX(x - 1), noiseY(y),
					ground->lacunarity, ground->persistance, ground->octaves) * ground->amplitude + ground->offset;

			float x_pos_height;
			if (x < ground->quads)
				x_pos_height = vertices[x + y_offset + 1].Z;
			else
				x_pos_height = noise->noise2DEx(noiseX(x + 1), noiseY(y),
					ground->lacunarity, ground->persistance, ground->octaves) * ground->amplitude + ground->offset;

			float y_neg_height;
			if (y > 0)
				y_neg_height = vertices[x + y_offset_prev].Z;
			else
				y_neg_height = noise->noise2DEx(noiseX(x), noiseY(y - 1),
					ground->lacunarity, ground->persistance, ground->octaves) * ground->amplitude + ground->offset;

			float y_pos_height;
			if (y < ground->quads)
				y_pos_height = vertices[x + y_offset_next].Z;
			else
				y_pos_height = noise->noise2DEx(noiseX(x), noiseY(y + 1),
					ground->lacunarity, ground->persistance, ground->octaves) * ground->amplitude + ground->offset;

			FVector y_neg(0, -size_per_cell, y_neg_height - center_height);
			FVector y_pos(0, size_per_cell, y_pos_height - center_height);
			FVector x_neg(-size_per_cell, 0, x_neg_height - center_height);
			FVector x_pos(size_per_cell, 0, x_pos_height - center_height);

			/*if (y == quads / 2 && x == quads / 2)
			{
				UE_LOG(LogTemp, Warning, TEXT("top(%.2f %.2f %.2f) bottom(%.2f %.2f %.2f) left(%.2f %.2f %.2f) right(%.2f %.2f %.2f)"),
					toTop.X, toTop.Y, toTop.Z,
					toBottom.X, toBottom.Y, toBottom.Z,
					toLeft.X, toLeft.Y, toLeft.Z,
					toRight.X, toRight.Y, toRight.Z);
			}*/

			FVector negNormal = FVector::CrossProduct(x_neg, y_neg);
			negNormal.Normalize();
			FVector posNormal = FVector::CrossProduct(x_pos, y_pos);
			posNormal.Normalize();

			FVector normal = negNormal + posNormal;
			normal.Normalize();
			normals.Add(normal);

			/*	if (y == quads / 2 && x == quads / 2)
				{

					UE_LOG(LogTemp, Warning, TEXT("topleftN(%.2f %.2f %.2f) bottomRightN(%.2f %.2f %.2f) NORMAL(%.2f %.2f %.2f)"),
						topLeftNormal.X, topLeftNormal.Y, topLeftNormal.Z,
						bottomRightNormal.X, bottomRightNormal.Y, bottomRightNormal.Z,
						normal.X, normal.Y, normal.Z);
				}*/

			x_pos.Normalize();
			tangents.Add(FProcMeshTangent(x_pos, false));
		}
	}

	for (int y = 0; y < ground->quads; ++y)
	{
		const int y_offset = y * quadsPlus1;
		const int y_offset_next = (y + 1) * quadsPlus1;

		for (int x = 0; x < ground->quads; ++x)
		{
			if (abort)
				return;

			triangles.Add(x + y_offset);
			triangles.Add(x + y_offset_next + 1);
			triangles.Add(x + y_offset + 1);

			triangles.Add(x + y_offset);
			triangles.Add(x + y_offset_next);
			triangles.Add(x + y_offset_next + 1);
		}
	}
}