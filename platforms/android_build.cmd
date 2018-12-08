cd android
call gradlew assembleDebug

cd ..

mkdir ..\bin\android

copy /y android\build\android-app-cmake\outputs\apk\debug\*.apk ..\bin\android
