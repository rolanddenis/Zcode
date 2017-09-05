#pragma once

#include <tree/node/cell.hpp>
#include <tree/slot/slot.hpp>

template<std::size_t dim, typename TValue = std::size_t>
using CellPack = Slot<Cell<dim, TValue>>;

template<std::size_t dim, typename TValue = std::size_t>
using PackCollection = Slot<CellPack<dim, TValue>>;