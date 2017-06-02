#include "maze_generator.h"

#include <stack>
#include <iostream>
#include <iomanip>
#include <random>
#include <algorithm>

#include "random.h"

MazeDir opposite(MazeDir dir) {
	switch (dir) {
		case MazeDir::UP: return MazeDir::DOWN;
		case MazeDir::DOWN: return MazeDir::UP;
		case MazeDir::LEFT: return MazeDir::RIGHT;
		case MazeDir::RIGHT: return MazeDir::LEFT;
		default: abort();
	}
}

void MazeGenerator::generate(int len_x, int len_y, int start_x, int start_y) {
	m_len_x = len_x;
	m_len_y = len_y;
	
	m_start_x = start_x;
	m_start_y = start_y;
	
	m_nodes.clear();
	m_nodes.resize(m_len_x * m_len_y);
	
	// create classic dfs maze
	classicDFS();
	
	//dump();
	
	findMainPath();
	
	//dump(true);
	
	fillFalseCycles();
	
	//dump();
	
	closeFalceCycles();
	
	//dump();
	addRandomPaths();
	
	m_last_pos_x = m_start_x;
	m_last_pos_y = m_start_y;
	genHeights(m_start_x, m_start_y);
}

void MazeGenerator::classicDFS() {
	std::stack<MazeIndex> dfs_stack;
	std::vector<MazeDir> next;
	
	dfs_stack.push(MazeIndex(m_start_x, m_start_y));
	node(MazeIndex(m_start_x, m_start_y)).order() = 1;

	while(!dfs_stack.empty()) {
		MazeIndex cur = dfs_stack.top();
		
		node(cur).visited() = true;
		
		next.clear();
		for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
			MazeIndex neigh_ind = cur.neigh(static_cast<MazeDir>(dir));
			if (inside(neigh_ind) && !node(neigh_ind).visited()) {
				next.push_back(static_cast<MazeDir>(dir));
			}
		}

		if (next.size() > 0) {
			MazeDir direction = next.at(Random::getInt(0, next.size()-1));
			MazeIndex next_node = cur.neigh(direction);
			node(cur).dir(direction) = true;
			node(next_node).order() = node(cur).order() + 1;
			dfs_stack.push(next_node);
		} else {
			dfs_stack.pop();
		}
		
	}
}

void MazeGenerator::findMainPath() {
	// find exit position (longest path)
	auto comp = [](MazeNode& a, MazeNode& b){ return a.order() < b.order(); };
	auto me = std::max_element(m_nodes.begin(), m_nodes.end(), comp);
	int exit_pos = std::distance(m_nodes.begin(), me);
	m_end_x = exit_pos % m_len_x;
	m_end_y = exit_pos / m_len_x;
	
	bool go_next = false;
	// find path from exit to start
	MazeIndex cur(m_end_x, m_end_y);
	node(cur).mainPath() = true;
	while (cur.x() != m_start_x || cur.y() != m_start_y) {
		go_next = false;
		for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
			MazeDir out_dir = static_cast<MazeDir>(dir);
			MazeDir in_dir = opposite(out_dir);
			MazeIndex neigh_ind = cur.neigh(out_dir);
			if (inside(neigh_ind) && node(neigh_ind).dir(in_dir)) {
				cur = neigh_ind;
				go_next = true;
			}
			if (go_next) break;
		}
		node(cur).mainPath() = true;
	}
}

void MazeGenerator::dump(bool path_only) {
	const char arrow_left[] = "\u02c2";
	const char arrow_right[] = "\u02c3";
	const char arrow_up[] = "\u02c4";
	const char arrow_down[] = "\u02c5";
	
	const int num_spaces = 4;
	const char spaces[num_spaces+1] = "    ";
	
	std::cout << "Start: " << m_start_x << ", " << m_start_y << std::endl;
	
	for (int j = m_len_y - 1; j >= 0; j--) {
		std::cout << " ";
		for (int i = 0; i < m_len_x; i++) {
			if (j != m_len_y - 1) {
				if (node(i, j).dir(MazeDir::UP))
					std::cout << arrow_up;
				else
					std::cout << " ";
				std::cout << spaces;
			}
		}
		std::cout<<std::endl;
		for (int i = 0; i < m_len_x; i++) {
			if (i != 0) {
				if (node(i, j).dir(MazeDir::LEFT))
					std::cout << arrow_left;
				else
					std::cout << " ";
			}
			
			std::cout << std::setw(num_spaces-1);
			if (path_only && !node(i, j).mainPath()) {
				std::cout << -1;
			} else {
				std::cout << node(i, j).order();
			}
			
			if (i != m_len_x - 1) {
				if (node(i, j).dir(MazeDir::RIGHT))
					std::cout << arrow_right;
				else
					std::cout << " ";
			}
		}
		std::cout << std::endl;
		std::cout << " ";
		for (int i = 0; i < m_len_x; i++) {
			if (j != 0) {
				if (node(i, j).dir(MazeDir::DOWN))
					std::cout << arrow_down;
				else
					std::cout << " ";
				std::cout << spaces;
			}
		}
		std::cout<<std::endl;
  }
  
  std::cout << "End: " << m_end_x << ", " << m_end_y << std::endl;
}

