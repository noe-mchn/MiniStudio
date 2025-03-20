#pragma once
#include <exception>
#include <initializer_list>
#include <sstream>

namespace KT
{
	template<typename type , size_t size>
	class VectorND
	{
	public:
        using value_type = type;
        using pointer = type*;
        using const_pointer = const type*;
        using reference = type&;
        using const_reference = const type&;
        struct iterator;
        struct const_iterator;
        struct reverse_iterator;
        struct const_reverse_iterator;
        using reverse_iterator = reverse_iterator;
        using const_reverse_iterator = const_reverse_iterator;
        using iterator = iterator;
        using const_iterator = const_iterator;

        VectorND(std::initializer_list<type> list)
        {
            if (list.size() > size)
                throw std::runtime_error("Out of Range");

           KT::Math::Copy(list.begin(), list.end(), m_data.data());
            KT::Math::Fill((m_data.begin() + list.size()), m_data.end(), type());
        }

        VectorND()
        {
           KT::Math::Fill(m_data.begin(), m_data.end(), type());
        }

        VectorND(const VectorND& tab)
        {
            if (tab.m_data.size() != size)
                throw std::out_of_range("size must be equal");
            KT::Math::Copy(tab.m_data.begin(), tab.m_data.end(), m_data.data());
        }

        VectorND& operator=(const VectorND& tab)
        {
            if (m_data.size() != tab.m_data.size())
                throw std::out_of_range("size must be equal");
            if (this != &tab)
               KT::Math::Copy(tab.m_data.begin(), tab.m_data.end(), m_data.data());
            return *this;
        }

        reference operator[](const size_t& idx)
        {
            return m_data[idx];
        }

        const_reference operator[](const size_t& idx) const
        {
            return m_data[idx];
        }

        reference at(const size_t& idx)
        {
            if (idx >= size)
                throw std::out_of_range("Out of Range");
            return m_data[idx];
        }

        const_reference at(const size_t& idx) const
        {
            if (idx >= size)
                throw std::out_of_range("Out of Range");
            return m_data[idx];
        }

        size_t Size()
        {
            return m_data.size();
        }

        const size_t Size() const
        {
            return m_data.size();
        }

        bool Empty()
        {
            return m_data.empty();
        }

        bool Empty() const
        {
            return  m_data.empty();
        }

        pointer data()
        {
            return m_data.data();
        }

        const_pointer data() const
        {
            return m_data.data();
        }

        iterator begin()
        {
            return iterator(data());
        }

        iterator end()
        {
            return iterator(data() + size);
        }

        const_iterator begin() const
        {
            return const_iterator(data());
        }

        const_iterator end() const
        {
            return const_iterator(data() + size);
        }

        reverse_iterator rbegin()
        {
            return reverse_iterator(data() + size - 1);
        }

        reverse_iterator rend()
        {
            return reverse_iterator(data() - 1);
        }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator(data() + size - 1);
        }

        const_reverse_iterator rend() const
        {
            return const_reverse_iterator(data() - 1);
        }

        KT::VectorND<type, size> operator+(const KT::VectorND<type,size>& data)
        {
            KT::VectorND<type, size> result;
            for (size_t i = 0; i < m_data.Size(); ++i)
            {
                result[i] = data[i] + m_data[i];
            }
            return result;
        }

        KT::VectorND<type, size> operator-(const KT::VectorND<type,size>& data)
        {
            KT::VectorND<type, size> result;
            for (size_t i = 0; i < m_data.Size(); ++i)
            {
                result[i] = data[i] - m_data[i];
            }
            return result;
        }

        KT::VectorND<type, size> operator*(const int& idx)
        {
            KT::VectorND<type, size> result;
            for (size_t i = 0; i < m_data.Size(); ++i)
            {
                result[i] =  m_data[i] * idx;
            }
            return result;
        }

        KT::VectorND<type, size> operator/(const int& idx)
        {
            if (idx == 0)
                throw std::runtime_error("cannot divide by 0");
            KT::VectorND<type, size> result;
            for (size_t i = 0; i < m_data.Size(); ++i)
            {
                result[i] =  m_data[i] / idx;
            }
            return result;
        }

        bool operator ==(const KT::VectorND<type, size>& data)
        {
            for (size_t i = 0; i < m_data.Size(); ++i)
            {
                if (data[i] != m_data[i])
                    return false;
            }
            return true;
        }

        bool operator !=(const KT::VectorND<type, size>& data)
        {
            bool result;
            for (size_t i = 0; i < m_data.Size(); ++i)
            {
                if (data[i] == m_data[i])
                    result = false;
                else
                    result = true;
            }
            return result;
        }

        bool operator ==(const KT::VectorND<type, size>& data) const
        {
            for (size_t i = 0; i < m_data.Size(); ++i)
            {
                if (data[i] != m_data[i])
                    return false;
            }
            return true;
        }

