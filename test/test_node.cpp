#include "gtest/gtest.h"
#include <tuple>
#include <tree/node/definitions.hpp>
#include <tree/node/cell.hpp>
#include <tree/node/family.hpp>
#include <tree/node/util.hpp>
#include <tree/slot/slot.hpp>

#define DIM_GROUP(T, isSlot) std::tuple<std::integral_constant<std::size_t, 1>, T, isSlot>, std::tuple<std::integral_constant<std::size_t, 2>, T, isSlot>, std::tuple<std::integral_constant<std::size_t, 3>, T, isSlot>

struct test_slot {};
struct test_cell {};

template <typename T>
struct CellTest: public ::testing::Test { 
    static const std::size_t dim = std::tuple_element<0, T>::type::value;
    using value_type = typename std::tuple_element<1, T>::type;
    using isCell = typename std::tuple_element<2, T>::type;

    using node_type = typename std::conditional<std::is_same<isCell, test_cell>::value, 
                                                Cell<dim, value_type>, 
                                                Slot<Cell<dim, value_type>>>::type;

    using definition = definitions<dim, value_type>;

    static const value_type Xbit = definition::Xbit;
    static const value_type Ybit = definition::Ybit;
    static const value_type Zbit = definition::Zbit;

    auto build_brothers(std::integral_constant<std::size_t, 1>)
    {
        std::array<value_type, 2> brothers{0, Xbit};
        return brothers;
    }
    auto build_brothers(std::integral_constant<std::size_t, 2>)
    {
        std::array<value_type, 4> brothers{0, Ybit, Xbit, Xbit+Ybit};
        return brothers;
    }
    auto build_brothers(std::integral_constant<std::size_t, 3>)
    {
        std::array<value_type, 8> brothers{0, Ybit, Xbit, Xbit+Ybit,
                                           Zbit, Ybit + Zbit, Xbit + Zbit, Xbit + Ybit + Zbit};
        return brothers;
    }

    auto build_brothers(std::size_t level)
    {
        auto brothers = build_brothers(std::integral_constant<std::size_t, dim>{});
        std::for_each(brothers.begin(), brothers.end(), [&level](auto &b){b >>= level*dim;});
        return brothers;
    }
};

typedef ::testing::Types<DIM_GROUP(unsigned short, test_cell),
                         DIM_GROUP(unsigned int  , test_cell),
                         DIM_GROUP(std::size_t   , test_cell),
                         DIM_GROUP(unsigned short, test_slot),
                         DIM_GROUP(unsigned int  , test_slot),
                         DIM_GROUP(std::size_t   , test_slot)> CellTypes;
TYPED_TEST_CASE(CellTest, CellTypes);

TYPED_TEST(CellTest, constructor)
{
    constexpr auto dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = typename TestFixture::node_type;

    node_type cell{0};
    EXPECT_EQ( cell.value, 0 );
}

TYPED_TEST(CellTest, level)
{
    constexpr auto dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = typename TestFixture::node_type;

    node_type cell{0};
    EXPECT_EQ( cell.level(), 0 );
    cell.setLevel(2);
    EXPECT_EQ( cell.level(), 2 );
}

TYPED_TEST(CellTest, isVoid)
{
    constexpr auto dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using definition = typename TestFixture::definition;
    using node_type = typename TestFixture::node_type;

    node_type cell{0};
    EXPECT_EQ( cell.isVoid(), false );
    cell.value += definition::voidbit; 
    EXPECT_EQ( cell.isVoid(), true );
}

TYPED_TEST(CellTest, setTags)
{
    constexpr auto dim = TestFixture::dim;
    constexpr auto voidbit = TestFixture::definition::voidbit;
    using value_type = typename TestFixture::value_type;
    using node_type = typename TestFixture::node_type;

    node_type cell{0};
    cell.setTags(voidbit);
    EXPECT_EQ( cell.value , voidbit );
}

TYPED_TEST(CellTest, hasTags)
{
    constexpr auto dim = TestFixture::dim;
    constexpr auto voidbit = TestFixture::definition::voidbit;
    using value_type = typename TestFixture::value_type;
    using node_type = typename TestFixture::node_type;

    node_type cell{0};
    EXPECT_EQ( cell.hasTags(voidbit) , false );
    cell.setTags(voidbit);
    EXPECT_EQ( cell.hasTags(voidbit) , true );
}

TYPED_TEST(CellTest, hash)
{
    constexpr auto dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = typename TestFixture::node_type;

    node_type cell{0};
    EXPECT_EQ( cell.isHashed(), false );
    node_type cell_hash = cell.hash();
    EXPECT_EQ( cell_hash.isHashed(), true );
    node_type cell_unhash = cell_hash.unhash();
    EXPECT_EQ( cell_unhash.isHashed(), false );
}

TYPED_TEST(CellTest, brothers)
{
    constexpr auto dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using definition = typename TestFixture::definition;
    using node_type = typename TestFixture::node_type;

    node_type cell{0};

    for ( std::size_t level = 0; level < definition::nlevels; ++level)
    {
        node_type cell{0};
        std::array<value_type, ipow(2, dim)> b;
        cell.setLevel(level);
        brothers(cell, b);
        auto btrue = TestFixture::build_brothers(level);
        for (std::size_t i = 0; i<btrue.size(); ++i)
        {
            node_type c = b[i];
            c.setLevel(0);
            EXPECT_EQ( c.value, btrue[i] );
        }
    }
}

TYPED_TEST(CellTest, box)
{
    constexpr auto dim = TestFixture::dim;
    constexpr std::size_t stencil = 1; 

    using value_type = typename TestFixture::value_type;
    using node_type = typename TestFixture::node_type;

    std::array<value_type, ipow(2*stencil+1, dim)> b;
    node_type cell{0};
    boxNeighbors<stencil>(cell, b);
    // TODO: add the test
}

TYPED_TEST(CellTest, star)
{
    constexpr auto dim = TestFixture::dim;
    constexpr std::size_t stencil = 1; 

    using value_type = typename TestFixture::value_type;
    using node_type = typename TestFixture::node_type;

    std::array<value_type, 2*stencil*dim> b;
    node_type cell{0};
    starNeighbors<stencil>(cell, b);
    // TODO: add the test
}