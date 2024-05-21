add_library(tianli_frame STATIC)
add_library(tianli::frame ALIAS tianli_frame)

target_sources(tianli_frame
    PUBLIC
        frame.include.h
    PRIVATE
        frame.include.cpp
        capture/capture.include.h
        capture/capture.bitblt.h
        capture/capture.window_graphics.h
        capture/capture.window_graphics.cpp

        local/local.include.h
        local/local.picture.h
        local/local.video.h
        
        )

set(WITH_DYNAMIC_OPENCV OFF)
set_target_properties(tianli_frame PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:${WITH_DYNAMIC_OPENCV}>:DLL>")
set_target_properties(tianli_frame PROPERTIES FOLDER "cvAutoTrack/modules")

target_link_libraries(tianli_frame
    PUBLIC
        ${opencv_libs}
        d3d11.lib 
        d3dcompiler.lib 
        dxgi.lib)