#ifndef HMAP_GEN_H
#define HMAP_GEN_H

#include "Generator.hpp"
#include <random>

using chunk = ArraySequence<ArraySequence<float>>;
using column = ArraySequence<float>;

class HeightMapGenerator : public IGenerator<chunk>
{
public:
    HeightMapGenerator();
    HeightMapGenerator( const column& seed, const size_t size = 16, const float amp = 1.0f );
    HeightMapGenerator( const float begin, const size_t size = 16, const float amp = 1.0f );
    
    
    HeightMapGenerator( const HeightMapGenerator& other ) = default;
    HeightMapGenerator& operator=( const HeightMapGenerator& other ) = default;

    HeightMapGenerator( HeightMapGenerator&& other ) = default;
    HeightMapGenerator& operator=( HeightMapGenerator&& other ) = default;

    ~HeightMapGenerator() = default;
public:
    chunk getNext() override;
    chunk get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<chunk> tryGetNext() override;
private:
    chunk nextChunk();
private:
    size_t _size;
    Ordinal _lastMaterialized;
    column _prevEdge;

    
    std::uniform_real_distribution<float> _dist;
    std::mt19937 _rng;
};

#include "HeightMapGenerator.tpp"

#endif // HMAP_GEN_H