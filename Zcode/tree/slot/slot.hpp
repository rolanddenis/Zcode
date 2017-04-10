#pragma once
#include <vector>
#include <algorithm>

#include <tree/node/node.hpp>

template<class ForwardIt, class UnaryPredicate, class UnaryFunction>
void myreplace_if(ForwardIt first, ForwardIt last,
                UnaryPredicate p, UnaryFunction f)
{
    for (; first != last; ++first) {
        if(p(*first)) {
            f(*first);
        }
    }
}
////////////////////////////////////////////////////////////////////////////
///
/// slot stucture, used to store Nodes with a hash code in an interval.
///
/// \brief slot structures, store a set of Nodes.
////////////////////////////////////////////////////////////////////////////
template<std::size_t dim, typename value_type=std::size_t> 
struct slot: private std::vector<Node<dim, value_type>>
{
    using node_type = Node<dim, value_type>;
    using parent = std::vector<node_type>;
    using parent::operator[];
    using parent::push_back;
    using parent::insert;
    using parent::erase;
    using parent::reserve;
    using parent::resize;
    using parent::begin;
    using parent::end;
    using parent::cbegin;
    using parent::cend;
    using parent::size;
    using parent::capacity;

    static const value_type FreeBitsPart = node_type::FreeBitsPart;
    static const value_type voidbit = node_type::voidbit;
    static const value_type maskpos = node_type::maskpos;
    static const value_type decal = dim*node_type::nlevels;

    node_type s1, s2;

private:
    std::size_t slotrank;//!<rank in a collection of slots.
    std::size_t startrank;//!< used to define a rank for all Nodes, in a slot collection.
    mutable unsigned char slotMark=0; // to put various marks on the slot.
    bool vCanBeDeleted=true;

public:
    slot() = default;
    slot(const slot&) = default;

    slot(node_type _s1, node_type _s2, std::size_t size):s1{_s1}, s2{_s2}
    {
        reserve(size);
    }

    slot(std::size_t size)
    {
        reserve(size);
    }

    //! find a Node.
    //! \param x: Node *hashed*
    //! \note we *do* *not* *check* if x is hashed.
    inline auto find(node_type const & node)
    {
        auto predicate = [&](auto const& n){return node==n;};
        auto index = std::find_if(cbegin(), cend(), predicate);
        return index;
    }

    //! put a Node at the end.
    //! \param x a *hashed* node (not checked).
    inline void put(node_type x)
    {
        push_back(x);
    }

    //! put a vector of Node's at the end.
    //! \param x vector of  *hashed* nodes (not checked).
    inline void put(std::vector<node_type> x)
    {
        insert(end(), x.cbegin(), x.cend());
    }

    //! position of the last entered Node in v[]
    inline int lastPos() const 
    {
        return size()-1;
    }

    //! compress: ie, supress void Nodes with a given value
    //! \param N  supress nodes for which N&node !=0
    inline void compress(node_type N=node_type::voidbit)
    {
        const unsigned char mask = (N.value&FreeBitsPart)>>decal;
        if((mask&slotMark)==mask)
        {
            auto index = std::remove_if(begin(), end(), [&](auto const& n){return ((n.value&FreeBitsPart)==(N.value&FreeBitsPart));});
            resize(std::distance(begin(), index));
            slotMark -= mask;
        }
    }

    //! compress all marked Nodes. ie, supress Nodes with any value in
    //! the FreeBitsPart or marked as void
    inline void compressany()
    {
        auto index = std::remove_if(begin(), end(), [&](auto const& n){return (n.value&FreeBitsPart);});
        resize(std::distance(begin(), index));
        slotMark=0;
    }

    //! compress: ie, supress void Nodes with given values
    //! \param N  test value
    //! \param M  test value
    //! \note  a Node K are supressed iff K&N==N *or* K&M==M 
    inline void compress(node_type N, node_type M)
    {
        const unsigned char n1 =(N.value&FreeBitsPart)>>decal;
        const unsigned char m1 =(M.value&FreeBitsPart)>>decal;
        if(((slotMark&n1)==n1)||((slotMark&m1)==m1))
        {
            auto index = std::remove_if(begin(), end(), [&](auto const& n){return ((n.value&FreeBitsPart)==(N.value&FreeBitsPart)) || ((n.value&FreeBitsPart)==(M.value&FreeBitsPart));});
            resize(std::distance(begin(), index));
            if((slotMark&n1)==n1) slotMark-=n1;
            if((slotMark&m1)==m1) slotMark-=m1;
        }
    }

    // How is the slot marked?
    inline node_type howMasked() const
    {
        return {slotMark<<decal};
    }

