# sfAudio

Standalone port of the SFML Audio Engine from the version `2.4.0`. `sfAudio` replaces all the `SFML` specific functionalities with standards except for the `sf::Time` and `sf::Vector3f`,
which is just a `struct` that has `x`, `y`, `z` values.

# How To Use

The API is the same as `SFML`'s, so check out SFML's official documentation [here](http://www.sfml-dev.org/tutorials/2.4/audio-sounds.php).

You can find the compiled libraries [here](https://drive.google.com/drive/folders/0B2b4SnYRu-h_MGJudE5yRm5Ba00?usp=sharing).
I only have the `x86` and `x64` versions compiled with `MSVC14`. So If you compile them for other compilers, please contact me so I can add it for other people to use.
You can check out the `versions.txt` file to see the versions of the each library.
To include the library, add it to your `CMakeLists.txt` file.

```cmake
# Add sfAudio
set(SFAUDIO_ROOT D:/Development/SourceTree/GitHub/sfAudio)
add_subdirectory(${SFAUDIO_ROOT} sfAudio)

# Set the include directory
include_directories(
    ${SFAUDIO_INC}
)

# Link to your app
target_link_libraries(${APP_NAME} ${SFAUDIO_EXT_LIBS})

# Copy the required DLLs to your bin directory
pre_build(${APP_NAME}
  COMMAND ${CMAKE_COMMAND} -E copy ${SFAUDIO_ROOT}/external/libs-msvc2015/x86/openal32.dll ${APP_BIN_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy ${SFAUDIO_ROOT}/external/libs-msvc2015/x86/libogg.dll ${APP_BIN_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy ${SFAUDIO_ROOT}/external/libs-msvc2015/x86/libvorbis.dll ${APP_BIN_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy ${SFAUDIO_ROOT}/external/libs-msvc2015/x86/libvorbisfile.dll ${APP_BIN_DIR}
)
```
