#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

#define SIZE 256
#define SCALE 4
#define UPDATE_RANDOM 0
#define ALLOW_DRAW 1
#define SLOW_ANIMATION 1

class Game : public olc::PixelGameEngine
{
private:
	int m_Scale = 4;
	int* m_Board;
	int* m_AliveBoard;
	bool m_SimulationStarted = false;
public:
	Game() : m_Board(new int[SIZE*SIZE]), m_AliveBoard(new int[SIZE*SIZE])
	{
		sAppName = "Conway's Game of Life";
	}
	~Game()
	{
		delete[] m_Board;
		delete[] m_AliveBoard;
	}
private:
	void InitializePopulation()
	{
		for (int i = 0; i < SIZE; i += 1)
			for (int j = 0; j < SIZE; j += 1)
			{
				m_Board[i*SIZE + j] = 0;
				m_AliveBoard[i*SIZE + j] = 0;
			}
	}
	int CountNeighbours(int x, int y)
	{
		int count = 0;
		if (x > 0 && y > 0)
			count += m_Board[y-1 + (x-1)*SIZE];
		if (x > 0)
			count += m_Board[y+(x-1)*SIZE];
		if (x > 0 && y < SIZE-1)
			count += m_Board[y + 1 + (x - 1)*SIZE];
		if (y > 0)
			count += m_Board[y-1+x*SIZE];
		if (y < SIZE-1)
			count += m_Board[y + 1 + x*SIZE];
		if (x < SIZE-1 && y > 0)
			count += m_Board[y-1 + (x+1)*SIZE];
		if (x < SIZE-1)
			count += m_Board[y+(x+1)*SIZE];
		if (x < SIZE-1 && y < SIZE - 1)
			count += m_Board[y+1+(x+1)*SIZE];
		return count;
	}
	void ClearAliveBoard()
	{
		for (int i = 0; i < SIZE; ++i)
			for (int j = 0; j < SIZE; ++j)
				m_AliveBoard[i*SIZE + j] = 0;
	}
	void RandomPopulation()
	{
		ClearAliveBoard();
		for (int i = 0; i < SIZE; i++)
			for (int j = 0; j < SIZE; j++)
			{
				m_Board[i*SIZE + j] = rand() % 10 >= 7 ? 1 : 0;
				if (m_Board[i*SIZE + j])
					m_AliveBoard[i*SIZE + j] = 1;
			}
	}
	void UpdatePopulation()
	{
		int* newPopulation = new int[SIZE*SIZE];
		for (int x = 0; x < SIZE; x++)
			for (int y = 0; y < SIZE; y++)
			{
				int neighbourCount = CountNeighbours(x, y);
				if (m_Board[y+x*SIZE] == 1 && neighbourCount >= 2 && neighbourCount <= 3)
				{
					newPopulation[y + x*SIZE] = 1;
					m_AliveBoard[y + x*SIZE] = 1;
				}
				else if (m_Board[y + x*SIZE] == 1 && neighbourCount > 3)
					newPopulation[y + x*SIZE] = 0;
				else if (m_Board[y + x*SIZE] == 0 && neighbourCount == 3)
				{
					newPopulation[y + x*SIZE] = 1;
					m_AliveBoard[y + x*SIZE] = 1;
				}
				else
					newPopulation[y + x*SIZE] = 0;
			}
		delete[] m_Board;
		m_Board = newPopulation;
	}
	void HandleMouse()
	{
		int x = GetMouseX();
		int y = GetMouseY();
		if (GetMouse(0).bHeld)
		{
#if ALLOW_DRAW
			if (m_SimulationStarted)
				m_SimulationStarted = false;
#endif
			m_Board[y+x*SIZE] = 1;
		}
		else if (GetMouse(1).bHeld)
		{
#if ALLOW_DRAW
			if (m_SimulationStarted)
				m_SimulationStarted = false;
#endif
			m_Board[y + x*SIZE] = 0;
		}
	}
public:
	bool OnUserCreate() override
	{
		InitializePopulation();
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
#if ALLOW_DRAW
		HandleMouse();
#else
		if (!m_SimulationStarted)
			HandleMouse();
#endif
		if (GetKey(olc::Key::ENTER).bPressed)
			m_SimulationStarted = true;

		if (GetKey(olc::Key::ESCAPE).bPressed)
		{
			m_SimulationStarted = false; 
			InitializePopulation();
		}

		if (GetKey(olc::Key::P).bPressed)
			m_SimulationStarted = false;

		if (!m_SimulationStarted && GetKey(olc::Key::R).bPressed)
			RandomPopulation();

		for (int x = 0; x < SIZE; x++)
			for (int y = 0; y < SIZE; y++)
			{
				olc::Pixel color = m_Board[y + x*SIZE] == 1 ? olc::BLACK : (m_AliveBoard[y + x*SIZE] ? olc::GREEN : olc::WHITE);
				Draw(x, y, color);
			}

		if (!m_SimulationStarted)
		{
			DrawString(0, 0, "LMB - Alive cell\nRMB - Dead cell\nEnter - Start simulation\nEscape - Restart simulation\nP - Pause simulation\nR - Random population", olc::DARK_BLUE, 1);
		}

#if UPDATE_RANDOM
		RandomPopulation();
#else
		if (m_SimulationStarted)
			UpdatePopulation();
#endif
#if SLOW_ANIMATION
		using namespace std::chrono_literals;
		if (m_SimulationStarted)
			std::this_thread::sleep_for(20ms);
#endif
		return true;
	}
};

int main()
{
	srand(time(NULL));
	Game demo;
	if (demo.Construct(SIZE, SIZE, SCALE, SCALE))
		demo.Start();
	return 0;
}