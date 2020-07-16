#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

class SandboxLayer : public GLCore::Layer
{
public:
	SandboxLayer();
	virtual ~SandboxLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;
	static void SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix);
private:
	GLCore::Utils::Shader* m_Shader;
	GLCore::Utils::OrthographicCameraController m_CameraContorller;
	GLuint m_QuadVA, m_QuadVB, m_QuadIB;
	glm::vec4 m_SquareColor = { 0.8f,0.2f,0.3f,1.0f };
	GLuint first_img, second_img;
};