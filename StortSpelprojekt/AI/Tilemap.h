//Handles the walkable and non-walkable tiles
//Aron and Victor worked on this

//TODO Check the copy constructor to make sure it's correct. It's been a while since I did my last copy constructor :P. Aron

#ifndef TILEMAP_H
#define TILEMAP_H

class Tilemap
{
public:
	enum TileType{Walkable, Unwalkable};

private:
	int _height;
	int _width;
	
	TileType** _map;

public:
	bool IsValid(int x, int z) const;
	
public:
	Tilemap();
	Tilemap(int width, int height);
	Tilemap(const Tilemap& copy);
	~Tilemap();

	void ChangeTileState(int x, int z, TileType type);
	bool IsTileOccupied(int x, int z) const;
	TileType GetTileType(int x, int z) const;
	
	int SizeOfTileMap() const;
	int GetHeight() const;
	int GetWidth() const;
};
#endif