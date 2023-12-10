#include "mesh_utils.h"
#include <glad/glad.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>
#include <iostream>

void SimpleMesh::draw()
{
	if (VAO != 0) {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, indices);
	}
	else {
		std::cout << "Mesh not set!" << std::endl;
	}

	glBindVertexArray(0);
}

SimpleMesh* MeshUtils::makePlane(glm::vec2 size, glm::ivec2 partitions, glm::ivec2 tiling)
{
	if (partitions.x == 0) partitions.x = 1;
	if (partitions.y == 0) partitions.y = 1;
	if (size.x <= 0) size.x = 1.0f;
	if (size.y <= 0) size.y = size.x;

	float width = size.x, depth = size.y;
	float columns = partitions.x, rows = partitions.y;

	float cellW = width / columns;
	float cellD = depth / rows;

	float halfWidth = width * 0.5f, halfDepth = depth * 0.5f;

	unsigned int triCount = columns * rows * 6;

	std::vector<float> vertices(triCount * 3);
	std::vector<float> normals(triCount * 3);
	std::vector<float> texcoords(triCount * 2);

	size_t v = -1;
	float texMul_u = tiling.x;
	float texMul_v = tiling.y;

	for (int h = 0; h < rows; h++)
	{
		for (int w = 0; w < columns; w++)
		{
			float u0 = w / (float)columns;
			float v0 = h / (float)rows;
			float u1 = (w + 1) / (float)columns;
			float v1 = (h + 1) / (float)rows;
			float wx = cellW * w;
			float dy = cellD * h;
			float w1x = cellW * (w + 1);
			float h1y = cellD * (h + 1);

			unsigned int vertex1 = ++v;
			vertices[vertex1 * 3] = -halfWidth + wx;
			vertices[vertex1 * 3 + 1] = 0.0f;
			vertices[vertex1 * 3 + 2] = halfDepth - h1y;
			texcoords[vertex1 * 2] = u0 * texMul_u;
			texcoords[vertex1 * 2 + 1] = v1 * texMul_v;
			normals[vertex1 * 3] = 0;
			normals[vertex1 * 3 + 1] = 1;
			normals[vertex1 * 3 + 2] = 0;

			unsigned int vertex2 = ++v;
			vertices[vertex2 * 3] = -halfWidth + wx;
			vertices[vertex2 * 3 + 1] = 0.0f;
			vertices[vertex2 * 3 + 2] = halfDepth - dy;
			texcoords[vertex2 * 2] = u0 * texMul_u;
			texcoords[vertex2 * 2 + 1] = v0 * texMul_v;
			normals[vertex2 * 3] = 0;
			normals[vertex2 * 3 + 1] = 1;
			normals[vertex2 * 3 + 2] = 0;

			unsigned int vertex3 = ++v;
			vertices[vertex3 * 3] = -halfWidth + w1x;
			vertices[vertex3 * 3 + 1] = 0.0f;
			vertices[vertex3 * 3 + 2] = halfDepth - h1y;
			texcoords[vertex3 * 2] = u1 * texMul_u;
			texcoords[vertex3 * 2 + 1] = v1 * texMul_v;
			normals[vertex3 * 3] = 0;
			normals[vertex3 * 3 + 1] = 1;
			normals[vertex3 * 3 + 2] = 0;

			unsigned int vertex4 = ++v;
			vertices[vertex4 * 3] = vertices[vertex3 * 3];
			vertices[vertex4 * 3 + 1] = vertices[vertex3 * 3 + 1];
			vertices[vertex4 * 3 + 2] = vertices[vertex3 * 3 + 2];
			texcoords[vertex4 * 2] = texcoords[vertex3 * 2];
			texcoords[vertex4 * 2 + 1] = texcoords[vertex3 * 2 + 1];
			normals[vertex4 * 3] = 0;
			normals[vertex4 * 3 + 1] = 1;
			normals[vertex4 * 3 + 2] = 0;

			unsigned int vertex5 = ++v;
			vertices[vertex5 * 3] = vertices[vertex2 * 3];
			vertices[vertex5 * 3 + 1] = vertices[vertex2 * 3 + 1];
			vertices[vertex5 * 3 + 2] = vertices[vertex2 * 3 + 2];
			texcoords[vertex5 * 2] = texcoords[vertex2 * 2];
			texcoords[vertex5 * 2 + 1] = texcoords[vertex2 * 2 + 1];
			normals[vertex5 * 3] = 0;
			normals[vertex5 * 3 + 1] = 1;
			normals[vertex5 * 3 + 2] = 0;

			unsigned int vertex6 = ++v;
			vertices[vertex6 * 3] = -halfWidth + w1x;
			vertices[vertex6 * 3 + 1] = 0.0f;
			vertices[vertex6 * 3 + 2] = halfDepth - dy;
			texcoords[vertex6 * 2] = u1 * texMul_u;
			texcoords[vertex6 * 2 + 1] = v0 * texMul_v;
			normals[vertex6 * 3] = 0;
			normals[vertex6 * 3 + 1] = 1;
			normals[vertex6 * 3 + 2] = 0;
		}
	}

	size_t vCount = vertices.size(), nCount = normals.size(), tCount = texcoords.size(), float_size = sizeof(float);
	unsigned int buffer;

	SimpleMesh* out = new SimpleMesh();
	out->indices = triCount;
	glGenVertexArrays(1, &(out->VAO));
	glBindVertexArray((out->VAO));

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// We want to put all vertex data into one buffer.
	// So first, tell OpenGL to allocate enough space.
	glBufferData(GL_ARRAY_BUFFER, (vCount + nCount + tCount) * float_size, NULL, GL_STATIC_DRAW);

	// Arrange data to be layout as 111222333 instead of 123123123
	glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * float_size, vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, vCount * float_size, nCount * float_size, normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, (vCount + nCount) * float_size, tCount * float_size, texcoords.data());

	// Enforce layout where:
	// 0: position
	// 1: normal
	// 2: texcoord
	// 3: color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * float_size, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * float_size, (void*)(vCount * float_size));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * float_size, (void*)((vCount + nCount) * float_size));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	return out;
}

