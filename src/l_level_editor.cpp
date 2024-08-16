#include "glm/ext/quaternion_float.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "l_application.h"
#include "l_camera.h"
#include "l_common.h"
#include "l_entity_system.h"
#include "l_input_manager.h"
#include "l_level_editor.h"
#include "l_math.h"
#include "l_platform.h"
#include "l_render_system.h"
#include "l_resource_manager.h"
#include "l_shader.h"
#include "l_transform_system.h"
#include "l_physics_system.h"
#include <vector>

namespace lain {

    namespace level_editor {

        enum class level_editor_mode
        {
            move,
            edit
        };

        static float constexpr kGridSquareSize{0.5f};
        static float constexpr kHalfGridExtent{20.f};
        static glm::vec4 constexpr kGreyColour{0.5f, 0.5f, 0.5f, 1.f};
        static glm::vec4 constexpr kRedColour{1.f, 0.0f, 0.0f, 1.f};
        static glm::vec4 constexpr kGreenColour{0.f, 1.0f, 0.0f, 1.f};
        static glm::vec4 constexpr kDebugColour{0.33f, 0.1f, 0.1f, 1.f};
        static glm::vec4 constexpr kYellowColour{1.f, 1.f, 0.1f, 1.f};

        static level_editor_mode _mode;
        static camera3D _camera;
        static shader _grid;
        static shader _axisX;
        static shader _axisZ;
        static shader _ray;
        static shader _boundingBox;
        static glm::vec3 _currentCameraDirection;
        static entity_id _selectedEntity;
        static glm::vec3 _imGuiEntityPosition;
        static bool _debugDrawEntityAABB;
        static ray _cameraToCursorRay;

        static int GetSquaresToDraw();
        static void ProcessInputInEditMode();
        static void ProcessInputInMoveMode();
        static void UpdateInMoveMode(float const deltaTime);
        static void AddEntityToLevel(int const selection);
        static void UpdateCursorInEditMode();
        static void RemoveEntities();
        static void RemoveSelectedEntity();
        static void UpdateInEditMode();
        static void RenderInEditMode();
        static void RenderInMoveMode();
        static void DrawGrid();
        static void DrawWorldAxis();
        static void DrawEntities();

        void Initialise()
        {
            _selectedEntity = no_entity;

            _debugDrawEntityAABB = false;

            _camera._position = glm::vec3(0.f);
            _camera._targetPosition = glm::vec3(0.f);
            _camera._worldUp = glm::vec3(0.f, 1.f, 0.f);
            _camera._speed = 20.f;
            _camera._sensitivity = 0.1f;
            _camera._lerp = 0.1f;
            _camera._yaw = 0.f;
            _camera._pitch = 0.f;

            // ---------------------------------------------
            // Create grid vertices and the corresponding vao, math here is terribly
            // wrong but it does what you want fn
            // ---------------------------------------------
            int const squares{ level_editor::GetSquaresToDraw() };

            std::vector<float> vertices;

            for (int i{ -squares }; i <= squares; ++i) {
                // lines along the X axis
                vertices.push_back(-level_editor::kHalfGridExtent * 2);
                vertices.push_back(0.f);
                vertices.push_back(i * level_editor::kGridSquareSize);

                vertices.push_back(level_editor::kHalfGridExtent * 2);
                vertices.push_back(0.f);
                vertices.push_back(i * level_editor::kGridSquareSize);

                // lines along the Z axis
                vertices.push_back(i * level_editor::kGridSquareSize);
                vertices.push_back(0.f);
                vertices.push_back(-level_editor::kHalfGridExtent * 2);

                vertices.push_back(i * level_editor::kGridSquareSize);
                vertices.push_back(0.f);
                vertices.push_back(level_editor::kHalfGridExtent * 2);
            }

            _grid = resource_manager::CreatePrimitiveVAO(vertices, GL_STATIC_DRAW);

            // ---------------------------------------------
            // Create vertices for the X-Z axis
            // ---------------------------------------------
            vertices = {
                0.f,    0.01f, 0.f, // origin
                1000.f, 0.01f, 0.f, // X
            };

            _axisX = resource_manager::CreatePrimitiveVAO(vertices, GL_STATIC_DRAW);

            vertices = {
                0.f, 0.01f, 0.f,    // origin
                0.f, 0.01f, 1000.f, // Z
            };

            _axisZ = resource_manager::CreatePrimitiveVAO(vertices, GL_STATIC_DRAW);

            vertices.assign(6, 0.f);

            _ray = resource_manager::CreatePrimitiveVAO(vertices, GL_DYNAMIC_DRAW);

            std::vector<unsigned int> const indices = {
                0, 1, 1, 2, 2, 3, 3, 0, // Bottom face
                4, 5, 5, 6, 6, 7, 7, 4, // Top face
                0, 4, 1, 5, 2, 6, 3, 7  // Vertical lines
            };

            vertices.assign(24, 0.f);

            _boundingBox = resource_manager::CreateCubeVAO(vertices, GL_DYNAMIC_DRAW, indices);
        }

