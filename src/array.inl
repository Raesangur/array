/**
 * \file
 * \author  Pascal-Emmanuel Lachance
 * \p       https://www.github.com/Raesangur
 * ------------------------------------------------------------------------------------------------
 * MIT License
 * Copyright (c) 2020 Pascal-Emmanuel Lachance | Ràësangür
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once
#include "./array.hpp"

#include <sstream>
#include <ostream>

namespace pel
{


/**
 **************************************************************************************************
 * \brief       Overload of the left-shift << operator to print an array's
 *              content to an output stream.
 *
 * \param       os_:  Left-hand-side output stream.
 * \param       arr_: Right-hand-side vector to print.
 *
 * \retval      std::ostream&: Reference the output stream after appending data.
 *
 * \note        This method is not directly part of the pel::array class, and is rather appended
 *              to the std::ostream class.
 *************************************************************************************************/
template<typename ItemType, std:size_t ItemCount>
inline static std::ostream&
operator<<(std::ostream& os_, const array<ItemType, ItemCount>& arr_) noexcept
{
    /* Add capacity and length header */
    os_ << "Length: [" << vec_.length() << "]\n";

    for(ItemType& element : arr_)
    {
        os_ << element << '\n';
    }

    return os_;
}


/*************************************************************************************************/
/* CONSTRUCTORS & DESTRUCTORS ------------------------------------------------------------------ */
/*************************************************************************************************/


/**
 **************************************************************************************************
 * \brief       Default-value constructor for the array class.
 *
 * \param       value_:  Value to initialize all the elements initially allocated with.
 *************************************************************************************************/
template<typename ItemType, std::size_t ItemCount>
array<ItemType, ItemCount>::array(const ItemType& value_)
{
    /* clang-format off */
    std::for_each(begin(), end(),
                  [&](ItemType& item)
                  {
                      AllocatorTraits::construct(m_allocator, &item, value_);
                  });
    /* clang-format on */
}


/**
 **************************************************************************************************
 * \brief       Iterator-based copy constructor for the array class.
 *
 * \param       beginIterator_: Begin iterator of another array to start copying from.
 * \param       endIterator_:   End iterator of another array to end the copy.
 *************************************************************************************************/
template<typename ItemType, std::size_t ItemCount>
array<ItemType, ItemCount>::array(const IteratorType beginIterator_,
                                  const IteratorType endIterator_)
{
    check_fit(endIterator_ - beginIterator_);	

    std::copy(beginIterator_, endIterator_, begin());
}


/**
 **************************************************************************************************
 * \brief       Copy constructor for the array class.
 *
 * \param       otherArray_: Array to copy data from.
 *************************************************************************************************/
template<typename ItemType, std::size_t ItemCount>
template<std::size_t OtherSize>
array<ItemType, ItemCount>::array(const aray<ItemType, OtherSize>& otherArray)
{
    check_fit(otherArray_.length());
    
    std::copy(otherArray_.begin(), otherArray_.end(), begin());
}

template<typename ItemType, std::size_t ItemCount>
vector<ItemType, AllocatorType>::array(const array& otherArray_)
{
    std::copy(otherArray_.begin(), otherArray_.end(), begin());
}

/**
 **************************************************************************************************
 * \brief       Copy assignment operator for the array class.
 *
 * \param       copy_: Array to copy data from.
 *************************************************************************************************/
template<typename ItemType, std::size_t ItemCount>
template<std::size_t OtherSize>
typename array<ItemType, ItemCount>&
array<ItemType, ItemCount>::operator=(const array<ItemType, OtherSize>& copy_)
{
    check_fit(copy_.length());

    std::copy(copy_.begin(), copy_.begin(), begin());
}

template<typename ItemType, typename AllocatorType>
vector<ItemType, AllocatorType>&
vector<ItemType, AllocatorType>::operator=(const vector<ItemType, AllocatorType>& copy_)
{
    return operator=<ItemType, AllocatorType>(copy_);
}


