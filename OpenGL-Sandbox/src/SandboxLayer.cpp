#include "SandboxLayer.h"
#include <stb_image\stb_image.h>

using namespace GLCore;
using namespace GLCore::Utils;

SandboxLayer::SandboxLayer()
	:m_CameraContorller( 16.0f / 9.0f)
{
}

SandboxLayer::~SandboxLayer()
{
}

static GLuint LoadTexture(const std::string& path) {
	int w, h, bits;

	stbi_set_flip_vertically_on_load(1);
	auto* pixels = stbi_load(path.c_str(), &w, &h, &bits, STBI_rgb);
	GLuint textureID;

	/*
	OpenGL要求所有的纹理都是4字节对齐的，即纹理的大小永远是4字节的倍数。
	通常这并不会出现什么问题，因为大部分纹理的宽度都为4的倍数并/或每像素使用4个字节。
	但是这个图片是jpg并且宽高不是4的倍数，所以出现了问题。
	通过将纹理解压对齐参数设为1，这样才能确保不会有对齐问题
	*/
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glActiveTexture(GL_TEXTURE0);
	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	stbi_image_free(pixels);

	return textureID;
}

void SandboxLayer::OnAttach()
{
	EnableGLDebugging();

	// Init here
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_Shader = Shader::FromGLSLTextFiles(
		"assets/shaders/test.vert.glsl",
		"assets/shaders/test.frag.glsl"
	);

	glUseProgram(m_Shader->GetRendererID());
	auto loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Textures");
	int samplers[2] = { 0,1 };
	glUniform1iv(loc, 2, samplers);

	glCreateVertexArrays(1, &m_QuadVA);
	glBindVertexArray(m_QuadVA);

	float vertices[] = {
		-1.5f, -0.5f, 0.0f, 0.2f, 0.6f, 0.9f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.2f, 0.6f, 0.9f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f, 0.2f, 0.6f, 0.9f, 1.0f, 1.0f, 1.0f, 0.0f,
		-1.5f,  0.5f, 0.0f, 0.2f, 0.6f, 0.9f, 1.0f, 0.0f, 1.0f, 0.0f,

		 0.5f, -0.5f, 0.0f, 0.9f, 0.6f, 0.2f, 1.0f, 0.0f, 0.0f, 1.0f,
		 1.5f, -0.5f, 0.0f, 0.9f, 0.6f, 0.2f, 1.0f, 1.0f, 0.0f, 1.0f,
		 1.5f,  0.5f, 0.0f, 0.9f, 0.6f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f, 0.9f, 0.6f, 0.2f, 1.0f, 0.0f, 1.0f, 1.0f
	};

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(m_QuadVB, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, 0);

	glEnableVertexArrayAttrib(m_QuadVB, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void*)12);
	
	glEnableVertexArrayAttrib(m_QuadVB, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void*)28);
	
	glEnableVertexArrayAttrib(m_QuadVB, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void*)36);

	uint32_t indices[] = { 
		0,1,2,2,3,0,
		4,5,6,6,7,4
	};
	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	first_img = LoadTexture("assets/textures/陈钰琪.png");
	second_img = LoadTexture("assets/textures/卜冠今.png");
}

void SandboxLayer::OnDetach()
{
	// Shutdown here
	//glDeleteVertexArrays(1, &m_QuadVA);
	//glDeleteBuffers(1, &m_QuadVB);
	//glDeleteBuffers(1, &m_QuadIB);

}

void SandboxLayer::OnEvent(Event& event)
{
	// Events here
	m_CameraContorller.OnEvent(event);
	//EventDispatcher dispatcher(event);
}



void SandboxLayer::OnUpdate(Timestep ts)
{
	// Render here
	m_CameraContorller.OnUpdate(ts);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Shader->GetRendererID());
	glBindTextureUnit(0, first_img);
	glBindTextureUnit(1, second_img);


	SetUniformMat4(m_Shader->GetRendererID(), "u_ViewProj", m_CameraContorller.GetCamera().GetViewProjectionMatrix());
	SetUniformMat4(m_Shader->GetRendererID(), "u_Transform", glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.0f)));
	
	glBindVertexArray(m_QuadVA);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
}

void SandboxLayer::OnImGuiRender()
{
	// ImGui here
	//ImGui::Begin("Controls");
	//ImGui::ColorEdit4("Square Color", glm::value_ptr((m_SquareColor)));
	//ImGui::End();
}

void SandboxLayer::SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix)
{
	int loc = glGetUniformLocation(shader, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr((matrix)));
}