#pragma once
#include "Vei2.h"
#include "SpriteCodex.h"
#include<random>
#include "RectI.h"
#include "Sound.h"

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
		bool hasMine() const;
		void Draw( const Vei2 screenpos, bool GameOver, Graphics& gfx) const;
		void Reveal();
		bool hasRevealed() const;
		void ToggleFlag(bool value);
		bool hasFlagged() const;
		bool hasNoAjdMines() const;
		void SetAdjMinesCount( const int nAdjMineCount);
		
	private:
		State state = State::Hidden ;
		bool hasMined = false;
		int nAdjMines = -1; //uninitialized
	};
	 
public:
	GameLogic(Vei2& center);
	void Draw(Graphics& gfx) const;
	RectI GetRect() const;
	void RevealOnClickEvent(Vei2 screenpos);
	void FlagOnClickEvent(Vei2 screenpos);
	void RemoveFlagOnClickEvent(Vei2 screenpos);
	int NumberingCellsAdjToMines( const Vei2& gridpos );
	
	bool isWon() const;
	bool isGameOver() const;
	
	int nMinesFlagged = 0;
	

private:
	Tile& TileAt( const Vei2& gridpos);
	const Tile& TileAt( const Vei2& gridpos) const;
	void CountMinesFlagged(Vei2& gridpos);
	Vei2 ScreenToGrid(const Vei2& screenpos);
	void DestroyEmptyCells(const Vei2& gridpos);


private:   
	Vei2 boardpos;
	int nMines = 10;
	bool GameOver = false;
	Sound surprise = { L"Sounds\\surprise.wav" };
	static constexpr int width  = 10;
	static constexpr int height = 8;
	Tile field[width * height];
	int r = 0;
};
