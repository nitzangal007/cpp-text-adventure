#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>

#include "Board.h"
#include "utils.h"
#include "Point.h"
#include "Steps.h"
#include "Results.h"

// TODO: Better put this is a proper class
void getAllBoardFileNames(std::vector<std::string>& vec_to_fill) {
	namespace fs = std::filesystem;
	for (const auto& entry : fs::directory_iterator(fs::current_path())) {
		auto filename = entry.path().filename();
		auto filenameStr = filename.string();
		if (filenameStr.substr(0, 5) == "board" && filename.extension() == ".screen") {
			std::cout << " ^ added!!\n";
			vec_to_fill.push_back(filenameStr);
		}
	}
}

// TODO: Better put this is a proper class
void reportResultError(const std::string& message, const std::string& filename, size_t iteration) {
	system("cls");
	std::cout << "Screen " << filename << " - " << message << '\n';
	std::cout << "Iteration: " << iteration << '\n';
	std::cout << "Press any key to continue to next screens (if any)" << std::endl;
	_getch();
}

// TODO: Better put this is a proper class (as a static constexpr member)
constexpr int ESC = 27;

int main(int argc, char** argv) {
	// TODO: the entire game logic should be in a proper class
	// TODO: after moving the game logic to a proper class, the main function should be very short
	// TODO: the game logic function should be split into smaller functions and/or use helper functions
	ShowConsoleCursor(false);
	bool isLoad = argc > 1 && std::string(argv[1]) == "-load";
	bool isSilent = isLoad && argc > 2 && std::string(argv[2]) == "-silent";
	std::vector<std::string> fileNames;
	getAllBoardFileNames(fileNames);
	bool esc_pressed = false;
	bool unmatching_result_found = false;
	for (const auto& filename: fileNames) {
		if (esc_pressed) break;
		Board board;
		board.load(filename);
		board.setIsSilent(isSilent);
		board.reset();
		board.print();
		long random_seed;
		Steps steps;
		Results results;

		std::string filename_prefix = filename.substr(0, filename.find_last_of('.'));
		std::string stepsFilename = filename_prefix + ".steps";
		std::string resultsFilename = filename_prefix + ".result";

		// TODO: instead of an "if", this can go into two types of Game class, 
		// one for HumanInputGame, and one for FileInputGame
		if (isLoad) {
			steps = Steps::loadSteps(stepsFilename);
			random_seed = steps.getRandomSeed();
			results = Results::loadResults(resultsFilename);
		}
		else {
			// we regenerate the seed for each screen so we can save it as part of the screen steps
			random_seed = static_cast<long>(std::chrono::system_clock::now().time_since_epoch().count());
			steps.setRandomSeed(random_seed);
		}
		srand(random_seed);
		size_t iteration = 0; // we need iteration to be outside the loop
		for (; !board.finished(); ++iteration) {
			size_t nextBombIteration = 0;
			if (isLoad) {
				if (results.isFinishedBy(iteration)) {
					reportResultError("Results file reached finish while game hadn't!", filename, iteration);
					unmatching_result_found = true;
					break;
				}
				else {
					nextBombIteration = results.getNextBombIteration();
				}
			}
			board.drawPlayer();
			if (rand() % 100 == 0) {
				board.randomlyChangeEndPoint();
			}
			// TODO: instead of an "if", this can go into two types of Game class, 
			// one for HumanInputGame, and one for FileInputGame
			if (!isLoad) {
				if (_kbhit()) {
					char key = _getch();
					if (key == 'N') { // cheat for Next screen
						break;
					}
					else if (key == ESC) {
						esc_pressed = true;
						break;
					}
					if (board.keyPressed(key)) {
						steps.addStep(iteration, key);
					}
				}
			}
			else {
				// use steps loaded from file
				if (steps.isNextStepOnIteration(iteration)) {
					board.keyPressed(steps.popStep());
				}
			}
			Sleep(!isLoad? 50 : isSilent? 0 : 10);
			board.erasePlayer();
			bool goodMove = board.movePlayer();
			if(!goodMove) {
				// hit a bomb
				board.reset();
				board.print();
				// TODO: two types of Game class for load and save, Polymorphism etc.
				if (isLoad) {
					// check if the result is correct
					if (results.popResult() != std::pair{ iteration, Results::hitBomb }) {
						reportResultError("Results file doesn't match hit bomb event!", filename, iteration);
						unmatching_result_found = true;
						break;
					}
					nextBombIteration = results.getNextBombIteration();
				}
				else {
					// save hit bomb to the results
					results.addResult(iteration, Results::hitBomb);
				}
			}
			else if(isLoad) {
				// we didn't hit a bomb, so we check if the result is correct
				if (iteration == nextBombIteration) {
					reportResultError("Results file has a hit bomb event that didn't happen!", filename, iteration);
					unmatching_result_found = true;
					break;
				}
			}
		} // end of game loop

		// TODO: use two types of Game class etc. with polymorphism instead of an "if"
		if (isLoad) {
			// check if the result is correct, but only if we didn't find an unmatching result already
			if (!unmatching_result_found) {
				if (results.popResult() != std::pair{ iteration, Results::finished }) {
					reportResultError("Results file doesn't match finished event!", filename, iteration);
					unmatching_result_found = true;
				}
				if (results.popResult().second != Results::noResult) {
					reportResultError("Results file has additional events after finish event!", filename, iteration);
					unmatching_result_found = true;
				}
			}
		}
		else {
			results.addResult(iteration, Results::finished);
			steps.saveSteps(stepsFilename);
			results.saveResults(resultsFilename);
		}
		iteration = 0;
	}
	system("cls");
	std::cout << "!!!";
}
