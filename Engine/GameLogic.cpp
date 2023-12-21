#include "GameLogic.h"
#include <assert.h>
#include <random>
#include "SpriteCodex.h"
#include <algorithm>

GameLogic::GameLogic(int nMines)
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, width - 1); // - 2 cuz we dont want bombs in last row & col
	std::uniform_int_distribution<int> yDist(0, height - 1);
	Vei2 spawnpos;
	int i = 0;
	while (i < nMines)
	{
		do
		{
			spawnpos = { xDist(rng), yDist(rng) };

		} while ( TileAt( spawnpos ).getHasMine());

		TileAt(spawnpos).SpawnMine();
		
		i++;
	}

	for (Vei2 gridpos = { 0, 0 }; gridpos.y < height; gridpos.y++)
	{
		for (gridpos.x = 0; gridpos.x < width; gridpos.x++)
		{		
			TileAt(gridpos).SetAdjMinesCount(NumberingCellsAdjToMines(gridpos));
		}
	}
}

void GameLogic::Tile::SpawnMine()
{
	assert(!hasMine);
	hasMine = true;
}

bool GameLogic::Tile::getHasMine() const
{
	return hasMine;
}

void GameLogic::Tile::Draw(const Vei2 screenpos, bool GameOver, Graphics& gfx) const
{
	if (!GameOver)
	{
		switch (state)
		{
		case State::Hidden:
			SpriteCodex::DrawTileButton(screenpos, gfx);
			break;

		case State::Flagged:
			SpriteCodex::DrawTileButton(screenpos, gfx);
			SpriteCodex::DrawTileFlag(screenpos, gfx);
			break;

		case State::Revealed:
			if (!hasMine)
			{
				SpriteCodex::DrawTileNumber(screenpos, nAdjMines, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenpos, gfx);
			}
			break;
		}
	}
	else // if gameover
	{
		switch (state)
		{
		case State::Hidden:
			if (hasMine)
			{
				SpriteCodex::DrawTileBomb(screenpos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenpos, gfx);
			}
			break;

		case State::Flagged:
			if (hasMine)
			{
				SpriteCodex::DrawTileBomb(screenpos, gfx);
				SpriteCodex::DrawTileFlag(screenpos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenpos, gfx);
				SpriteCodex::DrawTileCross(screenpos, gfx);
			}
			break;

		case State::Revealed:
			if (!hasMine)
			{
				SpriteCodex::DrawTileNumber(screenpos, nAdjMines, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBombRed(screenpos, gfx);
			}
			break;
		}
	}
}

void GameLogic::Tile::Reveal()
{
	assert(!hasRevealed());
	state = State::Revealed;
}

bool GameLogic::Tile::hasRevealed() const
{
	return state == State::Revealed;;
}

void GameLogic::Tile::ToggleFlag()
{
	assert(!hasFlagged() && !hasRevealed());
	state = State::Flagged;
}

bool GameLogic::Tile::hasFlagged() const
{
	return state == State::Flagged;
}

void GameLogic::Tile::SetAdjMinesCount(const int nAdjMineCount)
{
	assert(nAdjMines == -1);
	nAdjMines = nAdjMineCount;
}

int GameLogic::NumberingCellsAdjToMines( const Vei2& gridpos)
{ 
	const int xStart = std::max(0, gridpos.x - 1);
	const int yStart = std::max(0, gridpos.y - 1);
	const int xEnd   = std::min(width -1, gridpos.x + 1);
	const int yEnd   = std::min(height-1, gridpos.y + 1);

	int count = 0;
	for (Vei2 gpos = { xStart, yStart }; gpos.y <= yEnd; gpos.y++)
	{
		for (gpos.x = xStart; gpos.x <= xEnd; gpos.x++)
		{
			if (TileAt(gpos).getHasMine())
			{
				count++;
			}
		}
	}
	return count;
}


void GameLogic::Draw(Graphics& gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridpos = { 0, 0 }; gridpos.y < height; gridpos.y++)
	{
		for (gridpos.x = 0; gridpos.x < width; gridpos.x++)
		{	
			TileAt(gridpos).Draw(gridpos * SpriteCodex::tileSize, GameOver, gfx);
		}
	}

}

RectI GameLogic::GetRect() const
{
	return RectI(0, width * SpriteCodex::tileSize, 0, height * SpriteCodex::tileSize);
}

void GameLogic::RevealOnClickEvent(Vei2 screenpos)
{
	if (!GameOver)
	{
		assert(GetRect().ContainsPoint(screenpos));
		Vei2 gridpos = ScreenToGrid(screenpos);
		Tile& Tile = TileAt(gridpos);
		if (!Tile.hasRevealed())
		{
			Tile.Reveal();
			if (Tile.getHasMine())
			{
				GameOver = true;
			}
		}
	}
}

void GameLogic::FlagOnClickEvent(Vei2 screenpos)
{
	if (!GameOver)
	{
		assert(GetRect().ContainsPoint(screenpos));
		Vei2 gridpos = ScreenToGrid(screenpos);
		Tile& Tile = TileAt(gridpos);
		if (!Tile.hasRevealed() && !Tile.hasFlagged())
		{
			Tile.ToggleFlag();	
		}
	}
}

GameLogic::Tile& GameLogic::TileAt(const Vei2& gridpos)
{
	return field[gridpos.y * width + gridpos.x];
}

const GameLogic::Tile& GameLogic::TileAt(const Vei2& gridpos) const
{
	return field[gridpos.y * width + gridpos.x];
}

Vei2 GameLogic::ScreenToGrid(const Vei2& screenpos)
{
	return screenpos/SpriteCodex::tileSize;
}