void MazeGenerator::fillFalseCycles()
{
	MazeIndex cur(m_start_x, m_start_y);
	while (cur.x() != m_end_x || cur.y() != m_end_y) {
		if (node(cur).mainPath()) { // we on the main path
			bool go_next = false;
			// process all false paths
			for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
				MazeDir out_dir = static_cast<MazeDir>(dir);
				// if path exists, from main path to false path, and orders are not equal
				if (node(cur).dir(out_dir) && !node(cur.neigh(out_dir)).mainPath() && node(cur.neigh(out_dir)).order() != node(cur).order()) {
					node(cur.neigh(out_dir)).order() = node(cur).order();
					cur = cur.neigh(out_dir);
					go_next = true;
				}
				if (go_next) break;
			}
			// process main path
			if (!go_next) {
				// find main path
				for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
					MazeDir out_dir = static_cast<MazeDir>(dir);
					// if path exists, from main path to main path
					if (node(cur).dir(out_dir) && node(cur.neigh(out_dir)).mainPath()) {
						cur = cur.neigh(out_dir);
						go_next = true;
					}
					if (go_next) break;
				}
			}
		} else { // we on the false path
			bool go_next = false;
			// process all existing paths from current node, they are false by construction
			for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
				MazeDir out_dir = static_cast<MazeDir>(dir);
				// if path exists and orders are not equal
				if (node(cur).dir(out_dir) && node(cur.neigh(out_dir)).order() != node(cur).order()) {
					node(cur.neigh(out_dir)).order() = node(cur).order();
					cur = cur.neigh(out_dir);
					go_next = true;
				}
				if (go_next) break;
			}
			// go back if there are no out paths
			if (!go_next) {
				for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
					MazeDir out_dir = static_cast<MazeDir>(dir);
					MazeDir in_dir = opposite(out_dir);
					if (inside(cur.neigh(out_dir)) && node(cur.neigh(out_dir)).dir(in_dir)) {
						cur = cur.neigh(out_dir);
						go_next = true;
					}
					if (go_next) break;
				}
			}
		}
	}
}

void MazeGenerator::closeFalceCycles()
{	
	for (int j = 0; j < m_len_y; ++j) {
		for (int i = 0; i < m_len_x; ++i) {	
			MazeIndex cur(i, j);
			// if we have dead end
			if (node(cur).numOutPaths() == 0 && !node(cur).mainPath()) {
				// clear visited status
				for (int j2 = 0; j2 < m_len_y; ++j2) {
					for (int i2 = 0; i2 < m_len_x; ++i2) {
						node(i2, j2).visited() = false;
					}
				}
				
				// start dfs from dead end
				bool stop_dfs = false;
				
				std::stack<MazeIndex> dfs_stack;
				std::vector<MazeDir> next;
				
				dfs_stack.push(MazeIndex(i, j));
				
				while(!dfs_stack.empty()) {
					MazeIndex cur = dfs_stack.top();
					
					node(cur).visited() = true;
					
					// try to find neighbour with lesser order
					for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
						MazeDir neigh_dir = static_cast<MazeDir>(dir);
						MazeIndex neigh_ind = cur.neigh(neigh_dir);
						if (inside(neigh_ind) && node(neigh_ind).order() < node(cur).order())
						{
							// create path and stop this dfs
							node(cur).dir(neigh_dir) = true;
							stop_dfs = true;
							break;
						}
					}
					if (stop_dfs) break;
					
					// try to find neighbour with equal order on main path
					for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
						MazeDir neigh_dir = static_cast<MazeDir>(dir);
						MazeIndex neigh_ind = cur.neigh(neigh_dir);
						if (inside(neigh_ind) && node(neigh_ind).order() == node(cur).order() && node(neigh_ind).mainPath())
						{
							// create path and stop this dfs
							node(cur).dir(neigh_dir) = true;
							stop_dfs = true;
							break;
						}
					}
					if (stop_dfs) break;
					
					// find all nodes with the same order
					next.clear();
					for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
						MazeIndex neigh_ind = cur.neigh(static_cast<MazeDir>(dir));
						if (inside(neigh_ind) && !node(neigh_ind).visited() && node(neigh_ind).order() == node(cur).order()) {
							next.push_back(static_cast<MazeDir>(dir));
						}
					}

					if (next.size() > 0) {
						MazeDir direction = next.at(Random::getInt(0, next.size()-1));
						MazeIndex next_node = cur.neigh(direction);
						node(cur).dir(direction) = true;
						dfs_stack.push(next_node);
					} else {
						dfs_stack.pop();
					}
				}
				
				
			}
		}
	}
}

void MazeGenerator::addRandomPaths()
{
	float rate = 0.5;
	for (int i = 0; i < m_len_x; ++i) {
		for (int j = 0; j < m_len_y; ++j) {
			for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
				if (inside(MazeIndex(i, j).neigh(static_cast<MazeDir>(dir))) && Random::getFloat(0, 1) < rate) {
					node(MazeIndex(i, j)).dir(static_cast<MazeDir>(dir)) = true;
				}
			}
		}
	}
}

void MazeGenerator::genHeights(int x, int y)
{
	m_heights.resize(m_len_x * m_len_y);
	
	MazeIndex cur(x, y);
	
	float min_height = 0.1;
	float max_height = 0.9;
	
	for (int i = 0; i < m_len_x; ++i) {
		for (int j = 0; j < m_len_y; ++j) {
			height(i, j) = Random::getInt(1, 9) * 0.1;
		}
	}
	
	//generate random height for current node
	height(cur) = Random::getInt(3, 7) * 0.1;

	for (int dir = 0; dir != static_cast<int>(MazeDir::ALL); ++dir) {
		MazeDir neigh_dir = static_cast<MazeDir>(dir);
		if (inside(cur.neigh(neigh_dir))) {
			bool path_exists = node(cur).dir(neigh_dir);
			int min = min_height * 10;
			int max = max_height * 10;
			if (path_exists) {
				max = height(cur) * 10;
			} else {
				min = height(cur) * 10 + 1;
			}
			height(cur.neigh(neigh_dir)) = Random::getInt(min, max) * 0.1;
		}
	}

}
