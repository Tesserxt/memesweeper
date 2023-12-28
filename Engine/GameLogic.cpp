#include "GameLogic.h"
#include <assert.h>
#include <random>
#include "SpriteCodex.h"
#include <algorithm>

GameLogic::GameLogic(Vei2& center)
	:
	boardpos( center - Vei2(width * SpriteCodex::tileSize, height * SpriteCodex::tileSize)/2 )
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

		} while ( TileAt( spawnpos ).hasMine());

		TileAt(spawnpos).SpawnMine();
		
		i++;
	}

	for (Vei2 gridpos = { 0,0 }; gridpos.y < height; gridpos.y++)
	{					
		for (gridpos.x = 0; gridpos.x < width; gridpos.x++)
		{		
			TileAt(gridpos).SetAdjMinesCount(NumberingCellsAdjToMines(gridpos));
		}
	}
}

void GameLogic::Tile::SpawnMine()
{
	assert(!hasMined);
	hasMined = true;
}

bool GameLogic::Tile::hasMine() const
{
	return hasMined;
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
			if (!hasMined)
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
			if (hasMined)
			{
				SpriteCodex::DrawTileBomb(screenpos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenpos, gfx);
			}
			break;

		case State::Flagged:
			if (hasMined)
			{
				SpriteCodex::DrawTileBomb(screenpos, gfx);
				SpriteCodex::DrawTileFlag(screenpos, gfx);
			}
			else
			{
				//SpriteCodex::DrawTileBomb(screenpos, gfx);
				SpriteCodex::DrawTileCross(screenpos, gfx);
			}
			break;

		case State::Revealed:
			if (!hasMined)
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

void GameLogic::Tile::ToggleFlag(bool value)
{
	if (value)
	{
		assert(!hasFlagged() && !hasRevealed());
		state = State::Flagged;
	}
	else
	{
		assert(hasFlagged() && !hasRevealed());
		state = State::Hidden;
	}
}

bool GameLogic::Tile::hasFlagged() const
{
	return state == State::Flagged;
}

bool GameLogic::Tile::hasNoAjdMines() const
{
	return nAdjMines == 0;
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
			if (TileAt(gpos).hasMine())
			{
				count++;
			}
		}
	}
	return count;
}

void GameLogic::DestroyEmptyCells(const Vei2& gridpos)
{

	Tile& Tile = TileAt(gridpos);
	if (!Tile.hasRevealed() && !Tile.hasFlagged())
	{
		Tile.Reveal();
		assert( ! TileAt(gridpos).hasNoAjdMines());
		if (Tile.hasNoAjdMines())
		{
			const int xStart = std::max(0, gridpos.x - 1);
			const int yStart = std::max(0, gridpos.y - 1);
			const int xEnd = std::min(width - 1, gridpos.x + 1);
			const int yEnd = std::min(height - 1, gridpos.y + 1);

			for (Vei2 gpos = { xStart, yStart }; gpos.y <= yEnd; gpos.y++)
			{
				for (gpos.x = xStart; gpos.x <= xEnd; gpos.x++)
				{
					DestroyEmptyCells(gpos);
				}
			}
		}
	}



}

bool GameLogic::isWon() const
{
	for (const Tile t : field)
	{
		if (t.hasMine() && !t.hasFlagged() ||
			!t.hasMine() && !t.hasRevealed())
		{
			return false;
		}
	}
	return true;
}

bool GameLogic::isGameOver() const
{
	return GameOver;
}

void GameLogic::CountMinesFlagged(Vei2& gridpos)
{	
	if (TileAt(gridpos).hasMine() && TileAt(gridpos).hasFlagged())
	{
		nMinesFlagged++;
	}
}

void GameLogic::Draw(Graphics& gfx) const
{
	gfx.DrawRect(GetRect().GetExpanded(SpriteCodex::tileSize), Colors::Blue);
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	for (Vei2 gridpos = {0,0}; gridpos.y < height; gridpos.y++)
	{
		for (gridpos.x = 0; gridpos.x < width; gridpos.x++)
		{	
			TileAt(gridpos).Draw( boardpos + gridpos * SpriteCodex::tileSize, GameOver, gfx);
		}
	}

}

RectI GameLogic::GetRect() const
{
	return RectI( boardpos, width * SpriteCodex::tileSize, height * SpriteCodex::tileSize);
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
			if (Tile.hasMine())
			{
				GameOver = true;
				surprise.Play();
			}
			if( Tile.hasNoAjdMines())
				DestroyEmptyCells(gridpos);
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
			Tile.ToggleFlag(true);
			CountMinesFlagged(gridpos);
		}
	}
}

void GameLogic::RemoveFlagOnClickEvent(Vei2 screenpos)
{
	if (!GameOver)
	{
		assert(GetRect().ContainsPoint(screenpos));
		Vei2 gridpos = ScreenToGrid(screenpos);
		Tile& Tile = TileAt(gridpos);
		if (!Tile.hasRevealed() && Tile.hasFlagged())
		{
			Tile.ToggleFlag(false);
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
	return (screenpos - boardpos)/SpriteCodex::tileSize;
}

