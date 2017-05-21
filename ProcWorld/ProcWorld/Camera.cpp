#include "Camera.h"
#include <gtc/matrix_transform.hpp>
#include <gtx/quaternion.hpp>

Camera::Camera() {
}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp) {
}

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp, 
	GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far) 
	: m_fovy(fovy)
	, m_aspect(aspect)
	, m_near(near)
	, m_far(far)
	, m_position(position)
	, m_rotation(glm::quat())
	, m_speed(20.0f)
	, m_sensitivity(10.0f)
	, m_yaw(0.0f)
	, m_pitch(0.0f)
	, m_worldUp(worldUp)
	, m_camUp(glm::vec3(0.0f, 1.0f, 0.0f))
	, m_camRight(glm::vec3(1.0f, 0.0f, 0.0f))
	, m_direction(glm::vec3(0.0f, 0.0f, -1.0f))
	, m_lookAtTarget(target)
	, m_projectionMat(glm::perspective(m_fovy, m_aspect, m_near, m_far))
	, m_lookAtMat(glm::mat4())
{
}

void Camera::ActivateKey(MOVEMENT_DIRECTION direction) {
	m_keysDown[direction] = 1;
}

void Camera::DeactivateKey(MOVEMENT_DIRECTION direction) {
	m_keysDown[direction] = 0;
}

void Camera::ProcessMovement(GLfloat deltaTime) {
	if (m_keysDown[FORWARD] == 1)
	{
		m_position += m_direction * m_speed * deltaTime;
	}

	if (m_keysDown[RIGHT] == 1)
	{
		m_position += glm::normalize(glm::cross(m_direction, m_camUp)) * m_speed * deltaTime;
	}

	if (m_keysDown[LEFT] == 1)
	{
		m_position -= glm::normalize(glm::cross(m_direction, m_camUp)) * m_speed * deltaTime;
	}

	if (m_keysDown[BACKWARD] == 1)
	{
		m_position -= m_direction * m_speed * deltaTime;
	}

	UpdateVectors();
}

void Camera::ProcessMotion(GLfloat dx, GLfloat dy) {
	float xOffset = dx;
	float yOffset = dy;

	xOffset *= 2.25f * m_sensitivity;
	yOffset *= 2.25f * m_sensitivity;

	m_yaw += xOffset;
	m_pitch += yOffset;

	UpdateRotation();
	UpdateVectors();
}

const glm::mat4& Camera::GetViewMat() const {
	return m_lookAtMat;
}

const glm::mat4& Camera::GetProjectionMat() const {
	return m_projectionMat;
}

const glm::vec3& Camera::GetPosition() const
{
	return m_position;
}

const glm::vec3& Camera::GetDirection() const
{
	return m_direction;
}

void Camera::UpdateRotation() {
	// Quaternion version
	glm::quat q = glm::angleAxis(glm::radians(-m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	q *= glm::angleAxis(glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));

	m_rotation = glm::normalize(q);
}

void Camera::UpdateVectors() {
	m_direction = glm::rotate(glm::conjugate(m_rotation), glm::vec3(0.0f, 0.0f, -1.0f));
	// Calculste right / up vectors
	m_camRight = glm::normalize(glm::cross(m_worldUp, m_direction));
	m_camUp = glm::cross(m_direction, m_camRight);
	m_lookAtMat = glm::lookAt(m_position, m_position + m_direction, m_camUp);
}
