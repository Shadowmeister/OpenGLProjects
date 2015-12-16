#include <Camera.h>

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) :
	Front{ glm::vec3(0.0f, 0.0f, -1.0f) },
	MovementSpeed{ SPEED },
	MouseSensitivity{ SENSITIVITY },
	Zoom{ ZOOM }
{
	this->Position = position;
	this->WorldUp = up;
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :
	Front{ glm::vec3(0.0f, 0.0f, -1.0f) },
	MovementSpeed{ SPEED },
	MouseSensitivity{ SENSITIVITY },
	Zoom{ ZOOM }
{
	this->Position = glm::vec3(posX, posY, posZ);
	this->WorldUp = glm::vec3(upX, upY, upZ);
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
	GLfloat velocity = this->MovementSpeed * deltaTime;
	switch (direction)
	{
	case FORWARD:
		this->Position += this->Front * velocity;
		break;
	case BACKWARD:
		this->Position -= this->Front * velocity;
		break;
	case LEFT:
		this->Position -= this->Right * velocity;
		break;
	case RIGHT:
		this->Position += this->Right * velocity;
		break;
	}
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
	xoffset *= this->MouseSensitivity * (this->Zoom / ZOOM);
	yoffset *= this->MouseSensitivity * (this->Zoom / ZOOM);

	this->Yaw += xoffset;
	this->Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (this->Pitch > 89.0f)
		{
			this->Pitch = 89.0f;
		}
		if (this->Pitch < -89.0f)
		{
			this->Pitch = -89.0f;
		}
	}

	// Update Front, Right, and Up vectors using the updated Eular angles
	this->updateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat yoffset)
{
	if (this->Zoom >= 1.0f && this->Zoom <= ZOOM)
	{
		this->Zoom -= yoffset;
	}
	if (this->Zoom <= 1.0f)
	{
		this->Zoom = 1.0f;
	}
	if (this->Zoom >= ZOOM)
	{
		this->Zoom = ZOOM;
	}
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	this->Front = glm::normalize(front);
	// Also re-calculate the Right and Up vectors
	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}
