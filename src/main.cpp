#include "Background.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <SFML/Audio.hpp>
#include <iostream>
#include "player.h"
#include <chrono>
#include <thread>
#include "muzzleflash.h"


#define FRAME_DURATION 0.00833333333333f

sf::SoundBuffer shootBuffer;
sf::Sound shootSound;


int main() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return -1;
	}

	if (!shootBuffer.loadFromFile("Resources/shoot.mp3")) {
		std::cerr << "Failed to load shoot sound!" << std::endl;
		return -1;
	}

	shootSound.setBuffer(shootBuffer);
	shootSound.setVolume(50.f);

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
		0.1f, -0.1f, 0.0f, 0.0f, 0.0f,
		0.1f, -1.f, 0.0f, 0.0f, 1.0f,
		1.f, -1.f, 0.0f, 1.0f, 1.0f,
		1.f, -0.1f, 0.0f, 1.0f, 0.0f
	};
	Player player2("Resources/player2_idle.png",
		"Resources/player2_ready.png",
		"Resources/player2_dead.png",
		player2Vertices);

	GLfloat player1MuzzleVertices[] = {
	-0.32f, -0.4f, 0.0f, 0.0f, 0.0f,
	-0.32f, -0.3f, 0.0f, 0.0f, 1.0f,
	-0.12f, -0.3f, 0.0f, 1.0f, 1.0f,
	-0.12f, -0.4f, 0.0f, 1.0f, 0.0f
	};
	MuzzleFlash player1Muzzle(player1MuzzleVertices);

	GLfloat player2MuzzleVertices[] = {
	0.32f, -0.4f, 0.0f, 0.0f, 0.0f,
	0.32f, -0.3f, 0.0f, 0.0f, 1.0f,
	0.12f, -0.3f, 0.0f, 1.0f, 1.0f,
	0.12f, -0.4f, 0.0f, 1.0f, 0.0f
	};
	MuzzleFlash player2Muzzle(player2MuzzleVertices);

	bool player1ReadyInProgress = false;
	auto player1ReadyTime = std::chrono::steady_clock::time_point::min();

	bool player2ReadyInProgress = false;
	auto player2ReadyTime = std::chrono::steady_clock::time_point::min();

	bool player1DeathPending = false;
	auto player1DeathTime = std::chrono::steady_clock::time_point::min();

	bool player2DeathPending = false;
	auto player2DeathTime = std::chrono::steady_clock::time_point::min();

	bool player1MuzzleVisible = false;
	auto player1MuzzleTime = std::chrono::steady_clock::time_point::min();

	bool player2MuzzleVisible = false;
	auto player2MuzzleTime = std::chrono::steady_clock::time_point::min();

	const std::chrono::milliseconds MUZZLE_FLASH_DURATION(150);
	bool inputDisabled = false;

	while (!glfwWindowShouldClose(window)) {
		auto frameStart = std::chrono::high_resolution_clock::now();

		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();

		background.render();

		if (std::chrono::steady_clock::now() < player1MuzzleTime + MUZZLE_FLASH_DURATION) {
			player1Muzzle.setShow(true);
		}
		else {
			player1Muzzle.setShow(false);
		}

		if (std::chrono::steady_clock::now() < player2MuzzleTime + MUZZLE_FLASH_DURATION) {
			player2Muzzle.setShow(true);
		}
		else {
			player2Muzzle.setShow(false);
		}

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
				shootSound.play();

				player1MuzzleTime = std::chrono::steady_clock::now();

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
				shootSound.play();

				player2MuzzleTime = std::chrono::steady_clock::now();
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
		player1Muzzle.render();
		player2.render();
		player2Muzzle.render();

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