SimpleMesh* MeshUtils::simpleObjLoader(const std::string& filePath)
{
	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = "";
	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(filePath, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
		}

		return 0;
	}
	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	std::vector<tinyobj::real_t> vertices;
	std::vector<tinyobj::real_t> normals;
	std::vector<tinyobj::real_t> texcoords;

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;

		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

				vertices.push_back(vx);
				vertices.push_back(vy);
				vertices.push_back(vz);

				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

					normals.push_back(nx);
					normals.push_back(ny);
					normals.push_back(nz);
				}

				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

					texcoords.push_back(tx);
					texcoords.push_back(ty);
				}
			}

			index_offset += fv;
		}
	}

	size_t vCount = vertices.size(), nCount = normals.size(), tCount = texcoords.size(), float_size = sizeof(float);
	unsigned int buffer;

	SimpleMesh* out = new SimpleMesh();
	out->indices = vCount / 3;
	glGenVertexArrays(1, &(out->VAO));
	glBindVertexArray(out->VAO);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// We want to put all vertex data into one buffer.
	// So first, tell OpenGL to allocate enough space.
	glBufferData(GL_ARRAY_BUFFER, (vCount + nCount + tCount) * float_size, NULL, GL_STATIC_DRAW);

	// Arrange data to be layout as 111222333 instead of 123123123
	glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * float_size, vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, vCount * float_size, nCount * float_size, normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, (vCount + nCount) * float_size, tCount * float_size, texcoords.data());

	// Enforce layout where:
	// 0: position
	// 1: normal
	// 2: texcoord
	// 3: color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * float_size, (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * float_size, (void*)(vCount * float_size));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * float_size, (void*)((vCount + nCount) * float_size));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	return out;
}

SimpleMesh* MeshUtils::makeQuad()
{
	static std::vector<float> vertices =
	{
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f
	};

	static std::vector<float> texCoords =
	{
		0.0f,  1.0f,
		0.0f,  0.0f,
		1.0f,  0.0f,
		1.0f,  0.0f,
		1.0f,  1.0f,
		0.0f,  1.0f
	};

	size_t vCount = vertices.size(), tCount = texCoords.size(), float_size = sizeof(float);
	unsigned int buffer;

	SimpleMesh* mesh = new SimpleMesh();
	mesh->indices = vCount / 3;

	glGenVertexArrays(1, &(mesh->VAO));
	glBindVertexArray(mesh->VAO);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// We want to put all vertex data into one buffer.
	// So first, tell OpenGL to allocate enough space.
	glBufferData(GL_ARRAY_BUFFER, (vCount + tCount) * float_size, NULL, GL_STATIC_DRAW);

	// Arrange data to be layout as 111222333 instead of 123123123
	glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * float_size, vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, vCount * float_size, tCount * float_size, texCoords.data());

	// Enforce layout where:
	// 0: position
	// 1: normal
	// 2: texcoord
	// 3: color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * float_size, (void*)0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * float_size, (void*)(vCount * float_size));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	return mesh;
}

SimpleMesh* MeshUtils::makeSkybox()
{
	static std::vector<float> vertices =
	{
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	static std::vector<float> colors =
	{
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Quad 1
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// Quad 2
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// Quad 3
		0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,	// Quad 4
		1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,	// Quad 5
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,	// Quad 6
	};

	size_t vCount = vertices.size(), cCount = colors.size(), float_size = sizeof(float);
	unsigned int buffer;

	SimpleMesh* mesh = new SimpleMesh();
	mesh->indices = vCount / 3;
	glGenVertexArrays(1, &(mesh->VAO));
	glBindVertexArray(mesh->VAO);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// We want to put all vertex data into one buffer.
	// So first, tell OpenGL to allocate enough space.
	glBufferData(GL_ARRAY_BUFFER, (vCount + cCount) * float_size, NULL, GL_STATIC_DRAW);

	// Arrange data to be layout as 111222333 instead of 123123123
	glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * float_size, vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, vCount * float_size, cCount * float_size, colors.data());

	// Enforce layout where:
	// 0: position
	// 1: normal
	// 2: texcoord
	// 3: color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * float_size, (void*)0);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * float_size, (void*)(vCount * float_size));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

	return mesh;
}