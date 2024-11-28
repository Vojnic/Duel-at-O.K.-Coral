#include "Background.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include "player.h"
#include <chrono>
#include <thread>

#define FRAME_DURATION 0.01666666666f
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
	while (!glfwWindowShouldClose(window)) {
		auto frameStart = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(frameStart.time_since_epoch());
		std::cout << "Frame Start Time: " << duration.count() << " ms" << std::endl;


		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();

		// Check if 'A' key is pressed
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && player1.getState() != Player::State::Dead) {
			player1.setState(Player::State::Ready); // Set player1's state to READY when 'A' is pressed
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && player1.getState() != Player::State::Dead) {
			if (player1.getState() == Player::State::Ready)
			{
				player2.setState(Player::State::Dead); // Set player2's state to DEAD when 'S' is pressed
			}
		}
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && player2.getState() != Player::State::Dead) {
			player2.setState(Player::State::Ready); // Set player2's state to READY when 'K' is pressed
		}
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && player2.getState() != Player::State::Dead) {
			if (player2.getState() == Player::State::Ready)
			{
				player1.setState(Player::State::Dead); // Set player1's state to DEAD when 'L' is pressed
			}
		}

		background.render();

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