    //! mark the slot with some value.
    //! \param N associated value.
    inline void setMark(node_type N)
    {
        const unsigned char mask = (N.value&FreeBitsPart)>>decal;
        if(!(slotMark&mask)) slotMark+=mask;
    }

    //! get the mark tag.
    inline unsigned char getMark()
    {
        return slotMark;
    }

    // ! change the mark the marked Nodes
    // ! \param a ancient mark
    // ! \param b new mark.
    inline void changeMark(node_type a, node_type b)
    {
        const unsigned char amask = (a.value&FreeBitsPart)>>decal;
        const unsigned char bmask = (b.value&FreeBitsPart)>>decal;
        if(slotMark&amask)
        {
            slotMark -= amask;
            if(!(slotMark&bmask)) 
                slotMark += bmask;
            myreplace_if(begin(), end(), [&](auto &n){return n.value&a.value;}, [&](auto &n){n.value += -a.value+b.value;});
        }
    }

    //! count Nodes such that Node&N == N.
    //! \param N test value.
    inline auto countAndEq(node_type N) const
    {
        return std::count_if(begin(), end(), [&](auto const& n){return (n.value&N.value)==N.value;});
    }

    //! suppress a given mark
    //! \param N the mark
    //! \note throw an exception if not marked "mark".
    inline void unsetMark(node_type N)
    {
        const unsigned char mask = (N.value&FreeBitsPart)>>decal;
        if(slotMark&mask) 
            slotMark -= mask;
    }

    //! test if the slot has been marked by an other mark as "mark"
    //! \param mark for the test.
    bool markedOther(node_type mark)
    {
        node_type N{FreeBitsPart-mark};
        const unsigned char m = (N.value&FreeBitsPart)>>decal;
        return slotMark&m;
    }

    //! does this slot contains void Nodes ?
    inline bool hasvoidNodes() const 
    {
        return slotMark&voidbit;
    }

    //! mark the slot as containing void Nodes.
    inline void sethasvoidNodes()
    {
        setMark(voidbit);
    }

    //! make a logical "and" of all Nodes with a given value.
    //! \param N the value.
    inline void And(node_type N)
    {
        std::for_each(begin(), end(), [&](auto &n){n.value&=N.value;});
    }

    //! Tag, ie add  some value to all Nodes.
    //! \note a "and" with the value must be zero. Not tested if DEBUG
    //! is not set. We do not want to set a value to Nodes, but to tag them.
    //! \param N the value.
    inline void setTag(node_type N)
    {
        std::for_each(begin(), end(), [&](auto &n){n.value|=N.value;});
        const unsigned char m = (N.value&FreeBitsPart)>>decal;
        if(!(slotMark&m)) slotMark+=m;
    }

    //! empty the slot. Do not change s1 and s2, do not deallocate space.
    inline void empty()
    {
        resize(0);
    }    

    //! cut the slot in nc slots.
    //! \note sizes of the resulting slots are not garanted to be equal.
    //! \param nc number of slots
    //! \param s  result. Array of nc slot*.
    //inline void cut(int nc,slot* __restrict__ s[]) const
    inline auto cut(std::size_t const nc)
    {
        std::vector<slot> slarray(nc);

        std::sort(begin(), end(), [&](auto &n1, auto &n2){return (n1.value&maskpos)<(n2.value&maskpos);});

        std::size_t sizec = size()/nc;
        for(std::size_t i=0; i<nc-1; ++i)
        {
            slarray[i].insert(slarray[i].begin(), begin()+i*sizec, begin()+(i+1)*sizec);
            slarray[i].s1 = (*this)[i*sizec].value&maskpos;
            slarray[i].s2 = (*this)[(i+1)*sizec-1].value&maskpos;
        }
        std::size_t i = nc-1;
        slarray[i].insert(slarray[i].begin(), begin()+i*sizec, end());
        slarray[i].s1 = (*this)[i*sizec].value&maskpos;
        slarray[i].s2 = (*this)[size()-1].value&maskpos;
        return slarray;
    }

  //! cut this slot in 2 slots, at position pos, and then shrink it.
  //! the returned slot is the first part containing v[0,pos[
  //! \param pos 
  //! \param s2new value for s2 of the *new* slot, and s1 of this slot.
  //! \note we do not check that pos is correct, except if DEBUG is set.
  //! \note for s2new: position part only; tested only if DEBUG set.
  inline auto cutBefore(std::size_t pos, node_type s2new)
  {
      slot newslot(s1, s2new, capacity());
      newslot.insert(newslot.begin(), begin(), begin()+pos);

      erase(begin(), begin()+pos);
      s1 = s2new;
      return newslot;
  }
    
};