/**
 **************************************************************************************************
 * \brief       Move constructor for the vector class.
 *
 * \param       otherVector_: Vector to move data from.
 * \param       alloc_:       Allocator to use for all memory allocations
 *              [defaults : AllocatorType{}]
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
template<typename OtherAllocatorType>
vector<ItemType, AllocatorType>::vector(vector<ItemType, OtherAllocatorType>&& move_,
                                        AllocatorType&                         alloc_)
: container_base{alloc_},
  m_beginIterator{std::move(move_.m_beginIterator)},
  m_endIterator{std::move(move_.m_endIterator)},
  m_capacity{move_.m_capacity}
{
}

/**
 **************************************************************************************************
 * \brief       Move assignment operator for the vector class.
 *
 * \param       move__: Vector to move data from.
 *
 * \note        Will do nothing if attempting to move a vector into itself
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
template<typename OtherAllocatorType>
typename vector<ItemType, AllocatorType>&
vector<ItemType, AllocatorType>::operator=(vector<ItemType, OtherAllocatorType>&& move_)
{
    if(this != std::addressof(move_))
    {
        /* Grab the other vector's resources */
        m_allocator     = move_.get_allocator();
        m_beginIterator = move_.begin();
        m_endIterator   = move_.end();
        m_capacity      = move_.capacity();

        /* Invalidate the other vector */
        move_.m_beginIterator = IteratorType{nullptr};
        move_.m_endIterator   = IteratorType{nullptr};
    }
    return *this;
}

/**
 **************************************************************************************************
 * \brief       Initializer list constructor for the vector class.
 *
 * \param       ilist_: Initializer list of all the values to put in a new vector.
 * \param       alloc_: Allocator to use for all memory allocations
 *              [defaults : AllocatorType{}]
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
vector<ItemType, AllocatorType>::vector(InitializerListType ilist_, const AllocatorType& alloc_)
: container_base{alloc_}
{
    vector_constructor(ilist_.size());

    std::copy(ilist_.begin(), ilist_.end(), begin());
    change_size(ilist_.size());
}


/**
 **************************************************************************************************
 * \brief       Move-based constructor for the vector class.
 *              Takes the arguments used to  an `ItemType` object, and forwards them.
 *
 * \param       length_ : Number of elements to create
 * \param       args_ :   Arguments to forward to the `ItemType` constructor
 * \param       alloc_:   Allocator to use for all memory allocations
 *              [defaults : AllocatorType{}]
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
template<typename... Args>
vector<ItemType, AllocatorType>::vector(SizeType length_,
                                        Args&&... args_,
                                        const AllocatorType& alloc_)
: container_base{alloc_}
{
    vector_constructor(length_);
    add_size(length_);

    /* clang-format off */
    auto builder = [&](ItemType& element)
                   {
                       AllocatorTraits::construct(m_allocator,
                                                  &element,
                                                  std::forward<Args>(args_)...);
                   };
    /* clang-format on */
    std::for_each(begin(), end(), builder);
}


/**
 **************************************************************************************************
 * \brief       Generator-taking constructor for the vector class.
 *              Takes a generator function as parameter to create `length_` items in the vector.
 *
 * \param       length_ :   Number of elements to create
 * \param       function_ : Function (returning `ItemType` and taking no arguments) called to
 *                          initialize all the values in the vector.
 * \param       alloc_:     Allocator to use for all memory allocations
 *              [defaults : AllocatorType{}]
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
vector<ItemType, AllocatorType>::vector(SizeType                      length_,
                                        std::function<ItemType(void)> function_,
                                        const AllocatorType&          alloc_)
: container_base{alloc_}
{
    vector_constructor(length_);
    add_size(length_);

    std::generate(begin(), end(), function_);
}


/**
 **************************************************************************************************
 * \brief       Destructor for the vector class.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
vector<ItemType, AllocatorType>::~vector()
{
    /* Free and destroy elements in the allocated memory */
    AllocatorTraits::deallocate(m_allocator, begin().ptr(), capacity());
}


