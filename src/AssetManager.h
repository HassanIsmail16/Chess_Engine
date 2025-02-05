#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <filesystem>
#include <iostream>


class AssetManager {
public:
	// Prevent copying or assignment
	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;

	// Singleton Access
	static AssetManager& getInstance();

	// Load all assets from a directory
	void loadAssets(const std::string& directory_name);

	// Load a specific asset
	void loadTexture(const std::string& texture_name, const std::string& filename);
	void loadFont(const std::string& font_name, const std::string& filename);
	void loadSound(const std::string& sound_name, const std::string& filename);

	// Retrieve any asset by name
	sf::Sprite getSprite(const std::string& texture_name);
	sf::Font getFont(const std::string& font_name);
	sf::Sound getSound(const std::string& sound_name);

private:
	AssetManager();

	bool isImage(const std::string& extension);
	bool isFont(const std::string& extension);
	bool isAudio(const std::string& extension);

	const std::unordered_set<std::string> image_extensions = {".png", ".jpg", ".jpeg", ".bmp", ".tga", ".gif", ".psd"};
	const std::unordered_set<std::string> font_extensions = {".ttf", ".otf", ".fnt"};
	const std::unordered_set<std::string> audio_extensions = {".wav", ".ogg", ".flac", ".aiff", ".au"};

	std::unordered_map<std::string, sf::Texture> textures;
	std::unordered_map<std::string, sf::Font> fonts;
	std::unordered_map<std::string, sf::SoundBuffer> sound_buffers;
};