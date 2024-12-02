#include "Background.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <SFML/Audio.hpp>
#include "muzzleflash.h"
#include "player.h"
#include "SoundIndicator.h"
#include "GameOverlay.h"
#include "button.h"
#include "TextRenderer.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#define FRAME_DURATION 0.00833333333333f

sf::SoundBuffer shootBuffer;
sf::Sound shootSound;
std::chrono::steady_clock::time_point bot2ShootTime;

bool botIsReady = false;
void botShoot(Player& player1, Player& player2, bool& inputDisabled, sf::Sound& shootSound) {
	if (player2.getState() == Player::State::Idle) {
		player2.setState(Player::State::Ready);
		bot2ShootTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(500 + rand() % 1000);
		botIsReady = true;
		std::cout << "Bot is ready" << std::endl;

	}
	if(botIsReady && std::chrono::steady_clock::now() >= bot2ShootTime){
		player1.setState(Player::State::Dead);
		player2.addPoints();
		botIsReady = false;
		inputDisabled = true;
		shootSound.play();
		std::cout << "player1 points: " << player1.getPoints() << "\tplayer2 points: " << player2.getPoints() << std::endl;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return -1;
	}
	if (!shootBuffer.loadFromFile("Resources/shoot.mp3")) {
		std::cerr << "Failed to load shoot sound!" << std::endl;
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW!" << std::endl;
		return -1;
	}

	TextRenderer textRenderer("Resources/Roboto-Medium.ttf", "text.vert", "text.frag", 50);


	Background initialbackground("Resources/initial_background.jpg");
	bool isSinglePlayer = false;
	Button spButton("Resources/singleplayer.png", -0.5f, 0.25f, 1.f, 0.2f);
	Button mpButton("Resources/multiplayer.png", -0.5f, -0.25f, 1.f, 0.2f);
	Button retryButton("Resources/retry_button.png", -0.66f, 0.65f, .44f, 0.3f);
	Button quitButton("Resources/quit_button.png", 0.22f, 0.65f, .44f, 0.3f);


	shootSound.setBuffer(shootBuffer);
	shootSound.setVolume(50.f);

	Background background("Resources/wildwestbackground.jpg");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLfloat player1Vertices[] = {
		-1.f, -0.1f, 0.0f, 0.0f, 0.0f,
		-1.f, -1.f, 0.0f, 0.0f, 1.0f,
		-0.1f, -1.f, 0.0f, 1.0f, 1.0f,
		-0.1f, -0.1f, 0.0f, 1.0f, 0.0f
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




	bool inputDisabled = true;

	bool entryScreenActive = true;

	const std::chrono::milliseconds MUZZLE_FLASH_DURATION(150);


	SoundIndicator soundIndicator("Resources/fake_indicator.wav", "Resources/real_indicator.wav");

	bool waitingForSound = false;
	auto soundTriggerTime = std::chrono::steady_clock::now() + std::chrono::seconds(3);


	bool arePlayersReady = false;
	auto startTime = std::chrono::steady_clock::now();
	auto endTime = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(window)) {
		auto frameStart = std::chrono::high_resolution_clock::now();
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
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
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		float x = (2.0f * xpos) / width - 1.0f;
		float y = 1.0f - (2.0f * ypos) / height;



		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			if (entryScreenActive) {

				// Check if Start button was clicked
				if (spButton.isClicked(x, y)) {
					std::cout << "Start Button Clicked!" << std::endl;
					isSinglePlayer = true;
					entryScreenActive = false;
				}
				// Check if Quit button was clicked
				else if (mpButton.isClicked(x, y)) {
					std::cout << "Quit Button Clicked!" << std::endl;
					entryScreenActive = false;
				}
			}
			else if (player1.getState() == Player::State::Dead || player2.getState() == Player::State::Dead) {
					if (retryButton.isClicked(x, y)) {
						player1.setState(Player::State::Idle);
						player2.setState(Player::State::Idle);
						soundIndicator.setLastIndicatorReal(false);
					}
					else if (quitButton.isClicked(x, y)) {
					glfwSetWindowShouldClose(window, true);
				}
			}
		}
		if (!entryScreenActive) {
			if (!waitingForSound && player1.getState() == Player::State::Idle && player2.getState() == Player::State::Idle &&
				std::chrono::steady_clock::now() >= soundTriggerTime && !soundIndicator.wasLastIndicatorReal()) {
				waitingForSound = true;
				arePlayersReady = soundIndicator.playRandomIndicator(inputDisabled);
				if (arePlayersReady) {
					startTime = std::chrono::steady_clock::now();
				}
				soundTriggerTime = std::chrono::steady_clock::now() + std::chrono::seconds(2 + rand() % 2);
			}
			else if (waitingForSound && soundIndicator.isSoundFinished()) {
				waitingForSound = false;
			}
			if ((player1.getState() == Player::State::Dead || player2.getState() == Player::State::Dead) && !waitingForSound) {
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
					if (retryButton.isClicked(x, y)) {
						player1.setState(Player::State::Idle);
						player2.setState(Player::State::Idle);
						player1ReadyInProgress = false;
						player2ReadyInProgress = false;
						inputDisabled = true;
						waitingForSound = false;
						arePlayersReady = false;
						soundIndicator.setLastIndicatorReal(false);
						botIsReady = false;

						soundTriggerTime = std::chrono::steady_clock::now() + std::chrono::seconds(3);
					}
					else if (quitButton.isClicked(x, y)) {
						glfwSetWindowShouldClose(window, true);
					}
				}
			}
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
				
				endTime = std::chrono::steady_clock::now();
				std::chrono::duration<float> timeDifference = endTime - startTime;
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeDifference);
				std::cout << "Time difference: " << duration.count() << "ms" << std::endl;
			}
			if (isSinglePlayer) {
				botShoot(player1, player2, inputDisabled, shootSound);
				player2MuzzleTime = std::chrono::steady_clock::now();
				endTime = std::chrono::steady_clock::now();
				std::chrono::duration<float> timeDifference = endTime - startTime;
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeDifference);
			}
			if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && player2.getState() == Player::State::Idle && !player2ReadyInProgress && !isSinglePlayer) {
				player2ReadyTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(125);
				player2ReadyInProgress = true;
			}
			if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && player2.getState() == Player::State::Ready && player1.getState() != Player::State::Dead && !player1DeathPending && !isSinglePlayer) {
				player1DeathTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(350);
				player1DeathPending = true;
				player2.addPoints();
				inputDisabled = true;
				shootSound.play();

				player2MuzzleTime = std::chrono::steady_clock::now();
				std::cout << "player1 points: " << player1.getPoints() << "\tplayer2 points: " << player2.getPoints() << std::endl;

				endTime = std::chrono::steady_clock::now();
				std::chrono::duration<float> timeDifference = endTime - startTime;
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeDifference);
				std::cout << "Time difference: " << duration.count() << "ms" << std::endl;
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
		}

		if (player1DeathPending && std::chrono::steady_clock::now() >= player1DeathTime) {
			player1.setState(Player::State::Dead);
			player1DeathPending = false;
		}
		if (entryScreenActive) {
			initialbackground.render();
			spButton.render();
			mpButton.render();
		}
		else {
			background.render();
			player1.render();
			player1Muzzle.render();
			player2.render();
			player2Muzzle.render();
			int player1Score = player1.getPoints();
			int player2Score = player2.getPoints();

			if (player1.getState() == Player::State::Dead || player2.getState() == Player::State::Dead) {
				retryButton.render();
				quitButton.render();

				std::chrono::duration<float> timeSinceGameStart = endTime - startTime;
				auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceGameStart).count();
				if (player1.getState() == Player::State::Dead) {
					if (isSinglePlayer)
					{
						std::string timeString = "Bot shot in: " + std::to_string(timeMs) + "ms";
						glm::vec3 timeColor(1.0f, 1.0f, 1.0f);
						textRenderer.RenderText(timeString, 300.0f, 400.0f, 0.7f, timeColor);
					}
					else {
						std::string timeString = "Player2 shot in: " + std::to_string(timeMs) + "ms";
						glm::vec3 timeColor(1.0f, 1.0f, 1.0f);
						textRenderer.RenderText(timeString, 300.0f, 400.0f, 0.7f, timeColor);
					}
				}
				else {
					std::string timeString = "Player1 shot in : " + std::to_string(timeMs) + "ms";
					glm::vec3 timeColor(1.0f, 1.0f, 1.0f);
					textRenderer.RenderText(timeString, 100.0f, 400.0f, 0.7f, timeColor);
				}
			}

			glm::vec3 scoreColor(1.0f, 1.0f, 1.0f); // White color, adjust as needed

			textRenderer.RenderText(std::to_string(player1Score), 10.0f, 760.0f, 1.0f, scoreColor);

			textRenderer.RenderText(std::to_string(player2Score), 700.0f, 760.0f, 1.0f, scoreColor);
		}

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