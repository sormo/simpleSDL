LOCAL_PATH := $(call my-dir)

LOCAL_SHORT_COMMANDS := true

#
# Path to a the directory containing SDL sources
#
EXTERNAL_LIB_PATH := ../../../contrib

define walk
  $(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef

#
# Bullet
#

include $(CLEAR_VARS)
LOCAL_MODULE := libBullet
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src
BULLET_FILE_LIST := $(wildcard \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/LinearMath/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/Bullet3Common/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletCollision/BroadphaseCollision/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletCollision/CollisionDispatch/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletCollision/CollisionShapes/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletCollision/NarrowPhaseCollision/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletDynamics/ConstraintSolver/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletDynamics/Dynamics/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletDynamics/Featherstone/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletDynamics/MLCPSolvers/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletDynamics/Vehicle/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletDynamics/Character/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletSoftBody/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletInverseDynamics/*.cpp \
		$(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/BulletInverseDynamics/details/*.cpp \
		)

#ALL_BULLET_FILES := $(call walk, $(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src/)
#BULLET_FILE_LIST := $(filter %.cpp, $(ALL_BULLET_FILES))

LOCAL_SRC_FILES := $(BULLET_FILE_LIST:$(LOCAL_PATH)/%=%)

include $(BUILD_STATIC_LIBRARY)

#
# Build app using SDL
#

include $(CLEAR_VARS)
LOCAL_MODULE := libSimpleExample


# Includes
LOCAL_C_INCLUDES +=  \
  $(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/SDL/include \
  $(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/SDL_image \
  $(LOCAL_PATH)/../../../sourceCommon \
  $(LOCAL_PATH)/../../../modelConvert/include \
  $(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/flatbuffers/include \
  $(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src


# Source files
LOCAL_SRC_FILES :=  \
  ../../../source/main.cpp \
  ../../../source/Common.cpp \
  ../../../source/Application.cpp \
  ../../../source/OpenGL.cpp \
  ../../../source/Shader.cpp \
  ../../../source/Texture.cpp \
  ../../../source/Camera.cpp \
  ../../../source/ObjLoader.cpp \
  ../../../sourceCommon/VboIndexer.cpp \
  ../../../sourceCommon/TangentSpace.cpp \
  ../../../source/Text2D.cpp \
  ../../../source/Model.cpp \
  ../../../source/ModelShader.cpp \
  ../../../source/ShaderGenerator.cpp \
  ../../../source/TextureManager.cpp \
  ../../../sourceCommon/CommonProject.cpp \
  ../../../source/Skybox.cpp \
  ../../../source/Framebuffer.cpp \
  ../../../source/Postprocess.cpp \
  ../../../source/ShadowScene.cpp \
  ../../../source/Shadow.cpp \
  ../../../source/imgui/imgui.cpp \
  ../../../source/imgui/imgui_demo.cpp \
  ../../../source/imgui/imgui_draw.cpp \
  ../../../source/imgui/imgui_impl_gles2.cpp \
  ../../../source/imgui/imgui_widgets.cpp \
  ../../../source/Scene.cpp \
  ../../../source/Editor.cpp \
  ../../../source/EventDispatchers.cpp \
  ../../../source/UserInterface.cpp \
  ../../../source/Bullet.cpp \
  ../../../source/BulletDebug.cpp \
  ../../../source/DebugDraw.cpp \
  ../../../source/EditorDebug.cpp \
  ../../../source/Gizmo.cpp \
  ../../../source/GizmoDraw.cpp \
  ../../../source/Shapes.cpp \
  ../../../source/Generator.cpp


# Linking options
LOCAL_SHARED_LIBRARIES := SDL2
#LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image
LOCAL_STATIC_LIBRARIES := libBullet
#LOCAL_STATIC_LIBRARIES := SDL2main
LOCAL_LDLIBS := -lz -llog -landroid -lGLESv1_CM -lGLESv2


# Build app as a shared library
include $(BUILD_SHARED_LIBRARY)


#
# External modules.
#
$(call import-add-path,$(EXTERNAL_LIB_PATH))
$(call import-module,SDL)
#$(call import-module,SDL_image)
