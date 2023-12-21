#pragma once
#include "Board.h"
#include "Vei2.h"
#include "SpriteCodex.h"
#include<random>
#include "RectI.h"

class GameLogic
{
private:
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Flagged,
			Revealed
		};
	public:
		void SpawnMine();
		bool getHasMine() const;
		void Draw( const Vei2 screenpos, bool GameOver, Graphics& gfx) const;
		void Reveal();
		bool hasRevealed() const;
		void ToggleFlag();
		bool hasFlagged() const;
		void SetAdjMinesCount( const int nAdjMineCount);
		
	private:
		State state = State::Hidden;
		bool hasMine = false;
		int nAdjMines = -1; //uninitialized
	};
	 
public:
	GameLogic(int nMines);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void RevealOnClickEvent(Vei2 screenpos);
	void FlagOnClickEvent(Vei2 screenpos);
	int NumberingCellsAdjToMines( const Vei2& BombLoc );

private:
	Tile& TileAt( const Vei2& gridpos);
	const Tile& TileAt( const Vei2& gridpos) const;
	Vei2 ScreenToGrid(const Vei2& screenpos);


private:   
	Vei2 BoardLoc = { 0, 0 };
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[width * height];
	bool GameOver = false;
};
