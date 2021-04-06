#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE		// VULKAN
//#define GLM_FORCE_SS2
#define GLM_FORCE_INTRINSICS
#include "../../Dependencies/glm/glm/glm.hpp"
#include "../../Dependencies/glm/glm/matrix.hpp"
#include "../../Dependencies/glm/glm/ext/matrix_transform.hpp"
#include "../../Dependencies/glm/glm/ext/matrix_clip_space.hpp"

#include "../../Core/Assertions.h"
#include "../../Core/SmartPointers.h"

#include "../Core/Types.h"

#include "../../Events/CallbackHandle.h"
#include "../../Events/Event.h"

#include "../../Utilities/Hash.h"

#include "System.h"

#pragma warning(disable : 4251)


AK_NAMESPACE_BEGIN


namespace Events::Camera::Input 
{
	const ParamId INPUT = "Events::Camera::Input::INPUT"_Hash;
}

enum class InputButtons
{
	W,
	A,
	S,
	D
};


struct Camera;
struct Transform;
class AK_DLL CameraSystem : public System
{
public:
	void Initialize();
	void Update(float _dt);
	void Shutdown();

public:
	// utility functions
	bool IsMoving(const Entity _entity);
	float GetNearClip(const Entity _entity);
	float GetFarClip(const Entity _entity);

	void SetPerspective(const Entity _entity, float _fov, float _aspect, float _znear, float _zfar);
	void UpdateAspectRatio(const Entity _entity, float _aspect);
	void SetPosition(const Entity _entity, glm::vec3 _position);
	void SetRotation(const Entity _entity, glm::vec3 _rotation);
	void Rotate(const Entity _entity, glm::vec3 _delta);
	void SetTranslation(const Entity _entity, glm::vec3 _translation);
	void Translate(const Entity _entity, glm::vec3 _delta);

private:
	void UpdateViewMatrix(Camera& _camera, const Transform& _transform);
	void InputListener(Event& event);

private:
	CallbackHandle m_callbackHandle;
	std::unordered_map<Entity, std::bitset<4>> m_keys;	// only WASD (and hard-coded for now)
};

AK_NAMESPACE_END
