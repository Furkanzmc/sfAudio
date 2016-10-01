////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2016 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "sfAudio/SoundFileReaderWav.hpp"
#include "sfAudio/System/InputStream.hpp"
#include <algorithm>
#include <cctype>
#include <cassert>
#include <iostream>

namespace
{
// The following functions read integers as little endian and
// return them in the host byte order

bool decode(sf::InputStream &stream, uint8_t &value)
{
    return stream.read(&value, sizeof(value)) == sizeof(value);
}

bool decode(sf::InputStream &stream, uint16_t &value)
{
    unsigned char bytes[sizeof(value)];
    if (stream.read(bytes, sizeof(bytes)) != sizeof(bytes))
        return false;

    value = bytes[0] | (bytes[1] << 8);

    return true;
}

bool decode(sf::InputStream &stream, uint64_t &value)
{
    unsigned char bytes[sizeof(value)];
    if (stream.read(bytes, sizeof(bytes)) != sizeof(bytes))
        return false;

    value = bytes[0] | (bytes[1] << 8);

    return true;
}

bool decode24bit(sf::InputStream &stream, uint32_t &value)
{
    unsigned char bytes[3];
    if (stream.read(bytes, sizeof(bytes)) != sizeof(bytes))
        return false;

    value = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16);

    return true;
}

bool decode(sf::InputStream &stream, uint32_t &value)
{
    unsigned char bytes[sizeof(value)];
    if (stream.read(bytes, sizeof(bytes)) != sizeof(bytes))
        return false;

    value = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);

    return true;
}

const uint64_t mainChunkSize = 12;
}

namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
bool SoundFileReaderWav::check(InputStream &stream)
{
    char header[mainChunkSize];
    if (stream.read(header, sizeof(header)) < static_cast<uint64_t>(sizeof(header)))
        return false;

    return (header[0] == 'R') && (header[1] == 'I') && (header[2] == 'F') && (header[3] == 'F')
           && (header[8] == 'W') && (header[9] == 'A') && (header[10] == 'V') && (header[11] == 'E');
}


////////////////////////////////////////////////////////////
SoundFileReaderWav::SoundFileReaderWav() :
    m_stream(NULL),
    m_bytesPerSample(0),
    m_dataStart(0),
    m_dataEnd(0)
{
}


////////////////////////////////////////////////////////////
bool SoundFileReaderWav::open(InputStream &stream, Info &info)
{
    m_stream = &stream;

    if (!parseHeader(info)) {
        std::cerr << "Failed to open WAV sound file (invalid or unsupported file)" << std::endl;
        return false;
    }

    return true;
}


////////////////////////////////////////////////////////////
void SoundFileReaderWav::seek(uint64_t sampleOffset)
{
    assert(m_stream);

    m_stream->seek(m_dataStart + sampleOffset * m_bytesPerSample);
}


////////////////////////////////////////////////////////////
uint64_t SoundFileReaderWav::read(int16_t *samples, uint64_t maxCount)
{
    assert(m_stream);

    uint64_t count = 0;
    while ((count < maxCount) && (m_stream->tell() < m_dataEnd)) {
        switch (m_bytesPerSample) {
        case 1: {
            uint8_t sample = 0;
            if (decode(*m_stream, sample))
                *samples++ = (static_cast<int16_t>(sample) - 128) << 8;
            else
                return count;
            break;
        }

        case 2: {
            uint32_t sample = 0;
            if (decode(*m_stream, sample))
                *samples++ = sample;
            else
                return count;
            break;
        }

        case 3: {
            uint32_t sample = 0;
            if (decode24bit(*m_stream, sample))
                *samples++ = sample >> 8;
            else
                return count;
            break;
        }

        case 4: {
            uint32_t sample = 0;
            if (decode(*m_stream, sample))
                *samples++ = sample >> 16;
            else
                return count;
            break;
        }

        default: {
            assert(false);
            return 0;
        }
        }

        ++count;
    }

    return count;
}


////////////////////////////////////////////////////////////
bool SoundFileReaderWav::parseHeader(Info &info)
{
    assert(m_stream);

    // If we are here, it means that the first part of the header
    // (the format) has already been checked
    char mainChunk[mainChunkSize];
    if (m_stream->read(mainChunk, sizeof(mainChunk)) != sizeof(mainChunk))
        return false;

    // Parse all the sub-chunks
    bool dataChunkFound = false;
    while (!dataChunkFound) {
        // Parse the sub-chunk id and size
        char subChunkId[4];
        if (m_stream->read(subChunkId, sizeof(subChunkId)) != sizeof(subChunkId))
            return false;
        uint32_t subChunkSize = 0;
        if (!decode(*m_stream, subChunkSize))
            return false;

        // Check which chunk it is
        if ((subChunkId[0] == 'f') && (subChunkId[1] == 'm') && (subChunkId[2] == 't') && (subChunkId[3] == ' ')) {
            // "fmt" chunk

            // Audio format
            uint16_t format = 0;
            if (!decode(*m_stream, format))
                return false;
            if (format != 1) // PCM
                return false;

            // Channel count
            uint16_t channelCount = 0;
            if (!decode(*m_stream, channelCount))
                return false;
            info.channelCount = channelCount;

            // Sample rate
            uint32_t sampleRate = 0;
            if (!decode(*m_stream, sampleRate))
                return false;
            info.sampleRate = sampleRate;

            // Byte rate
            uint32_t byteRate = 0;
            if (!decode(*m_stream, byteRate))
                return false;

            // Block align
            uint16_t blockAlign = 0;
            if (!decode(*m_stream, blockAlign))
                return false;

            // Bits per sample
            uint16_t bitsPerSample = 0;
            if (!decode(*m_stream, bitsPerSample))
                return false;
            if (bitsPerSample != 8 && bitsPerSample != 16 && bitsPerSample != 24 && bitsPerSample != 32) {
                std::cerr << "Unsupported sample size: " << bitsPerSample << " bit (Supported sample sizes are 8/16/24/32 bit)" << std::endl;
                return false;
            }
            m_bytesPerSample = bitsPerSample / 8;

            // Skip potential extra information (should not exist for PCM)
            if (subChunkSize > 16) {
                if (m_stream->seek(m_stream->tell() + subChunkSize - 16) == -1)
                    return false;
            }
        }
        else if ((subChunkId[0] == 'd') && (subChunkId[1] == 'a') && (subChunkId[2] == 't') && (subChunkId[3] == 'a')) {
            // "data" chunk

            // Compute the total number of samples
            info.sampleCount = subChunkSize / m_bytesPerSample;

            // Store the start and end position of samples in the file
            m_dataStart = m_stream->tell();
            m_dataEnd = m_dataStart + info.sampleCount * m_bytesPerSample;

            dataChunkFound = true;
        }
        else {
            // unknown chunk, skip it
            if (m_stream->seek(m_stream->tell() + subChunkSize) == -1)
                return false;
        }
    }

    return true;
}

} // namespace priv

} // namespace sf
