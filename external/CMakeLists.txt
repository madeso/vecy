###################################################################################################
# glm
add_library(external_glm INTERFACE)
target_include_directories(external_glm SYSTEM
    INTERFACE
        glm
)
add_library(external::glm ALIAS external_glm)


###################################################################################################
# wxWdigets
set(wxWidgets_USE_STATIC ON)
SET(WX_COMPONENTS core aui base stc adv html)
SET(WX_COMPONENTS core base)
# find_package(wxWidgets COMPONENTS ${WX_COMPONENTS} REQUIRED)
find_package(wxWidgets REQUIRED gl core base)
# include("${wxWidgets_USE_FILE}")

add_library(external_wx INTERFACE)
target_include_directories(external_wx INTERFACE ${wxWidgets_INCLUDE_DIRS})
target_link_libraries(external_wx INTERFACE ${wxWidgets_LIBRARIES})
add_library(external::wx ALIAS external_wx)


###################################################################################################
# open color
add_library(open_color INTERFACE)
target_include_directories(open_color
    INTERFACE
        SYSTEM open-color/include
)
add_library(external::open_color ALIAS open_color)

