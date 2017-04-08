﻿//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_VIEWER_H_
#define SABA_VIEWER_VIEWER_H_

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "ViewerContext.h"
#include "ViewerCommand.h"
#include "Grid.h"
#include "ModelDrawer.h"
#include "CameraOverrider.h"

#include <Saba/GL/GLObject.h>
#include <Saba/GL/Model/MMD/GLMMDModel.h>
#include <Saba/GL/Model/OBJ/GLOBJModelDrawContext.h>
#include <Saba/GL/Model/MMD/GLMMDModelDrawContext.h>

#include <imgui.h>
#include <ImGuizmo.h>

#include <sol.hpp>

#include <map>
#include <string>
#include <memory>

namespace saba
{
	class ImGUILogSink;

	class Viewer
	{
	public:
		Viewer();
		~Viewer();

		struct InitializeParameter
		{
			InitializeParameter();

			bool	m_msaaEnable;
			int		m_msaaCount;
		};

		bool Initialize(const InitializeParameter& initParam = InitializeParameter());
		void Uninitislize();

		int Run();

		bool ExecuteCommand(const ViewerCommand& cmd);

	private:
		struct Mouse
		{
			Mouse();
			void Initialize(GLFWwindow* window);
			void Update(GLFWwindow* window);
			void SetScroll(double x, double y);

			double	m_prevX;
			double	m_prevY;
			double	m_dx;
			double	m_dy;

			double	m_saveScrollX;
			double	m_saveScrollY;
			double	m_scrollX;
			double	m_scrollY;
		};

		enum class CameraMode
		{
			None,
			Orbit,
			Dolly,
			Pan,
		};

		enum class MouseLockMode
		{
			None,
			RequestLock,
			Lock,
		};

	private:
		using ModelDrawerPtr = std::shared_ptr<ModelDrawer>;

		void SetupSjisGryphRanges();
		void Draw();
		void DrawUI();
		void DrawInfoUI();
		void DrawLogUI();
		void DrawCommandUI();
		void DrawManip();
		void DrawCtrlUI();
		void DrawModelListCrtl();
		void DrawTransformCtrl();
		void DrawAnimCtrl();
		void DrawCameraCtrl();
		void DrawLightCtrl();
		void DrawLightGuide();
		void DrawModelCtrl();
		void UpdateAnimation();
		void InitializeAnimation();
		void RegisterCommand();
		void RefreshCustomCommand();

		bool CmdOpen(const std::vector<std::string>& args);
		bool CmdClear(const std::vector<std::string>& args);
		bool CmdPlay(const std::vector<std::string>& args);
		bool CmdStop(const std::vector<std::string>& args);
		bool CmdSelect(const std::vector<std::string>& args);
		bool CmdTranslate(const std::vector<std::string>& args);
		bool CmdRotate(const std::vector<std::string>& args);
		bool CmdScale(const std::vector<std::string>& args);
		bool CmdRefreshCustomCommand(const std::vector<std::string>& args);
		bool CmdEnableUI(const std::vector<std::string>& args);
		bool CmdClearAnimation(const std::vector<std::string>& args);
		bool CmdClearSceneAnimation(const std::vector<std::string>& args);

		bool LoadOBJFile(const std::string& filename);
		bool LoadPMDFile(const std::string& filename);
		bool LoadPMXFile(const std::string& filename);
		bool LoadVMDFile(const std::string& filename);

		bool ClearAnimation(ModelDrawer* modelDrawer);
		bool ClearSceneAnimation();

		bool AdjustSceneScale();
		std::string GetNewModelName();
		ModelDrawerPtr FindModelDrawer(const std::string& name);

		static void OnMouseButtonStub(GLFWwindow* window, int button, int action, int mods);
		void OnMouseButton(int button, int action, int mods);

		static void OnScrollStub(GLFWwindow* window, double offsetx, double offsety);
		void OnScroll(double offsetx, double offsety);

		static void OnKeyStub(GLFWwindow* window, int key, int scancode, int action, int mods);
		void OnKey(int key, int scancode, int action, int mods);

		static void OnCharStub(GLFWwindow* window, unsigned int codepoint);
		void OnChar(unsigned int codepoint);

		static void OnDropStub(GLFWwindow* window, int count, const char** paths);
		void OnDrop(int count, const char** paths);

	private:
		enum class FPSMode
		{
			FPS30,
			FPS60,
		};

		struct Command
		{
			std::string												m_name;
			std::function<bool(const std::vector<std::string>&)>	m_commandFunc;
		};

		struct CustomCommand
		{
			std::string		m_name;
			sol::function	m_commandFunc;
			std::string		m_menuName;
		};

		using CustomCommandPtr = std::unique_ptr<CustomCommand>;

		struct CustomCommandMenuItem
		{
			std::string										m_name;
			std::map<std::string, CustomCommandMenuItem>	m_items;
			CustomCommand*									m_command;

			CustomCommandMenuItem& operator[] (const std::string& name)
			{
				return m_items[name];
			}
		};

	private:
		bool	m_msaaEnable;
		int		m_msaaCount;

		ViewerContext	m_context;
		std::unique_ptr<GLOBJModelDrawContext>	m_objModelDrawContext;
		std::unique_ptr<GLMMDModelDrawContext>	m_mmdModelDrawContext;
		std::vector<Command>	m_commands;

		std::vector<ModelDrawerPtr>	m_modelDrawers;
		ModelDrawerPtr				m_selectedModelDrawer;

		std::unique_ptr<CameraOverrider>	m_cameraOverrider;

		bool		m_glfwInitialized;
		GLFWwindow*	m_window;

		GLProgramObject		m_bgProg;
		GLVertexArrayObject	m_bgVAO;
		GLint			m_uColor1;
		GLint			m_uColor2;

		Mouse		m_mouse;
		CameraMode	m_cameraMode;
		Grid		m_grid;
		MouseLockMode	m_mouseLockMode;
		float		m_sceneUnit;

		double		m_prevTime;

		// Model Name
		size_t	m_modelNameID;

		//
		std::vector<ImWchar>	m_gryphRanges;

		std::unique_ptr<sol::state>		m_lua;
		std::vector<CustomCommandPtr>	m_customCommands;
		CustomCommandMenuItem			m_customCommandMenuItemRoot;

		// InfoUI
		bool	m_enableInfoUI;

		// LogUI
		std::shared_ptr<ImGUILogSink>	m_imguiLogSink;
		bool							m_enableLogUI;

		// CommandUI
		bool	m_enableCommandUI;

		// Manipulator
		bool				m_enableManip;
		ImGuizmo::OPERATION	m_currentManipOp;
		ImGuizmo::MODE		m_currentManipMode;

		// AnimCtrl
		float		m_animCtrlEditFPS;
		FPSMode		m_animCtrlFPSMode;

		// Control UI
		bool		m_enableCtrlUI;

		// Light Ctrl
		bool		m_enableLightManip;
		bool		m_enableLightGuide;
		ImGuizmo::OPERATION	m_lightManipOp;
		glm::vec3	m_lightManipPos;
		glm::mat4	m_lgihtManipMat;

		// Camera Override
		bool	m_cameraOverride;

		// Clip Elapsed
		bool	m_clipElapsed;
	};
}

#endif // !SABA_VIEWER_VIEWER_H_
