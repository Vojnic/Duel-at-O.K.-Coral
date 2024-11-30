#include "Background.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include "player.h"
#include <chrono>
#include <thread>


#define FRAME_DURATION 0.00833333333333f
int main() {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1400, 940, "Western Duel", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW!" << std::endl;
		return -1;
	}

	// Create background instance with your western background image
	Background background("Resources/wildwestbackground.jpg");


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLfloat player1Vertices[] = {
		-1.f, -0.1f, 0.0f, 0.0f, 0.0f,  // Top-left
		-1.f, -1.f, 0.0f, 0.0f, 1.0f,  // Bottom-left
		-0.1f, -1.f, 0.0f, 1.0f, 1.0f,  // Bottom-right
		-0.1f, -0.1f, 0.0f, 1.0f, 0.0f   // Top-right
	};
	Player player1("Resources/player1_idle.png",
		"Resources/player1_ready.png",
		"Resources/player1_dead.png",
		player1Vertices);

	GLfloat player2Vertices[] = {
		0.1f, -0.1f, 0.0f, 0.0f, 0.0f,  // Top-left 
		0.1f, -1.f, 0.0f, 0.0f, 1.0f,  // Bottom-left
		1.f, -1.f, 0.0f, 1.0f, 1.0f,  // Bottom-right
		1.f, -0.1f, 0.0f, 1.0f, 0.0f   // Top-right 
	};
	Player player2("Resources/player2_idle.png",
		"Resources/player2_ready.png",
		"Resources/player2_dead.png",
		player2Vertices);

	// Variables to manage delayed transitions
	bool player1ReadyInProgress = false;
	bool player2ReadyInProgress = false;

	bool player1DeathPending = false;
	bool player2DeathPending = false;

	auto player1ReadyTime = std::chrono::steady_clock::time_point::min();
	auto player2ReadyTime = std::chrono::steady_clock::time_point::min();

	auto player1DeathTime = std::chrono::steady_clock::time_point::min();
	auto player2DeathTime = std::chrono::steady_clock::time_point::min();

	bool inputDisabled = false; // Disable input after a player shoots

	while (!glfwWindowShouldClose(window)) {
		auto frameStart = std::chrono::high_resolution_clock::now();

		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();

		background.render();

		if (!inputDisabled) {
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && player1.getState() == Player::State::Idle && !player1ReadyInProgress) {
				player1ReadyTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(125);
				player1ReadyInProgress = true;
			}
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && player1.getState() == Player::State::Ready && player2.getState() != Player::State::Dead && !player2DeathPending) {
				player2DeathTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(350);
				player2DeathPending = true;
				player1.addPoints();
				inputDisabled = true;
				std::cout << "player1 points: " << player1.getPoints() << "\tplayer2 points: " << player2.getPoints() << std::endl;
			}

			if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && player2.getState() == Player::State::Idle && !player2ReadyInProgress) {
				player2ReadyTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(125);
				player2ReadyInProgress = true;
			}
			if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && player2.getState() == Player::State::Ready && player1.getState() != Player::State::Dead && !player1DeathPending) {
				player1DeathTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(350);
				player1DeathPending = true;
				player2.addPoints();
				inputDisabled = true;
				std::cout << "player1 points: " << player1.getPoints() << "\tplayer2 points: " << player2.getPoints() << std::endl;
			}
		}

		if (player1ReadyInProgress && std::chrono::steady_clock::now() >= player1ReadyTime) {
			player1.setState(Player::State::Ready);
			player1ReadyInProgress = false;
		}

		if (player2ReadyInProgress && std::chrono::steady_clock::now() >= player2ReadyTime) {
			player2.setState(Player::State::Ready);
			player2ReadyInProgress = false;
		}

		if (player2DeathPending && std::chrono::steady_clock::now() >= player2DeathTime) {
			player2.setState(Player::State::Dead);
			player2DeathPending = false;
			inputDisabled = false;
		}

		if (player1DeathPending && std::chrono::steady_clock::now() >= player1DeathTime) {
			player1.setState(Player::State::Dead);
			player1DeathPending = false;
			inputDisabled = false;
		}

		player1.render();
		player2.render();

		glfwSwapBuffers(window);
		auto frameEnd = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> frameDuration = frameEnd - frameStart;
		if (frameDuration.count() < FRAME_DURATION) {
			std::this_thread::sleep_for(std::chrono::duration<float>(FRAME_DURATION - frameDuration.count()));
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}