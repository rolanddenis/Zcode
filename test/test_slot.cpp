#include "gtest/gtest.h"
#include <tuple>
#include <tree/slot/slot.hpp>
#include <tree/node/node.hpp>

#define DIM_GROUP(T) std::tuple<std::integral_constant<std::size_t, 1>, T>, std::tuple<std::integral_constant<std::size_t, 2>, T>, std::tuple<std::integral_constant<std::size_t, 3>, T>

template <typename T>
struct SlotTest: public ::testing::Test { 
    static const std::size_t dim = std::tuple_element<0, T>::type::value;
    using value_type = typename std::tuple_element<1, T>::type;
};

typedef ::testing::Types<DIM_GROUP(unsigned short), DIM_GROUP(unsigned int), DIM_GROUP(std::size_t)> SlotTypes;
TYPED_TEST_CASE(SlotTest, SlotTypes);

TYPED_TEST(SlotTest, constructor_1)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;

    Node<dim, value_type> s1{0};
    Node<dim, value_type> s2{1};
    slot<dim, value_type> slot{0, 1, 10};
    EXPECT_EQ( slot.s1, s1 );
    EXPECT_EQ( slot.s2, s2 );
    EXPECT_EQ( slot.capacity(), 10 );
    EXPECT_EQ( slot.size(), 0 );
}

TYPED_TEST(SlotTest, constructor_2)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;

    Node<dim, value_type> s1{0};
    Node<dim, value_type> s2{1};
    slot<dim, value_type> slot{0, 1, 10};
    EXPECT_EQ( slot.s1, s1 );
    EXPECT_EQ( slot.s2, s2 );
    EXPECT_EQ( slot.capacity(), 10 );
    EXPECT_EQ( slot.size(), 0 );
}

TYPED_TEST(SlotTest, putNode)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;

    slot<dim, value_type> slot{0, 1, 10};
    Node<dim, value_type> zero{0};
    Node<dim, value_type> one{1};
    slot.put({0});
    slot.put({1});
    EXPECT_EQ( slot.size(), 2 );
    EXPECT_EQ( slot[0], zero );
    EXPECT_EQ( slot[1], one );
}

TYPED_TEST(SlotTest, putNodes)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{0, 1, 10};
    node_type zero{0};
    node_type one{1};
    std::array<node_type, 2> nodes{zero, one};
    slot.put(nodes);
    EXPECT_EQ( slot.size(), 2 );
    EXPECT_EQ( slot[0], zero );
    EXPECT_EQ( slot[1], one );
}

TYPED_TEST(SlotTest, find)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
        nodes[i] = i;
    slot.put(nodes);
    EXPECT_EQ( slot.find({0}), slot.begin() );
    EXPECT_EQ( slot.find({9}), slot.begin() + 9 );
    EXPECT_EQ( slot.find({9}), slot.end() - 1 );
    EXPECT_EQ( slot.find({11}), slot.end() );
}

TYPED_TEST(SlotTest, setmark)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    node_type voidnode{node_type::voidbit};
    slot.setMark(node_type::voidbit);
    EXPECT_EQ( slot.getMark(), node_type::voidbit>>slot.decal );
    EXPECT_EQ( slot.howMasked(), voidnode );
}

TYPED_TEST(SlotTest, changeMark)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    // Mark only odd nodes
    for (std::size_t i=0; i<10; ++i)
    {
        nodes[i] = i;
        if (i&1)
            nodes[i] += node_type::voidbit;
    }
    slot.put(nodes);
    slot.setMark(node_type::voidbit);

    node_type n1{node_type::voidbit}, n2{node_type::voidbit>>1};
    slot.changeMark(n1, n2);
    EXPECT_EQ( slot.getMark(), node_type::voidbit>>1>>slot.decal );

    for (std::size_t i=0; i<10; ++i)
    {
        node_type node{static_cast<value_type>(i)};
        if (i&1)
          node += node_type::voidbit>>1;
        EXPECT_EQ( slot[i], node );
    }
}

TYPED_TEST(SlotTest, unsetMark)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    slot.setMark(node_type::voidbit + (node_type::voidbit>>1));
    node_type n{node_type::voidbit>>1};
    slot.unsetMark(n);
    EXPECT_EQ( slot.getMark(), node_type::voidbit>>slot.decal );
}

TYPED_TEST(SlotTest, markedOther)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    slot.setMark(node_type::voidbit);
    node_type n0{node_type::voidbit};
    node_type n1{node_type::voidbit>>1};
    EXPECT_EQ( slot.markedOther(n0), false );
    EXPECT_EQ( slot.markedOther(n1), true );
}

