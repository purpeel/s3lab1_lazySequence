HeightMapGenerator::HeightMapGenerator() 
: _size(16), _lastMaterialized(0), _prevEdge() {
    _rng = std::mt19937( std::random_device{}() );
    _dist = std::uniform_real_distribution<float>( -1.0f, 1.0f );
    
    for (size_t i = 0; i < _size; i++) {
        _prevEdge.append(1.0f);
    }
}
    
HeightMapGenerator::HeightMapGenerator( const column& seed, const size_t size, const float amp )
: _size( size ),  _lastMaterialized(0), _prevEdge( seed ) {
    _rng = std::mt19937( std::random_device{}() );
    _dist = std::uniform_real_distribution<float>( -amp, amp ); 
}

HeightMapGenerator::HeightMapGenerator( const float begin, const size_t size, const float amp )
: _size( size ), _lastMaterialized(0), _prevEdge() {
    _rng = std::mt19937( std::random_device{}() );
    _dist = std::uniform_real_distribution<float>( -amp, amp ); 

    for (size_t i = 0; i < _size; i++) {
        _prevEdge.append(begin);
    }
}

chunk HeightMapGenerator::getNext() {
    if (!hasNext()) { 
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    auto res = nextChunk();
    _prevEdge = res[_size - 1];
    return res;
}

chunk HeightMapGenerator::get( const Ordinal& index ) {
    if (index.isTransfinite()) {
        throw Exception( Exception::ErrorCode::INCONSISTENT_CHUNK_ACCESS );
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

bool HeightMapGenerator::hasNext() {
    return true;
}

Option<chunk> HeightMapGenerator::tryGetNext() {
    if (hasNext()) {
        return Option<chunk>( getNext() );
    } else {
        return Option<chunk>();
    }
}

// keep in mind that structure-wise chunk is not a usual 2d-array:
// when accessing elements of chunk, you firstly need to access needed column and then row.
// its done to assign the _prevEdge as the last column of the newly-generated chunk to increase readability.
// tl;dr - chunk is a transponed matrix
chunk HeightMapGenerator::nextChunk() {
    chunk res;
    for (size_t col = 0; col < _size; col++) {
        column c;
        for (size_t row = 0; row < _size; row++) {
            float adjLeft, adjUpper;
            if (col == 0) { adjLeft =  _prevEdge[row]; } 
            else          { adjLeft = res[col-1][row]; }

            if (row == 0) { adjUpper = adjLeft; }
            else          { adjUpper = c[row-1];}

            c.append( (adjLeft + adjUpper) * 0.5f + _dist( _rng ) );
        }
        res.append( c );
    }
    return res;
}