        void ProcessInput()
        {
            if (input_manager::IsKeyPressed(input_manager::key::f1)) {
                _mode = level_editor_mode::edit;
            } else if (input_manager::IsKeyPressed(input_manager::key::f2)) {
                _mode = level_editor_mode::move;
            }

            if (input_manager::IsKeyPressed(input_manager::key::b)) {
                _debugDrawEntityAABB = !_debugDrawEntityAABB;
            }

            switch (_mode) {
                case level_editor_mode::edit:
                    ProcessInputInEditMode();
                    break;
                case level_editor_mode::move:
                    ProcessInputInMoveMode();
                    break;
            }
        }

        void Update(float const deltaTime)
        {
            ImGuiIO& io = ImGui::GetIO();

            switch (_mode) {
                case level_editor_mode::edit:
                    ReleaseCursorFromWindow();
                    io.MouseDrawCursor = true;
                    UpdateInEditMode();
                    break;
                case level_editor_mode::move:
                    ConstrainCursorInWindow();
                    io.MouseDrawCursor = false;
                    UpdateInMoveMode(deltaTime);
                    break;
            }
        }

        void Render()
        {
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            DrawWorldAxis();

            DrawGrid();

            DrawEntities();

            switch (_mode) {
                case level_editor_mode::edit:
                    RenderInEditMode();
                    break;
                case level_editor_mode::move:
                    RenderInMoveMode();
                    break;
            }
        }

        static int GetSquaresToDraw()
        {
            return static_cast<int>((2 * kHalfGridExtent) / kGridSquareSize);
        }

        static void ProcessInputInEditMode()
        {
            bool const shouldProcessClick{
                !ImGui::IsAnyItemHovered() &&
                !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) &&
                input_manager::IsMouseButtonPressed(input_manager::mouse_button::left)};

            if (shouldProcessClick) {
                bool pickedEntity{false};
                std::size_t entity{0};

                // OPTIMISE: checking against every shape is bad, very bad. Change once you have
                // a spatial hash grid.
                while (!pickedEntity && entity < entity_system::GetEntityCount()) {
                    for (auto const& shape : physics_system::GetCollisionShapes(entity)) {
                        if (RayIntersectsAABB(_cameraToCursorRay, shape)) {
                            _selectedEntity = entity;
                            pickedEntity = true;
                        }
                    }
                    ++entity;
                }

                if (!pickedEntity) {
                    _selectedEntity = no_entity;
                }
            }
        }

        static void ProcessInputInMoveMode()
        {
            // Keyboard related stuff
            _currentCameraDirection = glm::vec3(0);

            if (input_manager::IsKeyHeld(input_manager::key::w)) {
                _currentCameraDirection.z = 1;
            }

            if (input_manager::IsKeyHeld(input_manager::key::s)) {
                _currentCameraDirection.z = -1;
            }

            if (input_manager::IsKeyHeld(input_manager::key::a)) {
                _currentCameraDirection.x = -1;
            }

            if (input_manager::IsKeyHeld(input_manager::key::d)) {
                _currentCameraDirection.x = 1;
            }

            if (input_manager::IsKeyHeld(input_manager::key::q)) {
                _currentCameraDirection.y = 1;
            }

            if (input_manager::IsKeyHeld(input_manager::key::e)) {
                _currentCameraDirection.y = -1;
            }

            _camera.ProcessKeyboard(_currentCameraDirection);

            // Cursor
            if (input_manager::IsCursorMoving()) {
                _camera.ProcessCursor(input_manager::GetCursorPosition());
            }
        }

        static void UpdateInMoveMode(float const deltaTime)
        {
            _camera.SetTargetPosition(deltaTime);
            _camera.Update(deltaTime);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Move mode");
            ImGui::Text("Position: (%.2f, %.2f, %.2f)",
                        _camera._position.x,
                        _camera._position.y,
                        _camera._position.z);
            ImGui::End();
        }

        static void AddEntityToLevel(int const selection)
        {
            model_type type;

            if (selection == 0) {
                type = model_type::maze;
            } else {
                type = model_type::ball;
            }

            resource_manager::LoadModel(type);

            // ----------------------------
            // Add entity in every system
            // ----------------------------
            _selectedEntity = entity_system::AddEntity();

            auto transform = transform_component(glm::mat4{1.f},
                                                 glm::quat(0.f, 0.f, 0.f, 0.f),
                                                 glm::vec3(0.f),
                                                 glm::vec3(1.f));

            transform_system::AddEntity(_selectedEntity, std::move(transform));

            resource_manager::AddEntityModelRelationship(_selectedEntity, type);

            auto const* model = resource_manager::GetModelDataFromEntity(_selectedEntity);

            render_system::AddEntity(render_component(model));

            physics_system::AddEntity(_selectedEntity, physics_component{});

            for (auto const& mesh : model->_meshes) {
                physics_system::AddCollisionShapeForEntity(_selectedEntity, mesh._boundingBox);
            }
        }

