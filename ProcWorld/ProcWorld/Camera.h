#pragma once
#include <GL/glew.h>

#include "glm.hpp"
#include <gtc/quaternion.hpp>

enum MOVEMENT_DIRECTION
{
	FORWARD,
	RIGHT,
	LEFT,
	BACKWARD
};

class Camera {
public:
	Camera();
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp);
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp, 
		GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far);


	void ActivateKey(MOVEMENT_DIRECTION direction);
	void DeactivateKey(MOVEMENT_DIRECTION direction);

	void ProcessMovement(GLfloat deltaTime);
	void ProcessMotion(GLfloat dx, GLfloat dy);

	const glm::mat4& GetViewMat(void) const;
	const glm::mat4& GetProjectionMat(void) const;

	void UpdateRotation(void);
	void UpdateVectors(void);

private:

	GLfloat m_fovy;
	GLfloat m_aspect;
	GLfloat m_near, m_far;

	// Camera transform
	glm::vec3 m_position;
	glm::quat m_rotation;

	// Movement params
	GLfloat m_speed;
	GLfloat m_sensitivity;
	GLfloat m_yaw, m_pitch;

	glm::vec3 m_worldUp;
	glm::vec3 m_camUp;
	glm::vec3 m_camRight;
	glm::vec3 m_direction;
	glm::vec3 m_lookAtTarget;

	glm::mat4 m_projectionMat;
	glm::mat4 m_lookAtMat;

	// Input (key down indicator)
	GLint	m_keysDown[4] = { 0 };
};