        bool operator !=(const KT::VectorND<type, size>& data) const
        {
            bool result;
            for (size_t i = 0; i < m_data.Size(); ++i)
            {
                if (data[i] == m_data[i])
                    result = false;
                else
                    result = true;
            }
            return result;
        }
	private:
        struct iterator {
            using iterator_category = std::random_access_iterator_tag;
            using value_type = type;
            using difference_type = std::ptrdiff_t;
            using pointer = type*;
            using reference = type&;
            iterator(pointer ptr) : m_ptr(ptr) {}
            reference operator*()
            {
                return *m_ptr;
            }
            const reference operator*() const
            {
                return *m_ptr;
            }
            pointer operator->()
            {
                return m_ptr;
            }
            const pointer operator->() const
            {
                return m_ptr;
            }
            iterator& operator++()
            {
                ++m_ptr;
                return *this;
            }
            iterator& operator--()
            {
                --m_ptr;
                return *this;
            }
            iterator operator+(difference_type n) const
            {
                return iterator(m_ptr + n);
            }
            iterator operator-(difference_type n) const
            {
                return iterator(m_ptr - n);
            }
            difference_type operator-(const iterator& other) const
            {
                return m_ptr - other.m_ptr;
            }
            difference_type operator+(const iterator& other) const
            {
                return m_ptr + other.m_ptr;
            }
            bool operator==(const iterator& other) const
            {
                return m_ptr == other.m_ptr;
            }
            bool operator!=(const iterator& other) const
            {
                return m_ptr != other.m_ptr;
            }
            bool operator<(const iterator& other) const
            {
                return m_ptr < other.m_ptr;
            }
            bool operator>(const iterator& other) const
            {
                return m_ptr > other.m_ptr;
            }
            bool operator<=(const iterator& other) const
            {
                return m_ptr <= other.m_ptr;
            }
            bool operator>=(const iterator& other) const
            {
                return m_ptr >= other.m_ptr;
            }
        private:
            pointer m_ptr;
        };
        struct const_iterator {
            using iterator_category = std::random_access_iterator_tag;
            using value_type = const type;
            using difference_type = std::ptrdiff_t;
            using pointer = const type*;
            using reference = const type&;
            const_iterator(pointer ptr) : m_ptr(ptr) {}
            reference operator*()
            {
                return *m_ptr;
            }
            const reference operator*() const
            {
                return *m_ptr;
            }
            pointer operator->()
            {
                return m_ptr;
            }
            const pointer operator->() const
            {
                return m_ptr;
            }
            const_iterator& operator++()
            {
                ++m_ptr;
                return *this;
            }
            const_iterator& operator--()
            {
                --m_ptr;
                return *this;
            }
            const_iterator operator+(difference_type n) const
            {
                return iterator(m_ptr + n);
            }
            const_iterator operator-(difference_type n) const
            {
                return const_iterator(m_ptr - n);
            }
            difference_type operator-(const const_iterator& other) const
            {
                return m_ptr - other.m_ptr;
            }
            difference_type operator+(const const_iterator& other) const
            {
                return m_ptr + other.m_ptr;
            }
            bool operator==(const const_iterator& other) const
            {
                return m_ptr == other.m_ptr;
            }
            bool operator!=(const const_iterator& other) const
            {
                return m_ptr != other.m_ptr;
            }
            bool operator<(const const_iterator& other) const
            {
                return m_ptr < other.m_ptr;
            }
            bool operator>(const const_iterator& other) const
            {
                return m_ptr > other.m_ptr;
            }
            bool operator<=(const const_iterator& other) const
            {
                return m_ptr <= other.m_ptr;
            }
            bool operator>=(const const_iterator& other) const
            {
                return m_ptr >= other.m_ptr;
            }
        private:
            pointer m_ptr;
        };
        struct reverse_iterator {
            using iterator_category = std::random_access_iterator_tag;
            using value_type = type;
            using difference_type = std::ptrdiff_t;
            using pointer = type*;
            using reference = type&;
            reverse_iterator(pointer ptr) : m_ptr(ptr) {}
            reference operator*()
            {
                return *m_ptr;
            }
            const reference operator*() const
            {
                return *m_ptr;
            }
            pointer operator->()
            {
                return m_ptr;
            }
            const pointer operator->() const
            {
                return m_ptr;
            }
            reverse_iterator& operator++()
            {
                --m_ptr;
                return *this;
            }
            reverse_iterator& operator--()
            {
                ++m_ptr;
                return *this;
            }
            reverse_iterator operator+(difference_type n) const
            {
                return reverse_iterator(m_ptr - n);
            }
            reverse_iterator operator-(difference_type n) const
            {
                return reverse_iterator(m_ptr + n);
            }
            difference_type operator-(const reverse_iterator& other) const
            {
                return m_ptr + other.m_ptr;
            }
            difference_type operator+(const reverse_iterator& other) const
            {
                return m_ptr - other.m_ptr;
            }
            bool operator==(const reverse_iterator& other) const
            {
                return m_ptr == other.m_ptr;
            }
            bool operator!=(const reverse_iterator& other) const
            {
                return m_ptr != other.m_ptr;
            }
            bool operator<(const reverse_iterator& other) const
            {
                return m_ptr > other.m_ptr;
            }
            bool operator>(const reverse_iterator& other) const
            {
                return m_ptr < other.m_ptr;
            }
            bool operator<=(const reverse_iterator& other) const
            {
                return m_ptr >= other.m_ptr;
            }
            bool operator>=(const reverse_iterator& other) const
            {
                return m_ptr <= other.m_ptr;
            }
        private:
            pointer m_ptr;
        };
        struct const_reverse_iterator {
            using iterator_category = std::random_access_iterator_tag;
            using value_type = const type;
            using difference_type = std::ptrdiff_t;
            using pointer = const type*;
            using reference = const type&;
            const_reverse_iterator(pointer ptr) : m_ptr(ptr) {}
            reference operator*()
            {
                return *m_ptr;
            }
            const reference operator*() const
            {
                return *m_ptr;
            }
            pointer operator->()
            {
                return m_ptr;
            }
            const pointer operator->() const
            {
                return m_ptr;
            }
            const_reverse_iterator& operator++()
            {
                --m_ptr;
                return *this;
            }
            const_reverse_iterator& operator--()
            {
                ++m_ptr;
                return *this;
            }
            const_reverse_iterator operator+(difference_type n) const
            {
                return const_reverse_iterator(m_ptr - n);
            }
            const_reverse_iterator operator-(difference_type n) const
            {
                return const_reverse_iterator(m_ptr + n);
            }
            difference_type operator-(const const_reverse_iterator& other) const
            {
                return m_ptr + other.m_ptr;
            }
            difference_type operator+(const const_reverse_iterator& other) const
            {
                return m_ptr - other.m_ptr;
            }
            bool operator==(const const_reverse_iterator& other) const
            {
                return m_ptr == other.m_ptr;
            }
            bool operator!=(const const_reverse_iterator& other) const
            {
                return m_ptr != other.m_ptr;
            }
            bool operator<(const const_reverse_iterator& other) const
            {
                return m_ptr > other.m_ptr;
            }
            bool operator>(const const_reverse_iterator& other) const
            {
                return m_ptr < other.m_ptr;
            }
            bool operator<=(const const_reverse_iterator& other) const
            {
                return m_ptr >= other.m_ptr;
            }
            bool operator>=(const const_reverse_iterator& other) const
            {
                return m_ptr <= other.m_ptr;
            }
        private:
            pointer m_ptr;
        };
        std::array<type, size> m_data;
	};
}


