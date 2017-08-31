#include "gtest/gtest.h"
#include <tuple>
#include <tree/slot/slotCollection.hpp>
#include <tree/node/node.hpp>

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
    SC.put(n, SC.cache);
    EXPECT_EQ( SC[0]->capacity(), 10 );
    EXPECT_EQ( SC[0]->size(), 1 );
    // remark: we don't set capacity for slotCollection and slot in the copy constructor
    slotCollection<dim, value_type> SCcopy{SC};
    EXPECT_EQ( SCcopy.capacity(), 1 );
    EXPECT_EQ( SCcopy.size(), 1 );
    EXPECT_EQ( SCcopy[0]->capacity(), 1 );
    EXPECT_EQ( SCcopy[0]->size(), 1 );
}

TYPED_TEST(SlotCollectionTest, clone)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slotCollection<dim, value_type> SC{2, 10, 10, 10};
    EXPECT_EQ( SC.capacity(), 2 );
    EXPECT_EQ( SC.size(), 1 );
    node_type n{1};
    SC.put(n, SC.cache);

    slotCollection<dim, value_type> SCclone{};
    SCclone.clone(SC);
    EXPECT_EQ( SCclone.capacity(), 1 );
    EXPECT_EQ( SCclone.size(), 1 );
    EXPECT_EQ( SCclone[0]->capacity(), SC[0]->size()*node_type::treetype );
    EXPECT_EQ( SCclone[0]->size(), 0 );
}

TYPED_TEST(SlotCollectionTest, swap)
{
    auto const dim = TestFixture::dim;
    using value_type = typename TestFixture::value_type;
    using node_type = Node<dim, value_type>;

    slotCollection<dim, value_type> SC1{2, 10, 10, 11};
    slotCollection<dim, value_type> SC2{10, 5, 5, 6};
    node_type n1{1}, n2{2};
    SC1.put(n1, SC1.cache);
    SC1.put(n2, SC1.cache);
    SC2.put(n1, SC2.cache);
    
    SC1.swap(SC2);
    EXPECT_EQ( SC1[0]->size(), 1 );
    EXPECT_EQ( SC1.dupsize, 5 );
    EXPECT_EQ( SC1.breaksize, 6 );

    EXPECT_EQ( SC2[0]->size(), 2 );
    EXPECT_EQ( SC2.dupsize, 10 );
    EXPECT_EQ( SC2.breaksize, 11 );
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
    SC.push_back(std::make_shared<slot_type>(10));
   
    SC.compress();
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
        SC.put( node_type(i), SC.cache );
        EXPECT_EQ( SC.nbNodes(), i+1 );
    }
}