/*************************************************************************************************/
/* ELEMENT ACCESSORS --------------------------------------------------------------------------- */
/*************************************************************************************************/


/**
 **************************************************************************************************
 * \brief       Get a pointer to the beginning of the vector's data space.
 *
 * \retval      ItemType*: Pointer to the beginning of the vector's data.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
[[nodiscard]] inline ItemType*
vector<ItemType, AllocatorType>::data() noexcept
{
    return begin().ptr();
}


/**
 **************************************************************************************************
 * \brief       Get a const pointer to the beginning of the vector's data space.
 *
 * \retval      ItemType*: Const pointer to the beginning of the vector's data.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
[[nodiscard]] inline const ItemType*
vector<ItemType, AllocatorType>::data() const noexcept
{
    return begin().ptr();
}


/**
 **************************************************************************************************
 * \brief       Assign a value to a certain offset in the vector for a certain amount of elements.
 *
 * \param       value_:  Value to assign to the vector.
 * \param       offset_: Offset at which data should be assigned.
 *              [defaults : 0]
 * \param       count_:  Number of elements to be assigned a new value.
 *              [defaults : 1]
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline void
vector<ItemType, AllocatorType>::assign(const ItemType& value_,
                                        DifferenceType  offset_,
                                        SizeType        count_)
{
    if constexpr(vector_safeness == true)
    {
        check_fit(count_);
    }

    std::fill_n(begin() + offset_, count_, value_);
}


/**
 **************************************************************************************************
 * \brief       Assign values to a certain offset in the vector through an initializer list.
 *
 * \param       ilist_:  Values to assign to the vector.
 * \param       offset_: Offset at which data should be assigned.
 *              [defaults : 0]
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline void
vector<ItemType, AllocatorType>::assign(InitializerListType ilist_, DifferenceType offset_)
{
    if constexpr(vector_safeness == true)
    {
        check_fit(ilist_.size());
    }

    std::copy(ilist_.begin(), ilist_.end(), begin() + offset_);
}


/*************************************************************************************************/
/* OPERATOR OVERLOADS -------------------------------------------------------------------------- */
/*************************************************************************************************/


/**
 **************************************************************************************************
 * \brief       Overload of the arithmetic += operator to add an element at the end of the vector.
 *
 * \param       rhs_: Item at the right-hand-size of the addition, to be added at the end of the
 *                    vector.
 *
 * \retval      vector&: Reference the vector itself.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline vector<ItemType, AllocatorType>&
vector<ItemType, AllocatorType>::operator+=(const ItemType& rhs_)
{
    push_back(rhs_);
    return *this;
}


/**
 **************************************************************************************************
 * \brief       Overload of the post-increment ++ operator to reserve one element of memory at the
 *              end of the vector.
 *
 * \retval      vector&: Reference the vector itself.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline const vector<ItemType, AllocatorType>
vector<ItemType, AllocatorType>::operator++(int)
{
    reserve(capacity() + 1);
    return *this;
}


/**
 **************************************************************************************************
 * \brief       Overload of the post-decrement -- operator to free one element of memory at the end
 *              of the vector.
 *
 * \retval      vector&: Reference the vector itself.
 *
 * \note        If the shrinking of the capacity of the vector causes it to shrink smaller than
 *              it's current size, the last element of the vector will be popped back and
 *              destroyed (safely).
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline const vector<ItemType, AllocatorType>
vector<ItemType, AllocatorType>::operator--(int)
{
    if(capacity() == length())
    {
        pop_back();
    }

    reserve(capacity() - 1);
    return *this;
}


/**
 **************************************************************************************************
 * \brief       Overload of the right-shift >> operator to shift the vector's elements to the right.
 *
 * \param       steps_: Shifts to the rights.
 *
 * \retval      vector&: Reference the vector itself.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline vector<ItemType, AllocatorType>&
vector<ItemType, AllocatorType>::operator>>(int steps_)
{
    std::shift_right(cbegin(), cend(), steps_);

    return *this;
}


/**
 **************************************************************************************************
 * \brief       Overload of the left-shift << operator to shift the vector's elements to the left.
 *
 * \param       steps_: Shifts to the left.
 *
 * \retval      vector&: Reference the vector itself.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline vector<ItemType, AllocatorType>&
vector<ItemType, AllocatorType>::operator<<(int steps_)
{
    std::shift_left(cbegin(), cend(), steps_);

    return *this;
}


/*************************************************************************************************/
/* ELEMENT MANAGEMENT -------------------------------------------------------------------------- */
/*************************************************************************************************/