template<typename type , size_t size>
std::ostream& operator<<(std::ostream& os, const KT::VectorND<type, size>& tab)
{
    if (tab.Size() == 0)
        return os;
    for (auto i = 0; i < tab.Size(); ++i)
        os << "(" << tab[i] << ")" << std::endl;
    return os;
}


#pragma once

/*****************************************************************//**
 * @file   KT_Math.h
 * @brief  This file contains all the function stl like
 *
 * @author Kroktur
 * @date   February 2025
 *********************************************************************/
 /**
  * @brief Namespace of my library
  */
namespace KT
{
    /**
    * @brief Namespace of my Math function
    */
    namespace Math
    {
        /**
         * @brief Fill a container with value
         * @tparam Iterator begin
         * @tparam Iterator End
         * @tparam value_type
         * @param Iterator
         * @param Iterator
         * @param value_type
         */
        template<typename input, typename outpout, typename value>
        void Fill(input begin, outpout end, const value& val)
        {
            if (begin == end)
                return;
            for (auto it = begin; it != end; ++it)
            {
                *it = val;
            }
        }

        /**
         * @brief Copy a range of data in a dest
         * @tparam Iterator begin
         * @tparam Iterator End
         * @tparam Iterator Destination
         * @param Iterator
         * @param Iterator
         * @param Iterator
         */
        template<typename input, typename output, typename dest>
        void Copy(input begin, output end, dest val)
        {
            if (begin == end)
                return;
            auto myval = val;
            size_t i = 0;
            for (auto it = begin; it != end; ++it)
            {
                *myval = *it;
                ++myval;
                ++i;
            }
        }

        /**
         * @brief swap element
         * @tparam value_type
         * @param value_type lhs
         * @param value_type rhs
         */
        template<typename type>
        void Swap(type& lhs, type& rhs)
        {
            auto type = lhs;
            lhs = rhs;
            rhs = lhs;
        }

        /**
         * @brief find an iterator by value
         * @tparam Iteator
         * @tparam Value_type
         * @param begin
         * @param end
         * @param val
         * @return Iterator
         */
        template<typename Iteator, typename Value_type>
        Iteator Find(Iteator begin, Iteator end, const Value_type& val)
        {
            for (auto it = begin; it != end; ++it)
            {
                if (*it == val)
                    return it;
            }
            return end;
        }
    }

}