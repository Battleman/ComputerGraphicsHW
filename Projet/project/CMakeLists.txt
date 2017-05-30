get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# these files are copied only once
file(GLOB TEXTURES
    skybox/skybox_texture.tga

    quad/quad_texture.tga
    quad/quad_texture.tga

    skybox/sky.jpg
    skybox/right.jpg
    skybox/top.jpg
    skybox/bottom.jpg
    skybox/back.jpg
    skybox/front.jpg
    skybox/left.jpg

    quad/RockTexture.jpg
    quad/SandTexture.jpg
    quad/ForestTexture.jpg
    quad/normalMap.png
    quad/waterDUDV.png)
copy_files_once(${TEXTURES})

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
    screenquad/*.glsl
    skybox/*.glsl
    quad/*.glsl
    water/*.glsl)
deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
