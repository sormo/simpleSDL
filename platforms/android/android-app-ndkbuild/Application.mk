# Targeted android API version
APP_PLATFORM := android-14

# Enable C++11 support with exception and rtti
APP_CPPFLAGS += -std=c++1z
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -frtti
APP_STL := c++_static
