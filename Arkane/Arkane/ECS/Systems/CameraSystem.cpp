#include "CameraSystem.h"


#include "../Components/Transform.h"
#include "../Components/Camera.h"

#include "../Managers/ECSManager.h"

#include "../../Events/EventManager.h"


AK_NAMESPACE_BEGIN

void CameraSystem::Initialize()
{
	m_callbackHandle = akAddMethodEventListener(Events::Camera::Input::INPUT, CameraSystem::InputListener);

	for (Entity entity : m_entities)
	{
		Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(entity);
		Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(entity);

		camera.m_view = glm::mat4(1.0);
		camera.m_proj = glm::mat4(1.0);

		camera.m_zNearPlane = 0.0f;
		camera.m_zFarPlane = 100.0f;
		camera.m_fov = AK_DEG_TO_RAD(45.0f);
		camera.m_type = Camera::CameraType::CameraType_LookAt;
		camera.m_rotationSpeed = 1.0f;
		camera.m_movementSpeed = 1.0f;
		camera.m_updated = false;

		transform.m_position = glm::vec3(0.0f, 0.0f, 0.0f);
		transform.m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		transform.m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	}
}

void CameraSystem::Update(float _dt)
{
	static glm::vec3 oneY = glm::vec3(0.0f, 1.0f, 0.0f);

	for (Entity entity : m_entities)
	{
		Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(entity);
		Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(entity);

		camera.m_updated = false;
		if (camera.m_type == Camera::CameraType::CameraType_FirstPerson)
		{
			if (IsMoving(entity))
			{
				glm::vec3 camFront;
				camFront.x = -std::cos(transform.m_rotation.x) * std::sin(transform.m_rotation.y);
				camFront.y = std::sin(transform.m_rotation.x);
				camFront.z = std::cos(transform.m_rotation.x) * std::cos(transform.m_rotation.y);
				camFront = glm::normalize(camFront);

				float moveSpeed = _dt * camera.m_movementSpeed;

				if (m_keys[entity].test(static_cast<std::size_t>(InputButtons::W)))
				{
					transform.m_position += camFront * moveSpeed;
				}
				if (m_keys[entity].test(static_cast<std::size_t>(InputButtons::S)))
				{
					transform.m_position -= camFront * moveSpeed;
				}
				if (m_keys[entity].test(static_cast<std::size_t>(InputButtons::A)))
				{
					transform.m_position -= glm::normalize(glm::cross(camFront, oneY)) * moveSpeed;
				}
				if (m_keys[entity].test(static_cast<std::size_t>(InputButtons::D)))
				{
					transform.m_position += glm::normalize(glm::cross(camFront, oneY)) * moveSpeed;
				}

				UpdateViewMatrix(camera, transform);
			}
		}
	}
}

void CameraSystem::Shutdown()
{
	akRemoveMethodEventListener(m_callbackHandle);
}

bool CameraSystem::IsMoving(const Entity _entity)
{
	if (m_keys.find(_entity) == m_keys.end())
	{
		return false;
	}

	return m_keys[_entity].any();
}

float CameraSystem::GetNearClip(const Entity _entity) 
{
	Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(_entity);
	return camera.m_zNearPlane;
}

float CameraSystem::GetFarClip(const Entity _entity)
{
	Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(_entity);
	return camera.m_zFarPlane;
}

void CameraSystem::SetPerspective(const Entity _entity, float _fov, float _aspect, float _znear, float _zfar)
{
	Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(_entity);
	Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(_entity);
	camera.m_fov = _fov;
	camera.m_zNearPlane = _znear;
	camera.m_zFarPlane = _zfar;
	camera.m_proj = glm::perspective(_fov, _aspect, _znear, _zfar);
};

void CameraSystem::UpdateAspectRatio(const Entity _entity, float _aspect)
{
	Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(_entity);
	Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(_entity);
	camera.m_proj = glm::perspective(camera.m_fov, _aspect, camera.m_zNearPlane, camera.m_zFarPlane);
}

void CameraSystem::SetPosition(const Entity _entity, glm::vec3 _position)
{
	Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(_entity);
	Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(_entity);
	transform.m_position = _position;
	UpdateViewMatrix(camera, transform);
}

void CameraSystem::SetRotation(const Entity _entity, glm::vec3 _rotation)
{
	Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(_entity);
	Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(_entity);
	transform.m_rotation = _rotation;
	UpdateViewMatrix(camera, transform);
};

void CameraSystem::Rotate(const Entity _entity, glm::vec3 _delta)
{
	Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(_entity);
	Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(_entity);
	transform.m_rotation += _delta;
	UpdateViewMatrix(camera, transform);
}

void CameraSystem::SetTranslation(const Entity _entity, glm::vec3 _translation)
{
	Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(_entity);
	Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(_entity);
	transform.m_position = _translation;
	UpdateViewMatrix(camera, transform);
};

void CameraSystem::Translate(const Entity _entity, glm::vec3 _delta)
{
	Camera& camera = ECSManager::GetInstance().GetComponent<Camera>(_entity);
	Transform& transform = ECSManager::GetInstance().GetComponent<Transform>(_entity);
	transform.m_position += _delta;
	UpdateViewMatrix(camera, transform);
}


void CameraSystem::UpdateViewMatrix(Camera& _camera, const Transform& _transform)
{
	static glm::mat4 iM = glm::mat4(1.0f);
	static glm::vec3 oneX = glm::vec3(1.0f, 0.0f, 0.0f);
	static glm::vec3 oneY = glm::vec3(0.0f, 1.0f, 0.0f);
	static glm::vec3 oneZ = glm::vec3(0.0f, 0.0f, 1.0f);
	static glm::vec3 invD = glm::vec3(1.0f, 1.0f, -1.0f);

// 	glm::mat4 rotM = glm::rotate(iM, glm::radians(_cameraTransform.m_rotation.x), oneX);
// 	rotM = glm::rotate(rotM, glm::radians(_cameraTransform.m_rotation.y), oneY);
// 	rotM = glm::rotate(rotM, glm::radians(_cameraTransform.m_rotation.z), oneZ);

	glm::mat4 rotM = glm::rotate(iM, _transform.m_rotation.x, oneX);
	rotM = glm::rotate(rotM, _transform.m_rotation.y, oneY);
	rotM = glm::rotate(rotM, _transform.m_rotation.z, oneZ);

	glm::mat4 transM = glm::translate(iM, _transform.m_position * invD);

	if (_camera.m_type == Camera::CameraType::CameraType_FirstPerson)
	{
		_camera.m_view = rotM * transM;
	}
	else
	{
		_camera.m_view = transM * rotM;
	}

	_camera.m_updated = true;
};

void CameraSystem::InputListener(Event& event)
{
	std::bitset<4> keys = event.GetParam<std::bitset<4>>(Events::Camera::Input::INPUT);
	if (event.HasSpecificEntity())
	{
		m_keys[event.GetEntity()] = keys;
	}
	else
	{
		for (Entity entity : m_entities)
		{
			m_keys[entity] = keys;
		}
	}
}

AK_NAMESPACE_END
