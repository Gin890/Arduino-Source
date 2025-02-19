/*  Waterfill Session
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_Kernels_Waterfill_Session_TPP
#define PokemonAutomation_Kernels_Waterfill_Session_TPP

#include <set>
#include <map>
#include "Common/Cpp/Exceptions.h"
#include "Kernels/Kernels_BitSet.h"
#include "Kernels/BinaryMatrix/Kernels_BinaryMatrix_t.h"
#include "Kernels/BinaryMatrix/Kernels_PackedBinaryMatrixCore.h"
#include "Kernels/BinaryMatrix/Kernels_SparseBinaryMatrixCore.h"
#include "Kernels_Waterfill_Session.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{
namespace Kernels{
namespace Waterfill{




template <typename Tile, typename TileRoutines>
class WaterfillSession_t final : public WaterfillSession{
public:
    WaterfillSession_t() = default;
    WaterfillSession_t(PackedBinaryMatrixCore<Tile>& source)
        : m_source(&source)
        , m_object(source.width(), source.height())
        , m_busy_tiles(source.width(), source.height())
        , m_object_tiles(source.width(), source.height())
    {}

    void set_source(PackedBinaryMatrixCore<Tile>& source){
        m_source = &source;
        if (m_object.width() < source.width() || m_object.height() < source.height()){
            m_object = PackedBinaryMatrixCore<Tile>(source.width(), source.height());
            m_busy_tiles = BitSet2D(source.width(), source.height());
            m_object_tiles = BitSet2D(source.width(), source.height());
        }
    }
    virtual void set_source(PackedBinaryMatrix_IB& source) override{
        if (source.type() != Tile::TYPE){
            throw InternalProgramError(nullptr, PA_CURRENT_FUNCTION, "Mismatching matrix format.");
        }
        set_source(static_cast<PackedBinaryMatrix_t<Tile>&>(source).get());
    }

    size_t tile_width() const{ return m_source->tile_width(); }
    size_t tile_height() const{ return m_source->tile_height(); }

    virtual std::unique_ptr<WaterfillIterator> make_iterator(size_t min_area) override;

    virtual bool find_object_on_bit(
        WaterfillObject& object, bool keep_object,
        size_t x, size_t y
    ) override;
    bool find_object_in_tile(
        WaterfillObject& object, bool keep_object,
        size_t tile_x, size_t tile_y
    );


private:
    bool find_object(
        WaterfillObject& object, bool keep_object,
        size_t tile_x, size_t tile_y,
        size_t bit_x, size_t bit_y
    );
#if 0
    void clear_dirty_tiles(){
        for (auto tile : m_dirty_tiles){
            m_object.tile(tile.first, tile.second).set_zero();
        }
        m_dirty_tiles.clear();
    }
#endif


private:
    //  Source matrix. This is zeroed as objects are found.
    PackedBinaryMatrixCore<Tile>* m_source = nullptr;

    //  Current object.
    PackedBinaryMatrixCore<Tile> m_object;
//    std::vector<std::pair<size_t, size_t>> m_dirty_tiles;

    //  Reused scratch buffers. Only used inside "find_object()".
    BitSet2D m_busy_tiles;
    BitSet2D m_object_tiles;
};




template <typename Tile, typename TileRoutines>
class WaterfillIterator_t final : public WaterfillIterator{
public:
    WaterfillIterator_t(WaterfillSession_t<Tile, TileRoutines>& session, size_t min_area)
        : m_session(session)
        , m_min_area(min_area)
    {}
    virtual bool find_next(WaterfillObject& object, bool keep_object) override;

private:
    WaterfillSession_t<Tile, TileRoutines>& m_session;
    size_t m_min_area;
    size_t m_tile_row = 0;
    size_t m_tile_col = 0;
};





template <typename Tile, typename TileRoutines>
std::unique_ptr<WaterfillIterator> WaterfillSession_t<Tile, TileRoutines>::make_iterator(size_t min_area){
    return std::make_unique<WaterfillIterator_t<Tile, TileRoutines>>(*this, min_area);
}


template <typename Tile, typename TileRoutines>
bool WaterfillSession_t<Tile, TileRoutines>::find_object_on_bit(
    WaterfillObject& object, bool keep_object,
    size_t x, size_t y
){
    if (!m_source->get(x, y)){
        return false;
    }

    std::set<TileIndex> busy;
    std::map<TileIndex, Tile> obj;

    size_t tile_x = x / PackedBinaryMatrixCore<Tile>::Tile::WIDTH;
    size_t tile_y = y / PackedBinaryMatrixCore<Tile>::Tile::HEIGHT;
    size_t bit_x = x % PackedBinaryMatrixCore<Tile>::Tile::WIDTH;
    size_t bit_y = y % PackedBinaryMatrixCore<Tile>::Tile::HEIGHT;

    return find_object(object, keep_object, tile_x, tile_y, bit_x, bit_y);
}
template <typename Tile, typename TileRoutines>
bool WaterfillSession_t<Tile, TileRoutines>::find_object_in_tile(
    WaterfillObject& object, bool keep_object,
    size_t tile_x, size_t tile_y
){
    Tile& start = m_source->tile(tile_x, tile_y);

    size_t bit_x, bit_y;
    if (!TileRoutines::find_bit(bit_x, bit_y, start)){
        return false;
    }

    return find_object(object, keep_object, tile_x, tile_y, bit_x, bit_y);
}


template <typename Tile, typename TileRoutines>
bool WaterfillSession_t<Tile, TileRoutines>::find_object(
    WaterfillObject& object, bool keep_object,
    size_t tile_x, size_t tile_y,
    size_t bit_x, size_t bit_y
){
//    clear_dirty_tiles();

    size_t tile_width = m_source->tile_width();
    size_t tile_height = m_source->tile_height();

    //  Set first tile.
    size_t x = tile_x;
    size_t y = tile_y;
    m_busy_tiles.set(x, y);
    m_object_tiles.set(x, y);
    m_object.tile(x, y).set_bit(bit_x, bit_y);

    //  Special case for isolated bit.
    bool first_expand = true;
#if 1
    {
        bool top_edge = bit_y == 0;
        bool bottom_edge = bit_y == Tile::HEIGHT - 1;
//        bool left_edge = bit_x == 0;
//        bool right_edge = bit_x == Tile::WIDTH - 1;

        Tile& tile = m_source->tile(x, y);
        uint64_t rowC = tile.row(bit_y);
        uint64_t rowA = top_edge ? 0 : tile.row(bit_y - 1);
        uint64_t rowB = bottom_edge ? 0 : tile.row(bit_y + 1);
        uint64_t test = rowA | rowB | (rowC << 1) | (rowC >> 1);
        test &= (uint64_t)1 << bit_x;

        first_expand = test;
    }
#endif

    //  Iterate Waterfill...
    while (m_busy_tiles.pop(x, y)){
        Tile& mask = m_source->tile(x, y);
        Tile& tile = m_object.tile(x, y);

        //  Expand current tile.
        if (first_expand){
            TileRoutines::waterfill_expand(mask, tile);
        }else{
            mask.andnot(tile);
        }
        first_expand = true;

        size_t current_x, current_y;
        if (y > 0 && tile.top() != 0){
            current_y = y - 1;
            const Tile& neighbor_mask = m_source->tile(x, current_y);
            if (TileRoutines::waterfill_touch_bottom(neighbor_mask, m_object.tile(x, current_y), tile)){
                m_busy_tiles.set(x, current_y);
                m_object_tiles.set(x, current_y);
            }
        }
        current_y = y + 1;
        if (current_y < tile_height && tile.bottom() != 0){
            const Tile& neighbor_mask = m_source->tile(x, current_y);
            if (TileRoutines::waterfill_touch_top(neighbor_mask, m_object.tile(x, current_y), tile)){
                m_busy_tiles.set(x, current_y);
                m_object_tiles.set(x, current_y);
            }
        }
        uint64_t row_or = TileRoutines::row_or(tile);
        if (x > 0 && (row_or & 1)){
            current_x = x - 1;
            const Tile& neighbor_mask = m_source->tile(current_x, y);
            if (TileRoutines::waterfill_touch_right(neighbor_mask, m_object.tile(current_x, y), tile)){
                m_busy_tiles.set(current_x, y);
                m_object_tiles.set(current_x, y);
            }
        }
        current_x = x + 1;
        const uint64_t MASK = (uint64_t)1 << (Tile::WIDTH - 1);
        if (current_x < tile_width && (row_or & MASK)){
            const Tile& neighbor_mask = m_source->tile(current_x, y);
            if (TileRoutines::waterfill_touch_left(neighbor_mask, m_object.tile(current_x, y), tile)){
                m_busy_tiles.set(current_x, y);
                m_object_tiles.set(current_x, y);
            }
        }
    }

    //  Compute stats.
    size_t tile_min_x = (size_t)0 - 1;
    size_t tile_min_y = (size_t)0 - 1;
    size_t tile_max_x = 0;
    size_t tile_max_y = 0;

    WaterfillObject stats;
    stats.body_x = tile_x * Tile::WIDTH + bit_x;
    stats.body_y = tile_y * Tile::HEIGHT + bit_y;

    std::unique_ptr<std::map<TileIndex, Tile>> sparse_set;
    if (keep_object){
        sparse_set = std::make_unique<std::map<TileIndex, Tile>>();
    }

    while (m_object_tiles.pop(x, y)){
//        m_dirty_tiles.emplace_back(x, y);
        Tile& tile = m_object.tile(x, y);

        if (sparse_set){
            (*sparse_set)[TileIndex{x, y}] = tile;
        }

        uint64_t popcount, sum_x, sum_y;
        popcount = TileRoutines::popcount_sumcoord(sum_x, sum_y, tile);
//        if (popcount == 7 && tile_x == 16 && tile_y == 73){
//            cout << popcount << ", " << sum_x << ", " << sum_y << endl;
//            cout << item.second.dump() << endl;
//        }
        stats.accumulate_body(
            x * Tile::WIDTH, y * Tile::HEIGHT,
            popcount, sum_x, sum_y
        );
        if (!(tile_min_x < x && x < tile_max_x && tile_min_y < y && y < tile_max_y)){
            size_t cmin_x, cmax_x, cmin_y, cmax_y;
            TileRoutines::boundaries(tile, cmin_x, cmax_x, cmin_y, cmax_y);
            stats.accumulate_boundary(
                x * Tile::WIDTH, y * Tile::HEIGHT,
                cmin_x, cmax_x, cmin_y, cmax_y
            );
        }
        tile_min_x = std::min(tile_min_x, x);
        tile_max_x = std::max(tile_max_x, x);
        tile_min_y = std::min(tile_min_y, y);
        tile_max_y = std::max(tile_max_y, y);

        tile.set_zero();
    }

#if 0
    cout << "x = (" << stats.m_min_x << "," << stats.m_max_x << ")" << endl;
    cout << "y = (" << stats.m_min_y << "," << stats.m_max_y << ")" << endl;
    cout << "area = " << stats.m_area << endl;
    cout << "sum x = " << stats.m_sum_x << endl;
    cout << "sum y = " << stats.m_sum_y << endl;
#endif

    object = stats;

    if (sparse_set){
        auto ptr = std::make_unique<SparseBinaryMatrix_t<Tile>>(m_source->width(), m_source->height());
        ptr->get().set_data(std::move(*sparse_set));
        object.object = std::move(ptr);
    }

    return true;
}




template <typename Tile, typename TileRoutines>
bool WaterfillIterator_t<Tile, TileRoutines>::find_next(WaterfillObject& object, bool keep_object){
    while (m_tile_row < m_session.tile_height()){
        while (m_tile_col < m_session.tile_width()){
            while (true){
                //  Not object found. Move to next tile.
                if (!m_session.find_object_in_tile(object, keep_object, m_tile_col, m_tile_row)){
                    break;
                }
                //  Object too small. Skip it.
                if (object.area < m_min_area){
                    continue;
                }
                return true;
            }
            m_tile_col++;
        }
        m_tile_col = 0;
        m_tile_row++;
    }
    return false;
}





}
}
}
#endif
