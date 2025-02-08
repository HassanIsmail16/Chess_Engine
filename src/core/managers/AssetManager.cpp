#include "AssetManager.h"
#include "../log/logger.hpp"

AssetManager& AssetManager::getInstance() {
	static AssetManager instance;
	return instance;
}

void AssetManager::loadAssets(const std::string& directory_name) {
	if (!std::filesystem::exists(directory_name)) {
		LOG_ERROR("Directory " + directory_name + " doesn't exist, failed to load assets");
		return;
	}

	for (const auto& file : std::filesystem::directory_iterator(directory_name)) {
		if (file.is_regular_file()) {
			// Extract file information
			std::string filepath = file.path().string();
			std::string filename = file.path().stem().string();
			std::string extension = file.path().extension().string();

			// Load assets according to extension
			if (isImage(extension)) {
				loadTexture(filename, filepath);
			}
			else if (isFont(extension)) {
				loadFont(filename, filepath);
			}
			else if (isAudio(extension)) {
				loadSound(filename, filepath);
			}
		}
	}
}


void AssetManager::loadTexture(const std::string& texture_name, const std::string& filename) {
	auto texture = std::make_shared<sf::Texture>();
	if (texture->loadFromFile(filename)) {
		textures[texture_name] = texture;
		LOG_INFO("Texture loaded successfully: ", texture_name, " from ", filename);
	}
	else {
		LOG_ERROR("Failed to load texture: ", filename);
	}
}

void AssetManager::loadFont(const std::string& font_name, const std::string& filename) {
	auto font = std::make_shared<sf::Font>();
	if (font->loadFromFile(filename)) {
		fonts[font_name] = font;
		LOG_INFO("Font loaded successfully: ", font_name, " from ", filename);
	}
	else {
		LOG_ERROR("Failed to load font: ", filename);
	}
}

void AssetManager::loadSound(const std::string& sound_name, const std::string& filename) {
	auto sound_buffer = std::make_shared<sf::SoundBuffer>();
	if (sound_buffer->loadFromFile(filename)) {
		sound_buffers[sound_name] = sound_buffer;
		LOG_INFO("Sound loaded successfully: ", sound_name, " from ", filename);
	}
	else {
		LOG_ERROR("Failed to load sound: ", filename);
	}
}

AssetManager::AssetManager() {}

sf::Sprite AssetManager::getSprite(const std::string& texture_name) {
	// Create sprite from the texture
	sf::Sprite sprite;
	try {
		sprite.setTexture(*textures.at(texture_name));
	}
	catch (const std::exception& e) {
		LOG_ERROR("Failed to retrieve texture: ", texture_name, " from AssetManager");
	}
	return sprite;
}

sf::Font AssetManager::getFont(const std::string& font_name) {
	try {
		return *fonts.at(font_name);
	}
	catch (const std::exception& e) {
		LOG_ERROR("Failed to retrieve font: ", font_name, " from AssetManager");
		return sf::Font();
	}
}

sf::Sound AssetManager::getSound(const std::string& sound_name) {
	sf::Sound sound;
	try {
		sound.setBuffer(*sound_buffers.at(sound_name));
	}
	catch (const std::exception& e) {
		LOG_ERROR("Failed to retrieve sound buffer: ", sound_name, "from AssetManager");
	}
	return sound;
}

bool AssetManager::isImage(const std::string& extension) {
	return image_extensions.count(extension);
}

bool AssetManager::isFont(const std::string& extension) {
	return font_extensions.count(extension);
}

bool AssetManager::isAudio(const std::string& extension) {
	return audio_extensions.count(extension);
}