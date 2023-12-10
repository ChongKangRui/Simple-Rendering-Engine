#pragma once
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include <string>

namespace TextureUtils
{
	unsigned int loadCubemap(std::vector<std::string> faces)
	{
		printf("Loading cubemap texture...\n");
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	unsigned int loadTextureRepeat(const std::string& path)
	{
		unsigned int texture_object;

		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glGenTextures(1, &texture_object);
			glBindTexture(GL_TEXTURE_2D, texture_object);

			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Basic support to detect image format with alpha channel.
			// jpg and bmp has no alpha channel, thus nrChannels will be 3.
			// png MAY have alpha channel; if present, nrChannels will be 4.
			GLint internalFormat = nrChannels == 3 ? GL_RGB : GL_RGBA;
			GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;

			// Pass the image data to GPU
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			printf("Failed to load texture: %s\n", path.c_str());
			texture_object = 0;
		}

		// Free memory after we sent the data to GPU.
		stbi_image_free(data);

		return texture_object;
	}
	unsigned int loadTexture(const std::string& path)
	{
		unsigned int texture_object;

		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glGenTextures(1, &texture_object);
			glBindTexture(GL_TEXTURE_2D, texture_object);

			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Basic support to detect image format with alpha channel.
			// jpg and bmp has no alpha channel, thus nrChannels will be 3.
			// png MAY have alpha channel; if present, nrChannels will be 4.
			GLint internalFormat = nrChannels == 3 ? GL_RGB : GL_RGBA;
			GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;

			// Pass the image data to GPU
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			printf("Failed to load texture: %s\n", path.c_str());
			texture_object = 0;
		}

		// Free memory after we sent the data to GPU.
		stbi_image_free(data);

		return texture_object;
	}
}