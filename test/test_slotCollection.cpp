#include "gtest/gtest.h"
#include <tuple>
#include <tree/slot/slotCollection.hpp>
#include <tree/node/node.hpp>
#include <tree/slot/cache.hpp>

#include <utility>
#include <vector>

#define DIM_GROUP(T) std::tuple<std::integral_constant<std::size_t, 1>, T>, std::tuple<std::integral_constant<std::size_t, 2>, T>, std::tuple<std::integral_constant<std::size_t, 3>, T>

template <typename T>
struct SlotCollectionTest: public ::testing::Test { 
    static const std::size_t dim = std::tuple_element<0, T>::type::value;
    using value_type = typename std::tuple_element<1, T>::type;
};

typedef ::testing::Types<DIM_GROUP(unsigned short), DIM_GROUP(unsigned int), DIM_GROUP(std::size_t)> SlotCollectionTypes;
TYPED_TEST_CASE(SlotCollectionTest, SlotCollectionTypes);

TYPED_TEST(SlotCollectionTest, constructor)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slotCollection<dim, value_type> SC{2, 10, 10, 10};
    EXPECT_EQ( SC.capacity(), 2 );
    EXPECT_EQ( SC.size(), 1 );
    node_type n{1};
    SC.insert(n);
    EXPECT_EQ( SC[0]->capacity(), 10 );
    EXPECT_EQ( SC[0]->size(), 1 );
    // remark: we don't set capacity for slotCollection and slot in the copy constructor
    slotCollection<dim, value_type> SCcopy{SC};
    EXPECT_EQ( SCcopy.capacity(), 1 );
    EXPECT_EQ( SCcopy.size(), 1 );
    EXPECT_EQ( SCcopy[0]->capacity(), 1 );
    EXPECT_EQ( SCcopy[0]->size(), 1 );
}

TYPED_TEST(SlotCollectionTest, swap)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slotCollection<dim, value_type> SC1{2, 10, 10, 11};
    slotCollection<dim, value_type> SC2{10, 5, 5, 6};
    node_type n1{1}, n2{2};
    SC1.insert(n1);
    SC1.insert(n2);
    SC2.insert(n1);
    
    std::swap(SC1, SC2);
    EXPECT_EQ( SC1[0]->size(), 1 );
    EXPECT_EQ( SC1.slot_min_size, 5 );
    EXPECT_EQ( SC1.slot_max_size, 6 );

    EXPECT_EQ( SC2[0]->size(), 2 );
    EXPECT_EQ( SC2.slot_min_size, 10 );
    EXPECT_EQ( SC2.slot_max_size, 11 );
}

TYPED_TEST(SlotCollectionTest, findSlot)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;
    using slot_type = slot<dim, value_type>;
    
    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    SC.push_back(std::make_shared<slot_type>(2, 4, 10));
    SC[0]->s2 = 2;
    node_type n1{1}, n2{3}, n3{5};
    EXPECT_EQ(SC.findSlot(n1, 0, SC.size()-1), 0); 
    EXPECT_EQ(SC.findSlot(n2, 0, SC.size()-1), 1); 
}

TYPED_TEST(SlotCollectionTest, ubound)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;
    using slot_type = slot<dim, value_type>;
    
    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    SC.push_back(std::make_shared<slot_type>(10));
    SC[0]->s2 = node_type::AllOnes[0];
    SC[1]->s1 = node_type::AllOnes[0];
    node_type n1{0}, n2{node_type::AllOnes[0] + (node_type::Xbit>>dim)};
    n2.set_level(1);
    EXPECT_EQ(SC.ubound(n1).get(), SC[0].get());
    EXPECT_EQ(SC.ubound_hashed(n1.hash()).get(), SC[0].get());
    EXPECT_EQ(SC.ubound(n2).get(), SC[1].get());
    EXPECT_EQ(SC.ubound_hashed(n2.hash()).get(), SC[1].get()); 
}

