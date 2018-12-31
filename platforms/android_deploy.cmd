call "android_build.cmd"
call "android_install.cmd"

call adb shell am start -n com.my.exampleapp/com.my.exampleapp.MainActivity