TYPED_TEST(SlotTest, hasvoidnodes)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;

    slot<dim, value_type> slot{10};
    EXPECT_EQ( slot.hasvoidNodes(), false );
    slot.sethasvoidNodes();
    EXPECT_EQ( slot.hasvoidNodes(), true );
}

TYPED_TEST(SlotTest, compress)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
    {
        nodes[i] = i;
        if (i&1)
            nodes[i] += node_type::voidbit;
    }
    slot.put(nodes);
    slot.setMark(node_type::voidbit);
    // remove all odd nodes
    slot.compress();
    EXPECT_EQ( slot.size(), 5 );
    for (std::size_t i=0; i<5; ++i)
    {
        node_type node{static_cast<value_type>(2*i)};
        EXPECT_EQ( slot[i], node );
    }
}

TYPED_TEST(SlotTest, compressany)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
    {
        nodes[i] = i;
        if (i&1)
            nodes[i] += node_type::voidbit;
        else
            nodes[i] += node_type::voidbit>>1;
    }
    slot.put(nodes);
    slot.setMark(node_type::voidbit + (node_type::voidbit>>1));
    slot.compressany();
    EXPECT_EQ( slot.size(), 0 );
}

TYPED_TEST(SlotTest, compresswithnodes_1)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
    {
        nodes[i] = i;
        if (i&1)
            nodes[i] += node_type::voidbit;
        else
            nodes[i] += node_type::voidbit>>1;
    }
    slot.put(nodes);
    slot.setMark(node_type::voidbit + (node_type::voidbit>>1));
    node_type n1{node_type::voidbit}, n2{node_type::voidbit>>1};
    slot.compress(n1, n2);
    EXPECT_EQ( slot.size(), 0 );
}

TYPED_TEST(SlotTest, compresswithnodes_2)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
    {
        nodes[i] = i;
        if (i&1)
            nodes[i] += node_type::voidbit;
        else
            nodes[i] += node_type::voidbit>>1;
    }
    slot.put(nodes);
    slot.setMark(node_type::voidbit + (node_type::voidbit>>1));
    node_type n1{node_type::voidbit}, n2{node_type::voidbit>>2};
    slot.compress(n1, n2);
    EXPECT_EQ( slot.size(), 5 );
    for (std::size_t i=0; i<5; ++i)
    {
        node_type node{static_cast<value_type>(2*i)};
        EXPECT_EQ( slot[i], node );
    }
}

TYPED_TEST(SlotTest, and)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
        nodes[i] = i + node_type::voidbit;
    slot.put(nodes);
    slot.setMark(node_type::voidbit);
    node_type voidnode{node_type::voidbit};
    slot.And(voidnode);
    for (std::size_t i=0; i<10; ++i)
    {
        EXPECT_EQ( slot[i], voidnode );
    }
}

TYPED_TEST(SlotTest, setTag)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
        nodes[i] = i;
    slot.put(nodes);
    node_type voidnode{node_type::voidbit};
    slot.setTag(voidnode);
    for (std::size_t i=0; i<10; ++i)
    {
        node_type node{static_cast<value_type>(i + node_type::voidbit)};
        EXPECT_EQ( slot[i], node );
    }
    EXPECT_EQ( slot.getMark(), node_type::voidbit>>slot.decal );  
}

TYPED_TEST(SlotTest, empty)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
        nodes[i] = i;
    slot.put(nodes);
    EXPECT_EQ( slot.size(), 10 );
    slot.empty();
    EXPECT_EQ( slot.size(), 0 );
}

TYPED_TEST(SlotTest, cut)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
        nodes[i] = i;
    slot.put(nodes);

    auto slot_cut2 = slot.cut(2);
    for (std::size_t i=0; i<5; ++i)
    {
        node_type node0{static_cast<value_type>(i)};
        node_type node1{static_cast<value_type>(i+5)};
        EXPECT_EQ( slot_cut2[0][i], node0 );
        EXPECT_EQ( slot_cut2[1][i], node1 );
    }
    
    auto slot_cut3 = slot.cut(3);
    EXPECT_EQ( slot_cut3[0].size(), 3 );
    EXPECT_EQ( slot_cut3[1].size(), 3 );
    EXPECT_EQ( slot_cut3[2].size(), 4 );
}

