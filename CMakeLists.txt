project(sfAudio)
cmake_minimum_required(VERSION 3.1)

get_filename_component(SFAUDIO_ROOT "./" ABSOLUTE)

if (NOT DEFINED AR)
    message("[sfAudio]: Architecture is not set, defaulting to x86")
    set(AR x86)
endif()

if (MSVC)
    set(SFAUDIO_EXT_LIBS
        external/libs-msvc2015/${AR}/flac.lib
        external/libs-msvc2015/${AR}/ogg.lib
        external/libs-msvc2015/${AR}/openal32.lib
        external/libs-msvc2015/${AR}/vorbis.lib
        external/libs-msvc2015/${AR}/vorbisenc.lib
        external/libs-msvc2015/${AR}/vorbisfile.lib
    )
else()
    message(FATAL_ERROR "Current compiler is not supported. Exiting.")
endif(MSVC)

set(SFAUDIO_INC
    ${SFAUDIO_ROOT}/external/headers/
    ${SFAUDIO_ROOT}/include
    CACHE LIST "Set the list of include paths for sfAudio"
)

include_directories(
    ${SFAUDIO_ROOT}/external/headers/
    ${SFAUDIO_ROOT}/include
)

set(SFAUDIO_SRC
    src/System/FileInputStream.cpp
    src/System/MemoryInputStream.cpp
    src/System/Time.cpp
    src/ALCheck.cpp
    src/AlResource.cpp
    src/AudioDevice.cpp
    src/InputSoundFile.cpp
    src/Listener.cpp
    src/Music.cpp
    src/OutputSoundFile.cpp
    src/Sound.cpp
    src/SoundBuffer.cpp
    src/SoundBufferRecorder.cpp
    src/SoundFileFactory.cpp
    src/SoundFileReaderFlac.cpp
    src/SoundFileReaderOgg.cpp
    src/SoundFileReaderWav.cpp
    src/SoundFileWriterFlac.cpp
    src/SoundFileWriterOgg.cpp
    src/SoundFileWriterWav.cpp
    src/SoundRecorder.cpp
    src/SoundSource.cpp
    src/SoundStream.cpp
)


set(SFAUDIO_HEADERS
    include/sfAudio/System/FileInputStream.hpp
    include/sfAudio/System/InputStream.hpp
    include/sfAudio/System/MemoryInputStream.hpp
    include/sfAudio/System/Vector3.h
    include/sfAudio/System/Time.hpp
    include/sfAudio/ALCheck.hpp
    include/sfAudio/AlResource.hpp
    include/sfAudio/AudioDevice.hpp
    include/sfAudio/InputSoundFile.hpp
    include/sfAudio/Listener.hpp
    include/sfAudio/Music.hpp
    include/sfAudio/OutputSoundFile.hpp
    include/sfAudio/Sound.hpp
    include/sfAudio/SoundBuffer.hpp
    include/sfAudio/SoundBufferRecorder.hpp
    include/sfAudio/SoundFileFactory.hpp
    include/sfAudio/SoundFileReader.hpp
    include/sfAudio/SoundFileReaderFlac.hpp
    include/sfAudio/SoundFileReaderOgg.hpp
    include/sfAudio/SoundFileReaderWav.hpp
    include/sfAudio/SoundFileWriter.hpp
    include/sfAudio/SoundFileWriterFlac.hpp
    include/sfAudio/SoundFileWriterOgg.hpp
    include/sfAudio/SoundFileWriterWav.hpp
    include/sfAudio/SoundRecorder.hpp
    include/sfAudio/SoundSource.hpp
    include/sfAudio/SoundStream.hpp
    include/sfAudio/SoundFileFactory.inl
)

add_library(sfAudio STATIC
    ${SFAUDIO_SRC}
    )


set_target_properties(sfAudio
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )

