add_executable(SDLShooter)

target_sources(SDLShooter PRIVATE main.cpp)

target_link_libraries(
  SDLShooter
  PRIVATE $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
          $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
          $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
          $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
          $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>)

# 1. 定义资源目录
set(RESOURCE_SRC_DIR "${PROJECT_SOURCE_DIR}/assets")
set(RESOURCE_DST_DIR "$<TARGET_FILE_DIR:SDLShooter>/assets")

# 2. 使用 GLOB 递归获取源资源文件列表（推荐使用 CONFIGURE_DEPENDS）
file(
  GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS
  RELATIVE "${RESOURCE_SRC_DIR}"
  "${RESOURCE_SRC_DIR}/*")

# 3. 创建自定义目标用于增量复制资源
add_custom_target(
  CopyAssets ALL
  COMMENT "Copying only changed asset files..."
  VERBATIM)

# 4. 为每个资源文件添加复制命令
foreach(RSRC_FILE IN LISTS RESOURCE_FILES)
  get_filename_component(RSRC_DIR "${RSRC_FILE}" DIRECTORY)

  # 为每个文件定义复制命令
  add_custom_command(
    TARGET CopyAssets
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCE_DST_DIR}/${RSRC_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RESOURCE_SRC_DIR}/${RSRC_FILE}" "${RESOURCE_DST_DIR}/${RSRC_FILE}"
    COMMENT "Checking: ${RSRC_FILE}"
    WORKING_DIRECTORY "${RESOURCE_SRC_DIR}" # 确保相对路径正确
  )
endforeach()

# 5. 为可执行目标添加依赖
add_dependencies(SDLShooter CopyAssets)
