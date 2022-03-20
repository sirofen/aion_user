set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(PATCHER_PATTERN F1023FAB401234FA)

message(STATUS "Downloading patcher")

include(ExternalProject)
ExternalProject_Add(
        patch_binary
        GIT_REPOSITORY          https://github.com/sirofen/aik_usermode-patch_binary.git
        GIT_TAG                 master

        BUILD_ALWAYS            false

        INSTALL_COMMAND         cmake -E echo "Skipping install step."
)

ExternalProject_Get_Property(patch_binary BINARY_DIR)

message(STATUS "Adding post build patch command")

set(PATCH_BINARY "${BINARY_DIR}/$<CONFIG>/patch_binary.exe")

add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${PATCH_BINARY} ${CMAKE_BINARY_DIR}/$<CONFIG>/${CMAKE_PROJECT_NAME}.exe ${PATCHER_PATTERN}
        COMMENT "Patching binary"
)