/**
 **************************************************************************************************
 * \brief       Add an element to the end of the vector, after the current last item.
 *
 * \param       value_: Element to push back at the end of the vector.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline void
vector<ItemType, AllocatorType>::push_back(const ItemType& value_)
{
    check_fit(1);

    end().value() = value_;
    add_size(1);
}


/**
 **************************************************************************************************
 * \brief       Add elements from an initializer list to the end of the vector, after the current
 *              last item.
 *
 * \param       ilist_: Initializer list containing elements to push back at the end of the vector.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline void
vector<ItemType, AllocatorType>::push_back(const InitializerListType ilist_)
{
    check_fit(ilist_.size());

    std::copy(ilist_.begin(), ilist_.end(), end());
    add_size(ilist_.size());
}


/**
 **************************************************************************************************
 * \brief       Add elements from another vector to the end of the vector, after the current
 *              last item.
 *
 * \param       otherVector_: Vector containing elements to push back at the end of the vector.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
template<typename OtherAllocatorType>
inline void
vector<ItemType, AllocatorType>::push_back(const vector<ItemType, OtherAllocatorType>& otherVector_)
{
    check_fit(otherVector_.length());

    std::copy(otherVector_.cbegin(), otherVector_.cend(), end());
    add_size(otherVector_.length());
}


/**
 **************************************************************************************************
 * \brief       Remove the last element of the vector.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline void
vector<ItemType, AllocatorType>::pop_back()
{
    if(length() == 0)
    {
        return;
    }

    std::destroy_at(--end());
}


/**
 **************************************************************************************************
 * \brief       Constructs an element at the last position.
 *              This function is often to be favored instead of 'push_back' when building new
 *              items, since it avoids a copy.
 *
 * \param       args: The arguments needed to be passed to the constructor of an element.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
template<typename... Args>
inline void
vector<ItemType, AllocatorType>::emplace_back(Args&&... args_)
{
    check_fit(1);

    end().value() = ItemType(std::forward<Args>(args_)...);

    add_size(1);
}


/**
 **************************************************************************************************
 * \brief       Constructs an element in the middle of the vector, right-shifting items on the
 *              right to fit.
 *
 * \param       position_: Position in vector to insert the element.
 * \param       count_:    Number of elements to insert from the initial offset.
 * \param       args:      The arguments needed to be passed to the constructor of an element.
 *
 * \retval      IteratorType: Position at which the element has been constructed.
 *                            (if multiple elements have been inserted, return position of the last
 *                             inserted element).
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
template<typename... Args>
inline typename vector<ItemType, AllocatorType>::IteratorType
vector<ItemType, AllocatorType>::emplace(IteratorType position_, SizeType count_, Args&&... args_)
{
    if constexpr(vector_safeness == true)
    {
        check_if_valid(position_);
    }

    check_fit(count_);
    add_size(count_);

    std::shift_right(position_, end(), count_);

    for(SizeType i = 0; i < count_; i++)
    {
        position_[i] = ItemType(std::forward<Args>(args_)...);
    }

    return position_ + count_;
}


/**
**************************************************************************************************
* \brief       Constructs an element in the middle of the vector, right-shifting items on the
*              right to fit.
*
* \param       offset_:   Position to insert the element at.
* \param       count_:    Number of elements to insert from the initial offset.
* \param       args:      The arguments needed to be passed to the constructor of an element.
*
* \retval      IteratorType: Position at which the element has been constructed.
*                            (if multiple elements have been inserted, return position of the last
*                             inserted element).
*************************************************************************************************/
template<typename ItemType, typename AllocatorType>
template<typename... Args>
inline typename vector<ItemType, AllocatorType>::IteratorType
vector<ItemType, AllocatorType>::emplace(DifferenceType offset_, SizeType count_, Args&&... args_)
{
    IteratorType position = cbegin() + offset_;

    return emplace(position, count_, std::forward<Args>(args)...);
}


