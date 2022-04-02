# Spear (Runtime Procedural Infinite Terrain)

This prototype in UE4 features an "infinite" terrain, procedurally generated in realtime.
To avoid large coordinate problems, the world origin is rebased if the player moves too far. 
Each client in a multiplayer game has its own world origin base.

The terrain chunks are generated with simplex noise.
Foliage can be added and is generated independently with poisson-disc sampling.

## View at ground level
![ground](https://github.com/ArthurKnauer/Spear/blob/main/Screenshots/ground.png)

## View from above with the visible chunk edges
![Above](https://github.com/ArthurKnauer/Spear/blob/main/Screenshots/above.png)