        static void UpdateCursorInEditMode()
        {
            ImGuiIO& io = ImGui::GetIO();

            glm::vec4 cursorPos(io.MousePos.x, io.MousePos.y, 0.f, 0.f);

            cursorPos = ScreenSpaceToNormalisedDeviceCoordinates(cursorPos,
                                                                 application::GetWindowWidth(),
                                                                 application::GetWindowHeight());

            cursorPos = NormalisedDeviceCoordinatesToClipSpace(cursorPos);

            cursorPos = ClipSpaceToViewSpace(cursorPos,
                                             render_system::GetCurrentProjectionMatrix());

            cursorPos = ViewSpaceToWorldSpace(cursorPos, _camera.GetViewMatrix());

            _cameraToCursorRay._direction = glm::normalize(glm::vec3(cursorPos) - _camera._position);

            _cameraToCursorRay._position = _camera._position;
        }

        static void RemoveEntities()
        {
            entity_system::RemoveAllEntities();
            transform_system::RemoveAllEntities();
            render_system::RemoveAllEntities();
            physics_system::RemoveAllEntities();

            _selectedEntity = no_entity;
        }

        static void RemoveSelectedEntity()
        {
            entity_system::RemoveEntity(_selectedEntity);
            transform_system::RemoveEntity(_selectedEntity);
            render_system::RemoveEntity(_selectedEntity);
            physics_system::RemoveEntity(_selectedEntity);

            _selectedEntity = no_entity;
        }

        static void UpdateInEditMode()
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Edit mode");
            ImGui::NewLine();
            ImGui::Text("Add entity");
            ImGui::NewLine();

            static int selection{ 0 };

            ImGui::RadioButton("Maze", &selection, 0);
            ImGui::RadioButton("Ball", &selection, 1);

            ImGui::NewLine();

            if (ImGui::Button("Add")) {
                AddEntityToLevel(selection);
            }

            if (entity_system::GetEntityCount() > 0) {
                if (ImGui::Button("Remove Entities")) {
                    RemoveEntities();
                }
            }

            ImGui::End();

            if (_selectedEntity != no_entity) {
                bool modified{false};

                auto transform = transform_system::GetTransform(_selectedEntity);

                _imGuiEntityPosition.x = transform._position.x;
                _imGuiEntityPosition.y = transform._position.y;
                _imGuiEntityPosition.z = transform._position.z;

                ImGui::Begin("Entity Information", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("Id: %d", _selectedEntity);
                ImGui::NewLine();

                if (ImGui::DragFloat3("Position", &_imGuiEntityPosition.x, 0.1, -10.f, 10.f)) {
                    modified = true;
                }

                ImGui::NewLine();

                if (ImGui::Button("Remove Selected Entity")) {
                    RemoveSelectedEntity();
                }

                ImGui::End();

                if (modified) {
                    transform._position.x = _imGuiEntityPosition.x;
                    transform._position.y = _imGuiEntityPosition.y;
                    transform._position.z = _imGuiEntityPosition.z;

                    // Override current transform for this entity after it was modified.
                    transform_system::SetEntity(_selectedEntity, std::move(transform));

                    // Update transforms and physics when you modify something!
                    transform_system::Update();
                    physics_system::Update();
                }
            }

            UpdateCursorInEditMode();
        }

        static void RenderInEditMode()
        {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        static void RenderInMoveMode()
        {
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        static void DrawGrid()
        {
            static unsigned int id{resource_manager::GetShader(kPrimitiveShaderId)->_id};

            render_system::DrawLines(id, _grid._vao, 1000, _camera.GetViewMatrix(), kGreyColour);
        }

        static void DrawWorldAxis()
        {
            static unsigned int id{resource_manager::GetShader(kPrimitiveShaderId)->_id};

            render_system::DrawLines(id, _axisX._vao, 2, _camera.GetViewMatrix(), kRedColour);

            render_system::DrawLines(id, _axisZ._vao, 2, _camera.GetViewMatrix(), kGreenColour);
        }

        static void DrawEntities()
        {
            static unsigned int id{resource_manager::GetShader(kPrimitiveShaderId)->_id};

            if (_debugDrawEntityAABB) {
                for (std::size_t i{0}; i < entity_system::GetEntityCount(); ++i) {
                    for (auto const& aabb : physics_system::GetCollisionShapes(i)) {
                        float vertices[] = {
                            aabb._min.x, aabb._min.y, aabb._min.z,
                            aabb._max.x, aabb._min.y, aabb._min.z,
                            aabb._max.x, aabb._max.y, aabb._min.z,
                            aabb._min.x, aabb._max.y, aabb._min.z,
                            aabb._min.x, aabb._min.y, aabb._max.z,
                            aabb._max.x, aabb._min.y, aabb._max.z,
                            aabb._max.x, aabb._max.y, aabb._max.z,
                            aabb._min.x, aabb._max.y, aabb._max.z,
                        };

                        glBindVertexArray(_boundingBox._vao);
                        glBindBuffer(GL_ARRAY_BUFFER, _boundingBox._vbo);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

                        render_system::DrawBoundingBox(id, _boundingBox._vao, _camera.GetViewMatrix(), kYellowColour);
                    }
                }
            }

            render_system::DrawEntities(_camera);
        }
    };
};