/**
 **************************************************************************************************
 * \brief       Insert an element in the middle of the vector, right-shifting items on the right to
 *              fit.
 *
 * \param       value_:    Element to insert in the vector.
 * \param       position_: Position in vector to insert the element.
 * \param       count_:    Number of elements to insert from the initial offset.
 *                         [defaults : 1]
 *
 * \retval      IteratorType: Position at which the element has been inserted.
 *                            (if multiple elements have been inserted, return position of the last
 *                             inserted element).
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline typename vector<ItemType, AllocatorType>::IteratorType
vector<ItemType, AllocatorType>::insert(const ItemType&    value_,
                                        const IteratorType position_,
                                        SizeType           count_)
{
    if constexpr(vector_safeness == true)
    {
        check_if_valid(position_);
    }

    check_fit(count_);
    add_size(count_);

    std::shift_right(position_, end(), count_);

    for(SizeType i = 0; i < count_; i++)
    {
        position_[i] = value_;
    }

    return position_ + count_;
}


/**
 **************************************************************************************************
 * \brief       Insert an element in the middle of the vector, right-shifting items on the right to
 *              fit.
 *
 * \param       value_:  Element to insert in the vector.
 * \param       offset_: Position to insert the element at.
 * \param       count_:  Number of elements to insert from the initial offset.
 *                       [defaults : 1]
 *
 * \retval      IteratorType: Position at which the element has been inserted.
 *                            (if multiple elements have been inserted, return position of the last
 *                             inserted element).
 *
 * \throws      std::invalid_argument("Invalid insert offset")
 *              Offset was out of bounds.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline typename vector<ItemType, AllocatorType>::IteratorType
vector<ItemType, AllocatorType>::insert(const ItemType& value_,
                                        DifferenceType  offset_,
                                        SizeType        count_)
{
    IteratorType position = cbegin() + offset_;

    return insert(value_, position, count_);
}


/**
 **************************************************************************************************
 * \brief       Insert elements in the middle of the vector from another vector, right-shifting
 *              items on the right to fit.
 *
 * \param       sourceBegin_: Begin iterator from another vector.
 * \param       sourceEnd_:   End iterator from another vector.
 * \param       position_:    Position in vector to start copy-inserting data at.
 *
 * \retval      IteratorType: Position at which the element has been inserted.
 *                            (if multiple elements have been inserted, return position of the last
 *                             inserted element).
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline typename vector<ItemType, AllocatorType>::IteratorType
vector<ItemType, AllocatorType>::insert(const IteratorType sourceBegin_,
                                        const IteratorType sourceEnd_,
                                        const IteratorType position_)
{
    if constexpr(vector_safeness == true)
    {
        check_if_valid(position_);
    }

    SizeType sourceSize = sourceEnd_ - sourceBegin_;

    check_fit(sourceSize);
    add_size(sourceSize);

    std::shift_right(position_, cend(), sourceSize);
    std::copy(sourceBegin_, sourceEnd_, position_);

    return position_ + sourceSize;
}


/**
 **************************************************************************************************
 * \brief       Insert elements in the middle of the vector from another vector, right-shifting
 *              items on the right to fit.
 *
 * \param       sourceBegin_: Begin iterator from another vector.
 * \param       sourceEnd_:   End iterator from another vector.
 * \param       offset_:      Offset in vector to start copy-inserting data at.
 *              [defaults : 0]
 *
 * \retval      IteratorType: Position at which the element has been inserted.
 *                            (if multiple elements have been inserted, return position of the last
 *                             inserted element).
 *
 * \throws      std::invalid_argument("Invalid insert offset")
 *              Offset was out of bounds.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline typename vector<ItemType, AllocatorType>::IteratorType
vector<ItemType, AllocatorType>::insert(const IteratorType sourceBegin_,
                                        const IteratorType sourceEnd_,
                                        DifferenceType     offset_)
{
    if constexpr(vector_safeness)
    {
        if(offset_ > length())
        {
            throw std::invalid_argument("Invalid insert offset");
        }
    }

    IteratorType position = cbegin() + offset_;

    return insert(sourceBegin_, sourceEnd_, position);
}


/**
 **************************************************************************************************
 * \brief       Insert elements in the middle of the vector from an initializer list,
 *              right-shifting items on the right to fit.
 *
 * \param       ilist_:  Initializer list containing element to copy-insert into vector.
 * \param       offset_: Offset in vector to start copy-inserting data at.
 *                       [defaults : 0]
 *
 * \retval      IteratorType: Position at which the element has been inserted.
 *                            (if multiple elements have been inserted, return position of the last
 *                             inserted element).
 *
 * \throws      std::invalid_argument("Invalid insert offset")
 *              Offset was out of bounds.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline typename vector<ItemType, AllocatorType>::IteratorType
vector<ItemType, AllocatorType>::insert(const InitializerListType ilist_, SizeType offset_)
{
    if constexpr(vector_safeness == true)
    {
        if(offset_ > length())
        {
            throw std::invalid_argument("Invalid insert offset");
        }
    }

    IteratorType position = cbegin() + offset_;

    return insert(ilist_.begin(), ilist_.end(), position);
}


/**
 **************************************************************************************************
 * \brief       Replace the element at a specified position with a new element.
 *
 * \param       value_:  Value that will replace element.
 * \param       offset_: Position of the element to replace.
 *                       [defaults : cbegin()]
 *
 * \retval      IteratorType: Position at which the element has been replaced.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline typename vector<ItemType, AllocatorType>::IteratorType
vector<ItemType, AllocatorType>::replace(const ItemType& value_, SizeType offset_)
{
    at(offset_) = value_;

    return begin() + offset_;
}


/**
 **************************************************************************************************
 * \brief       Replace the last element with a new element.
 *
 * \param       value_: Value that will replace element.
 *
 * \retval      IteratorType: Iterator to the element that was replaced.
 *                            (end iterator - 1)
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline typename vector<ItemType, AllocatorType>::IteratorType
vector<ItemType, AllocatorType>::replace_back(const ItemType& value_)
{
    IteratorType position = end() - 1;

    position.value() = value_;
    return position;
}


/**
 **************************************************************************************************
 * \brief       Replace the first element with a new element.
 *
 * \param       value_: Value that will replace element.
 *
 * \retval      IteratorType: Iterator to the element that was replaced.
 *                            (begin iterator)
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline typename vector<ItemType, AllocatorType>::IteratorType
vector<ItemType, AllocatorType>::replace_front(const ItemType& value_)
{
    IteratorType position = begin();

    position.value() = value_;
    return position;
}


/*************************************************************************************************/
/* MEMORY -------------------------------------------------------------------------------------- */
/*************************************************************************************************/