TYPED_TEST(SlotTest, cutBefore)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
        nodes[i] = i;
    slot.put(nodes);

    node_type s2new{1};
    auto new_slot = slot.cutBefore(3, s2new);

    for (std::size_t i=0; i<3; ++i)
    {
        node_type node{static_cast<value_type>(i)};
        EXPECT_EQ( new_slot[i], node );
    }
    EXPECT_EQ( new_slot.s2, s2new );

    for (std::size_t i=0; i<7; ++i)
    {
        node_type node{static_cast<value_type>(i+3)};
        EXPECT_EQ( slot[i], node );
    }
    EXPECT_EQ( slot.s1, s2new );
}

TYPED_TEST(SlotTest, fusion)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot1{5};
    slot<dim, value_type> slot2{5};
    std::array<node_type, 5> nodes1;
    std::array<node_type, 5> nodes2;
    for (std::size_t i=0; i<5; ++i)
    {
        nodes1[i] = i;
        nodes2[i] = i+5;
    }
    slot1.put(nodes1);
    slot2.put(nodes2);

    slot1.fusion(slot2);
    EXPECT_EQ( slot1.size(), 10 );
    for (std::size_t i=0; i<10; ++i)
    {
        node_type node{static_cast<value_type>(i)};
        EXPECT_EQ( slot1[i], node );
    }
}

TYPED_TEST(SlotTest, sort)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
        nodes[i] = 10 - i;
    slot.put(nodes);

    slot.sort();
    for (std::size_t i=0; i<10; ++i)
    {
        node_type node{static_cast<value_type>(i+1)};
        EXPECT_EQ( slot[i], node );
    }
}

TYPED_TEST(SlotTest, cutdown)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    node_type node{1};
    slot.put(node);
    EXPECT_EQ( slot.cutdown(), true );
    EXPECT_EQ( slot.capacity(), 2 );
    EXPECT_EQ( slot.cutdown(3), false );
}

TYPED_TEST(SlotTest, forgetFreeBits)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
        nodes[i] = i + node_type::voidbit;
    slot.put(nodes);
    slot.forgetFreeBits();
    for (std::size_t i=0; i<10; ++i)
    {
        node_type node{static_cast<value_type>(i)};
        EXPECT_EQ( slot[i], node );
    }
}

TYPED_TEST(SlotTest, uniq)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<5; ++i)
    {
        nodes[2*i] = i;
        nodes[2*i + 1] = i;
    }
    slot.put(nodes);
    slot.uniq();
    EXPECT_EQ( slot.size(), 5 );
    for (std::size_t i=0; i<5; ++i)
    {
        node_type node{static_cast<value_type>(i)};
        EXPECT_EQ( slot[i], node );
    }
}

TYPED_TEST(SlotTest, testwellformed)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot1{10};
    slot<dim, value_type> slot2{0, 1, 10};
    node_type node{2};
    slot1.put(node);
    slot2.put(node);
    EXPECT_EQ( slot1.testWellFormed(), true );
    EXPECT_EQ( slot2.testWellFormed(), false );
}

TYPED_TEST(SlotTest, exaequo)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slot<dim, value_type> slot{10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<5; ++i)
    {
        nodes[2*i] = i;
        nodes[2*i + 1] = i;
    }
    slot.put(nodes);
    EXPECT_EQ( slot.exaequo(), true );
    slot.uniq();
    EXPECT_EQ( slot.exaequo(), false );
}

TYPED_TEST(SlotTest, dumprestore)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;
    std::string filename="test_dump_restore.txt";

    node_type s1{1}, s2{2};
    slot<dim, value_type> slot_dump{s1, s2, 10};
    std::array<node_type, 10> nodes;
    for (std::size_t i=0; i<10; ++i)
        nodes[i] = i;
    slot_dump.put(nodes);
    slot_dump.setStartrank(2);

    std::ofstream file_out;
    file_out.open(filename.c_str(), std::ios::out|std::ios::binary );     
    slot_dump.dump(file_out);
    file_out.close();

    slot<dim, value_type> slot_restore{10};
    std::ifstream file_in;
    file_in.open(filename.c_str(), std::ios::in|std::ios::binary );     
    slot_restore.restore(file_in);
    file_in.close();

    EXPECT_EQ( slot_restore.s1, s1 );
    EXPECT_EQ( slot_restore.s2, s2 );
    EXPECT_EQ( slot_restore.Startrank(), 2 );
    for (std::size_t i=0; i<10; ++i)
    {
        node_type node{static_cast<value_type>(i)};
        EXPECT_EQ( slot_restore[i], node );
    }
}
