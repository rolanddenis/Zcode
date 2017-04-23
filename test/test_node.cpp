#include "gtest/gtest.h"
#include <tuple>
#include <tree/node/node.hpp>
#include <tree/node/family.hpp>
#include <tree/node/util.hpp>

#define DIM_GROUP(T) std::tuple<std::integral_constant<std::size_t, 1>, T>, std::tuple<std::integral_constant<std::size_t, 2>, T>, std::tuple<std::integral_constant<std::size_t, 3>, T>

template <typename T>
struct NodeTest: public ::testing::Test { 
    static const std::size_t dim = std::tuple_element<0, T>::type::value;
    using value_type = typename std::tuple_element<1, T>::type;
    static const value_type Xbit = Node<dim, value_type>::Xbit;
    static const value_type Ybit = Node<dim, value_type>::Ybit;
    static const value_type Zbit = Node<dim, value_type>::Zbit;

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

typedef ::testing::Types<DIM_GROUP(unsigned short), DIM_GROUP(unsigned int), DIM_GROUP(std::size_t)> NodeTypes;
TYPED_TEST_CASE(NodeTest, NodeTypes);

TYPED_TEST(NodeTest, constructor)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;

    Node<dim, value_type> node{};
    EXPECT_EQ( node.value, 0 );
}

TYPED_TEST(NodeTest, level)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;

    Node<dim, value_type> node{};
    EXPECT_EQ( node.level(), 0 );
    node.set_level(2);
    EXPECT_EQ( node.level(), 2 );
}

TYPED_TEST(NodeTest, is_void)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    node_type node{};
    EXPECT_EQ( node.isVoid(), false );
    node.value += node_type::voidbit; 
    EXPECT_EQ( node.isVoid(), true );
}

TYPED_TEST(NodeTest, setTags)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    node_type node{};
    node_type voidNode{node_type::voidbit};
    node.setTags(voidNode);
    EXPECT_EQ( node , voidNode );
}

TYPED_TEST(NodeTest, hash)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    node_type node{};
    EXPECT_EQ( node.isHashed(), false );
    auto node_hash = node.hash();
    EXPECT_EQ( node_hash.isHashed(), true );
    auto node_unhash = node_hash.unhash();
    EXPECT_EQ( node_unhash.isHashed(), false );
}

TYPED_TEST(NodeTest, brothers)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    for(std::size_t level=0; level<4; ++level)
    {
        node_type node{};
        std::array<node_type, ipow(2, dim)> b;
        node.set_level(level);
        brothers(node, b);
        // auto btrue = TestFixture::build_brothers(level);
        // for (std::size_t i = 0; i<btrue.size(); ++i)
        // {
        //     b[i].set_level(0);
        //     EXPECT_EQ( b[i].value, btrue[i] );
        // }
    }
}