/**
 **************************************************************************************************
 * \brief       Simple accessor, return the capacity (allocated size) of the vector.
 *
 * \retval      SizeType: Elements that can fit in the allocated space.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
[[nodiscard]] inline typename vector<ItemType, AllocatorType>::SizeType
vector<ItemType, AllocatorType>::capacity() const noexcept
{
    return m_capacity;
}


/**
 **************************************************************************************************
 * \brief       Allocate memory for the vector.
 *
 * \param       newCapacity_: Size in elements of the memory to allocate.
 *
 * \note        This function works for shrinking as well as expanding the vector's allocated
 *              memory space.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline void
vector<ItemType, AllocatorType>::reserve(SizeType newCapacity_)
{
    /* Check if resizing is necessary */
    if(newCapacity_ == capacity())
    {
        return;
    }

    /* Allocate a new memory segment */
    vector_constructor(newCapacity_);
}


/**
 **************************************************************************************************
 * \brief       Change amount of elements currently stocked in the vector.
 *
 * \param       newLength_: Size in elements of the vector.
 *
 * \note        This function is different than \ref reserve().
 *              \ref reserve() allocates memory space, but doesn't make changes to  the iterators
 *              or elements (unless shrinking below current vector's length).
 *              resize() changes the amount of elements contained in the vector, and can call
 *              \ref reserve() if in need of more memory.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline void
vector<ItemType, AllocatorType>::resize(SizeType newLength_)
{
    /* Check if reserving memory is necessary */
    if(newLength_ > capacity())
    {
        reserve(newLength_);
    }

    /* Check if freeing some memory is necessary */
    /** \todo */

    /* Resize */
    change_size(newLength_);
}