TYPED_TEST(SlotCollectionTest, compress)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;
    using slot_type = slot<dim, value_type>;
    
    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    
    const node_type n1{1};
    const node_type n2(2 + node_type::voidbit);

    SC.insert(n1);
    SC.insert(n2);

    EXPECT_EQ( SC.nbNodes(), 2 );
  
    SC[0]->setMark( node_type::voidbit ); // TODO: mark update should be triggered in slotCollection.
    SC.compress();
    
    EXPECT_EQ( SC.nbNodes(), 1 );
}

TYPED_TEST(SlotCollectionTest, nbNodes)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;
    using slot_type = slot<dim, value_type>;
    
    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    SC.push_back(std::make_shared<slot_type>(10));

    const std::size_t N = 100;
    EXPECT_EQ( SC.nbNodes(), 0 );
    for ( std::size_t i = 0; i < N; ++i )
    {
        SC.insert( node_type(i) );
        EXPECT_EQ( SC.nbNodes(), i+1 );
    }
}

TYPED_TEST(SlotCollectionTest, count)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    node_type n1{1}, n2{2}, n3{3};
    SC.insert(n1);
    SC.insert(n2);
    
    EXPECT_EQ( SC.count( n1 ), 1 );
    EXPECT_EQ( SC.count( n2 ), 1 );
    EXPECT_EQ( SC.count( n3 ), 0 );
}

TYPED_TEST(SlotCollectionTest, count_with_cache)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;
    using cache_type = Cache<dim, value_type>;

    cache_type cache{};

    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    const node_type n1{1}, n2{2}, n3{3};
    SC.insert(n1, cache);
    SC.insert(n2, cache);
    
    EXPECT_EQ( SC.count( n1, cache ), 1 );
    EXPECT_EQ( SC.count( n2, cache ), 1 );
    EXPECT_EQ( SC.count( n3, cache ), 0 );
}

TYPED_TEST(SlotCollectionTest, nbNodesByLevel)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    node_type n1{1}; n1.set_level(0);
    node_type n2{2}; n2.set_level(1);
    node_type n3{3}; n3.set_level(1);

    SC.insert(n1);
    SC.insert(n2);
    SC.insert(n3);
    
    // TODO: adding node in different slots.

    const auto level_count = SC.nbNodesByLevel();

    EXPECT_EQ( level_count[0], 1 );
    EXPECT_EQ( level_count[1], 2 );
    EXPECT_EQ( level_count[2], 0 );
}

TYPED_TEST(SlotCollectionTest, maxSlotSize)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    const node_type n1{1}, n2{2}, n3{3};

    SC.insert(n1);
    SC.insert(n2);
    SC.insert(n3);
    
    // TODO: adding node in different slots.

    EXPECT_EQ( SC.maxSlotSize(), 3 );
}

TYPED_TEST(SlotCollectionTest, copyInArray)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    const node_type n1{1}, n2{2}, n3{3};

    SC.insert(n1);
    SC.insert(n2);
    SC.insert(n3);
    
    // TODO: adding node in different slots.
    
    std::vector<node_type> array( 3 );
    SC.copyInArray( array );

    EXPECT_EQ( array[0], n1.hash() );
    EXPECT_EQ( array[1], n2.hash() );
    EXPECT_EQ( array[2], n3.hash() );
}

TYPED_TEST(SlotCollectionTest, clearFreeBits)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    const node_type n1{1};
    const node_type n2(n1.value + node_type::voidbit);

    SC.insert(n2);

    EXPECT_NE( (*SC[0])[0].value, n1.hash().value );

    SC.clearFreeBits();
    
    EXPECT_EQ( (*SC[0])[0].value, n1.hash().value );
}

TYPED_TEST(SlotCollectionTest, clear)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;
    using slot_type = slot<dim, value_type>;
    
    slotCollection<dim, value_type> SC{2, 10, 10, 11};
    
    const node_type n1{1}, n2{2}, n3{3};

    SC.insert(n1);
    SC.insert(n2);
    SC.insert(n3);

    EXPECT_EQ( SC.nbNodes(), 3 );
  
    SC.clear();
    
    EXPECT_EQ( SC.nbNodes(), 0 );
}
