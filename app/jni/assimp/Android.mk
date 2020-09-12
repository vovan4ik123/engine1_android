LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := assimp

LOCAL_CPP_FEATURES += exceptions
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include		\
					$(LOCAL_PATH)/code			\
					$(LOCAL_PATH)/contrib/irrXML\
					$(LOCAL_PATH)/contrib/unzip	\


LOCAL_SRC_FILES := \
	$(wildcard $(LOCAL_PATH)/code/Collada/*.cpp) \
	$(wildcard $(LOCAL_PATH)/code/FBX/*.cpp) \
	$(wildcard $(LOCAL_PATH)/code/CApi/*.cpp) \
	$(wildcard $(LOCAL_PATH)/code/Common/*.cpp) \
	$(wildcard $(LOCAL_PATH)/code/PostProcessing/*.cpp) \
	$(wildcard $(LOCAL_PATH)/code/include/DefaultLogger.hpp) \
    $(wildcard $(LOCAL_PATH)/code/include/LogStream.hpp) \
    $(wildcard $(LOCAL_PATH)/code/include/Logger.hpp) \
    $(wildcard $(LOCAL_PATH)/code/include/NullLogger.hpp) \
	$(wildcard $(LOCAL_PATH)/code/Material/*.cpp) \
	$(wildcard $(LOCAL_PATH)/contrib/irrXML/*.cpp) \
	$(wildcard $(LOCAL_PATH)/contrib/unzip/*.c) \

LOCAL_LDLIBS += -llog -lz
#LOCAL_LDFLAGS += -lz
LOCAL_CFLAGS := -DASSIMP_BUILD_NO_EXPORT			\
				-DASSIMP_BUILD_NO_COMPRESSED_X		\
				-DASSIMP_BUILD_NO_X_IMPORTER		\
                -DASSIMP_BUILD_NO_AMF_IMPORTER		\
                -DASSIMP_BUILD_NO_3DS_IMPORTER		\
                -DASSIMP_BUILD_NO_MD3_IMPORTER		\
                -DASSIMP_BUILD_NO_MDL_IMPORTER		\
                -DASSIMP_BUILD_NO_MD2_IMPORTER		\
                -DASSIMP_BUILD_NO_PLY_IMPORTER		\
                -DASSIMP_BUILD_NO_ASE_IMPORTER		\
                -DASSIMP_BUILD_NO_OBJ_IMPORTER		\
                -DASSIMP_BUILD_NO_HMP_IMPORTER		\
                -DASSIMP_BUILD_NO_SMD_IMPORTER		\
                -DASSIMP_BUILD_NO_MDC_IMPORTER		\
                -DASSIMP_BUILD_NO_MD5_IMPORTER		\
                -DASSIMP_BUILD_NO_STL_IMPORTER		\
                -DASSIMP_BUILD_NO_LWO_IMPORTER		\
                -DASSIMP_BUILD_NO_DXF_IMPORTER		\
                -DASSIMP_BUILD_NO_NFF_IMPORTER		\
                -DASSIMP_BUILD_NO_RAW_IMPORTER		\
                -DASSIMP_BUILD_NO_SIB_IMPORTER		\
                -DASSIMP_BUILD_NO_OFF_IMPORTER		\
                -DASSIMP_BUILD_NO_AC_IMPORTER		\
                -DASSIMP_BUILD_NO_BVH_IMPORTER		\
                -DASSIMP_BUILD_NO_IRRMESH_IMPORTER	\
                -DASSIMP_BUILD_NO_IRR_IMPORTER		\
                -DASSIMP_BUILD_NO_Q3D_IMPORTER		\
                -DASSIMP_BUILD_NO_B3D_IMPORTER		\
                -DASSIMP_BUILD_NO_TERRAGEN_IMPORTER	\
                -DASSIMP_BUILD_NO_CSM_IMPORTER		\
                -DASSIMP_BUILD_NO_3D_IMPORTER		\
                -DASSIMP_BUILD_NO_LWS_IMPORTER		\
                -DASSIMP_BUILD_NO_OGRE_IMPORTER		\
                -DASSIMP_BUILD_NO_OPENGEX_IMPORTER	\
                -DASSIMP_BUILD_NO_MS3D_IMPORTER		\
                -DASSIMP_BUILD_NO_COB_IMPORTER		\
                -DASSIMP_BUILD_NO_BLEND_IMPORTER	\
                -DASSIMP_BUILD_NO_Q3BSP_IMPORTER	\
                -DASSIMP_BUILD_NO_NDO_IMPORTER		\
                -DASSIMP_BUILD_NO_IFC_IMPORTER		\
                -DASSIMP_BUILD_NO_XGL_IMPORTER		\
                -DASSIMP_BUILD_NO_ASSBIN_IMPORTER	\
                -DASSIMP_BUILD_NO_GLTF_IMPORTER		\
                -DASSIMP_BUILD_NO_C4D_IMPORTER		\
                -DASSIMP_BUILD_NO_3MF_IMPORTER		\
                -DASSIMP_BUILD_NO_X3D_IMPORTER		\
                -DASSIMP_BUILD_NO_MMD_IMPORTER		\
                -DASSIMP_BUILD_NO_STEP_IMPORTER     \
                -DASSIMP_BUILD_NO_OWN_ZLIB

include $(BUILD_SHARED_LIBRARY)