/**
 **************************************************************************************************
 * \brief       Shrink allocated memory to fit exactly the number of elements currently being
 *              contained in the vector.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline void
vector<ItemType, AllocatorType>::shrink_to_fit()
{
    if(length() == capacity())
    {
        return;
    }

    reserve(length());
}


/*************************************************************************************************/
/* MISC ---------------------------------------------------------------------------------------- */
/*************************************************************************************************/

/**
 **************************************************************************************************
 * \brief       Convert the content of a vector to a string.
 *
 * \retval      A string containing the capacity, the size, and all the elements converted to a
 *              string.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
[[nodiscard]] inline std::string
vector<ItemType, AllocatorType>::to_string() const
{
    std::ostringstream os;
    os << *this;
    return os.str();
}


/*************************************************************************************************/
/* PRIVATE METHODS ----------------------------------------------------------------------------- */
/*************************************************************************************************/

/**
 **************************************************************************************************
 * \brief       Allocates or reallocates memory on the heap. Also resize iterators.
 *
 * \param       size_: Size (in elements) to allocate.
 *
 * \throws      std::bad_alloc: Could not allocate block of memory.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
void
vector<ItemType, AllocatorType>::vector_constructor(SizeType size_)
{
    /* Reallocate block of memory */
    ItemType* tempPtr = AllocatorTraits::allocate(m_allocator, size_);
    ItemType* oldPtr  = begin().ptr();

    /* Move data from old vector memory to new memory */
    std::move(begin(), end(), tempPtr);

    /* Set iterators */
    DifferenceType diff = length();
    m_beginIterator     = IteratorType(static_cast<ItemType*>(tempPtr));
    m_endIterator       = IteratorType(begin() + diff);

    /* Deallocate old memory */
    m_allocator.deallocate(oldPtr, capacity());
    m_capacity = size_;
}


/**
 **************************************************************************************************
 * \brief       Check if the vector is big enough to hold the required extra elements.
 *              If it is not currently big enough, reserve some memory.
 *
 * \param       extraLength_: Numbers of elements to add to the current length.
 *************************************************************************************************/
template<typename ItemType, typename AllocatorType>
inline void
vector<ItemType, AllocatorType>::check_fit(SizeType extraLength_)
{
    if(length() + extraLength_ > capacity())
    {
        reserve(capacity() + step_size());
    }
}


/**
**************************************************************************************************
* \brief       Get and increases the allocation step size.
*              Whenever this function is called, set the allocation step size to 150%, rounded to
*              the upper even number.
*              (Step size of 10 becomes 16).
*
* \retval      The adjusted step size.
*************************************************************************************************/
template<typename ItemType, typename AllocatorType>
typename vector<ItemType, AllocatorType>::SizeType
vector<ItemType, AllocatorType>::step_size() noexcept
{
    return ((m_stepSize += m_stepSize / 2) % 2 == 0) ? m_stepSize : ++m_stepSize;
}

}        // namespace pel

/*************************************************************************************************/
/* END OF FILE --------------------------------------------------------------------------------- */
/*************************************************************************************************/
