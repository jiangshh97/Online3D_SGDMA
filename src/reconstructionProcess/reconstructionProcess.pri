HEADERS += \
    $$PWD/common/image_common.h \
    $$PWD/common/model_common.h \
    $$PWD/common/pointcloud_common.h \
    $$PWD/common/workpiece_params.h \
    $$PWD/abstractreconalgorithm.h \
    $$PWD/abstractreconfactory.h \
    $$PWD/reconstructioncommon.h \
    $$PWD/reconstructionprocess.h \
    $$PWD/reconstructionalgorithm.h \
    $$PWD/steelangle/steelangle_params.h \
    $$PWD/steelangle/steelanglerecon.h \
    $$PWD/steelangle/yoloseginference.h


SOURCES += \
    $$PWD/common/image_common.cpp \
    $$PWD/common/model_common.cpp \
    $$PWD/common/pointcloud_common.cpp \    
    $$PWD/reconstructionprocess.cpp \
    $$PWD/reconstructionalgorithm.cpp \
    $$PWD/steelangle/steelanglerecon.cpp \
    $$PWD/steelangle/yoloseginference.cpp



