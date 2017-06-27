#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

#include <array>
#include <type_traits>
#include <cassert>
#include <vector>
#include <random>

enum class MazeDir {
	UP = 0,
	DOWN,
	LEFT,
	RIGHT,
	ALL,
	NONE
};

MazeDir opposite(MazeDir dir);

struct MazeIndex {
	MazeIndex(int x, int y) : m_x(x), m_y(y) {}
	
	MazeIndex neigh(MazeDir dir) {
		switch (dir) {
			case MazeDir::UP: return MazeIndex(m_x, m_y + 1); break;
			case MazeDir::DOWN: return MazeIndex(m_x, m_y - 1); break;
			case MazeDir::LEFT: return MazeIndex(m_x - 1, m_y); break;
			case MazeDir::RIGHT: return MazeIndex(m_x + 1, m_y); break;
			default: abort();
		}
	}

	bool inside(int x, int y) {
		return 0 <= m_x && m_x < x && 0 <= m_y && m_y < y;
	}
	
	int x() { return m_x; }
	int y() { return m_y; }
	
private:
	int m_x;
	int m_y;
};

struct MazeNode {
public:
	MazeNode() : m_main_path(false), m_order(0), m_visited(false) { m_dirs.fill(false); }
	
	bool& dir(MazeDir dir) { 
		assert(dir < MazeDir::ALL);
		return m_dirs[static_cast<int>(dir)];
	}
	
	bool& mainPath() {
		return m_main_path;
	}
	
	int& order() {
		return m_order;
	}
	
	bool& visited() {
		return m_visited;
	}
	
	int numOutPaths() {
		int count = 0;
		for (int d = 0; d != static_cast<int>(MazeDir::ALL); ++d) {
			if (dir(static_cast<MazeDir>(d))) {
				count += 1;
			}
		}
		return count;
	}
	
private:
	
	std::array<bool, static_cast<int>(MazeDir::ALL)> m_dirs;
	bool m_main_path;
	int m_order;
	bool m_visited;
};

class MazeGenerator {
public:
	MazeGenerator() {}
	
	MazeGenerator(int len_x, int len_y, int start_x = 0, int start_y = 0) {
		generate(len_x, len_y, start_x, start_y);
	}
	
	void generate(int len_x, int len_y, int start_x = 0, int start_y = 0, bool simple = true);
	
	MazeNode& node(int x, int y) {
		return node(MazeIndex(x, y));
	}
	
	MazeNode& node(MazeIndex ind) {
		return m_nodes.at(ind.x() + ind.y() * m_len_x);
	}
	
	bool inside(MazeIndex ind) const {
		return ind.inside(m_len_x, m_len_y);
	}
	
	void setPosition(int x, int y, bool simple = true) {
		assert(0 <= x && x < m_len_x);
		assert(0 <= y && y < m_len_y);
		if (x != m_last_pos_x || y != m_last_pos_y) {
			genHeights(x, y, simple);
		}
		
		m_last_pos_x = x;
		m_last_pos_y = y;
	}
	
	float& height(int x, int y) {
		return height(MazeIndex(x, y));
	}
	
	float& height(MazeIndex mn) {
		return m_heights.at(mn.x() + mn.y() * m_len_x);
	}
	
	MazeIndex getStart() const {
		return MazeIndex(m_start_x, m_start_y);
	}
	
	MazeIndex getLen() const {
		return MazeIndex(m_len_x, m_len_y);
	}
	
	MazeIndex getEnd() const {
		return MazeIndex(m_end_x, m_end_y);
	}
	
	void dump(bool path_only = false);
private:
	
	void classicDFS();
	void findMainPath();
	void fillFalseCycles();
	void closeFalceCycles();
	void addRandomPaths();
	void addReturnPaths();
	
	void genHeights(int x, int y, bool simple = true);
	
	std::vector<MazeNode> m_nodes;
	int m_len_x = 0, m_len_y = 0;
	int m_start_x = 0, m_start_y = 0;
	int m_end_x = 0, m_end_y = 0;
	
	int m_last_pos_x = 0, m_last_pos_y = 0;
	
	std::vector<float> m_heights;
};

#endif
