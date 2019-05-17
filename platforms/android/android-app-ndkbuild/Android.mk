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
  $(LOCAL_PATH)/../../../source \
  $(LOCAL_PATH)/../../../modelConvert/include \
  $(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/flatbuffers/include \
  $(LOCAL_PATH)/$(EXTERNAL_LIB_PATH)/bullet3/src


# Source files
SOURCE_FILE_LIST := $(wildcard \
	$(LOCAL_PATH)/../../../sourceCommon/*.cpp \
	$(LOCAL_PATH)/../../../source/*.cpp \
	$(LOCAL_PATH)/../../../source/editor/*.cpp \
	$(LOCAL_PATH)/../../../source/legacy/*.cpp \
	$(LOCAL_PATH)/../../../source/model/*.cpp \
	$(LOCAL_PATH)/../../../source/scene/*.cpp \
	$(LOCAL_PATH)/../../../source/utils/*.cpp \
	$(LOCAL_PATH)/../../../source/imgui/*.cpp \
	)

LOCAL_SRC_FILES := $(SOURCE_FILE_LIST:$(LOCAL_PATH)/%=%)

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
