#include "Vector.h"

#include "../Assert.h"

namespace Lumina
{
	// Creation

	// 2. Constructor with initial capacity
	template<typename T>
	Vector<T>::Vector(size_t capacity) : m_Capacity(capacity)
	{
		if (capacity > 0)
		{
			m_Data = new T[capacity];
		}
	}

	// Rule of Five - Resource Management

	// 1. Destructor
	template<typename T>
	Vector<T>::~Vector()
	{
		delete[] m_Data;
	}

	// 2. Copy constructor
	template<typename T>
	Vector<T>::Vector(const Vector& other) : m_Size(other.m_Size), m_Capacity(other.m_Capacity)
	{
		if (other.m_Data != nullptr)
		{
			// Allocate new memory for the data
			m_Data = new T[m_Capacity];

			// Copy the elements from the other vector
			for (size_t i = 0; i < m_Size; i++)
			{
				m_Data[i] = other.m_Data[i];
			}
		}
	}

	// 3. Copy assignment operator
	template<typename T>
	Vector<T>& Vector<T>::operator=(const Vector& other)
	{
		if (this != &other)
		{
			// Clean up existing resources
			delete[] m_Data;

			// Copy data from other vector
			m_Size = other.m_Size;
			m_Capacity = other.m_Capacity;

			// Allocate new memory if needed
			if (other.m_Data != nullptr)
			{
				// Allocate new memory for the data
				m_Data = new T[m_Capacity];
					
				// Copy the elements from the other vector
				for (size_t i = 0; i < m_Size; i++)
				{
					m_Data[i] = other.m_Data[i];
				}
			}
			else
			{
				m_Data = nullptr;
			}
		}
		return *this;
	}

	// 4. Move constructor
	template<typename T>
	Vector<T>::Vector(Vector&& other) noexcept : m_Data(other.m_Data), m_Size(other.m_Size), m_Capacity(other.m_Capacity)
	{
		other.m_Data = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;
	}

	// 5. Move assignment operator
	template<typename T>
	Vector<T>& Vector<T>::operator=(Vector&& other) noexcept
	{
		if (this != &other)
		{
			// Clean up existing resources
			delete[] m_Data;

			// Transfer ownership of resources
			m_Data = other.m_Data;
			m_Size = other.m_Size;
			m_Capacity = other.m_Capacity;

			// Reset the other vector
			other.m_Data = nullptr;
			other.m_Size = 0;
			other.m_Capacity = 0;
		}
		return *this;
	}

	// Element Access

	// 1. Subscript operator for non-const access
	template<typename T>
	T& Vector<T>::operator[](size_t index)
	{
		LUMINA_ASSERT(index < m_Size, "Vector: Index out of bounds: {0} >= {1}", index, m_Size);
		return m_Data[index];
	}

	// 2. Subscript operator for const access
	template<typename T>
	const T& Vector<T>::operator[](size_t index) const
	{
		LUMINA_ASSERT(index < m_Size, "Vector: Index out of bounds: {0} >= {1}", index, m_Size);
		return m_Data[index];
	}

	// 3. Access element at index with bounds checking
	template<typename T>
	T& Vector<T>::At(size_t index)
	{
		LUMINA_ASSERT(index < m_Size, "Vector: Index out of bounds: {0} >= {1}", index, m_Size);
		return m_Data[index];
	}

	// 4. Access element at index with bounds checking (const)
	template<typename T>
	const T& Vector<T>::At(size_t index) const
	{
		LUMINA_ASSERT(index < m_Size, "Vector: Index out of bounds: {0} >= {1}", index, m_Size);
		return m_Data[index];
	}

	// 5. Access first element
	template<typename T>
	T& Vector<T>::Front()
	{
		LUMINA_ASSERT(m_Size > 0, "Vector: Vector is empty, cannot access front element");
		return m_Data[0];
	}

	// 6. Access first element (const)
	template<typename T>
	const T& Vector<T>::Front() const
	{
		LUMINA_ASSERT(m_Size > 0, "Vector: Vector is empty, cannot access front element");
		return m_Data[0];
	}

	// 7. Access last element
	template<typename T>
	T& Vector<T>::Back()
	{
		LUMINA_ASSERT(m_Size > 0, "Vector: Vector is empty, cannot access back element");
		return m_Data[m_Size - 1];
	}

	// 8. Access last element (const)
	template<typename T>
	const T& Vector<T>::Back() const
	{
		LUMINA_ASSERT(m_Size > 0, "Vector: Vector is empty, cannot access back element");
		return m_Data[m_Size - 1];
	}

	// Push Elements
	
	// 1. Add an element to the end of the vector
	template<typename T>
	void Vector<T>::PushBack(const T& value)
	{
		// Check if we need to grow the vector
		if (m_Size >= m_Capacity)
		{
			Grow();
		}

		m_Data[m_Size++] = value;
	}

	// 2. Add an element to the end of the vector (move semantics)
	template<typename T>
	void Vector<T>::PushBack(T&& value)
	{
		// Check if we need to grow the vector
		if (m_Size >= m_Capacity)
		{
			Grow();
		}

		m_Data[m_Size++] = std::move(value);
	}

	// Emplace Elements

	// 1. Construct and add an element to the end of the vector
	template<typename T>
	template<typename... Args>
	void Vector<T>::EmplaceBack(Args&&... args)
	{
		// Check if we need to grow the vector
		if (m_Size >= m_Capacity)
		{
			Grow();
		}
		// Construct the element in place at the end of the vector
		m_Data[m_Size++] = T(std::forward<Args>(args)...);
	}

	// Pop Elements

	// 1. Remove the last element from the vector
	template<typename T>
	void Vector<T>::PopBack()
	{
		LUMINA_ASSERT(m_Size > 0, "Vector: Cannot pop from an empty vector");
		m_Size--;
	}

	// 2. Remove 'count' elements from the end of the vector
	template<typename T>
	void Vector<T>::PopBack(size_t count)
	{
		LUMINA_ASSERT(count <= m_Size, "Vector: Cannot pop more elements than the current size");
		m_Size -= count;
	}
	
	// 3. Remove the first element from the vector
	template<typename T>
	void Vector<T>::PopFront()
	{
		LUMINA_ASSERT(m_Size > 0, "Vector: Cannot pop from an empty vector");
		
		// Shift all elements to the left
		for (size_t i = 1; i < m_Size; i++)
		{
			m_Data[i - 1] = std::move(m_Data[i]);
		}

		m_Size--;
	}

	// 4. Remove 'count' elements from the front of the vector
	template<typename T>
	void Vector<T>::PopFront(size_t count)
	{
		LUMINA_ASSERT(count <= m_Size, "Vector: Cannot pop more elements than the current size");
		
		// Shift all elements to the left
		for (size_t i = count; i < m_Size; i++)
		{
			m_Data[i - count] = std::move(m_Data[i]);
		}

		m_Size -= count;
	}

	// Erase Elements

	// 1. Remove an element at a specific index
	template<typename T>
	void Vector<T>::Erase(size_t index)
	{
		LUMINA_ASSERT(index < m_Size, "Vector: Index out of bounds: {0} >= {1}", index, m_Size);

		// Shift all elements to the left from the index
		for (size_t i = index + 1; i < m_Size; i++)
		{
			m_Data[i - 1] = std::move(m_Data[i]);
		}

		m_Size--;
	}

	// 2. Remove 'count' elements starting from a specific index
	template<typename T>
	void Vector<T>::Erase(size_t index, size_t count)
	{
		LUMINA_ASSERT(index < m_Size, "Vector: Index out of bounds: {0} >= {1}", index, m_Size);
		LUMINA_ASSERT(count <= m_Size - index, "Vector: Cannot erase more elements than available from index {0}", index);
		
		// Shift all elements to the left from the index + count
		for (size_t i = index + count; i < m_Size; i++)
		{
			m_Data[i - count] = std::move(m_Data[i]);
		}

		m_Size -= count;
	}

	// Capacity Management

	// 1. Check if the vector is empty
	template<typename T>
	bool Vector<T>::Empty() const
	{
		return m_Size == 0;
	}

	// 2. Get the current number of elements in the vector
	template<typename T>
	size_t Vector<T>::Size() const
	{
		return m_Size;
	}

	// 3. Get the current capacity of the vector
	template<typename T>
	size_t Vector<T>::Capacity() const
	{
		return m_Capacity;
	}

	// 4. Clear the vector, removing all elements
	template<typename T>
	void Vector<T>::Clear()
	{
		// Delete the existing data array
		delete[] m_Data;

		// Reset the vector's state
		m_Data = nullptr;
		m_Size = 0;
		m_Capacity = 0;
	}

	// 5. Reserve space for at least 'capacity' elements without changing size
	template<typename T>
	void Vector<T>::Reserve(size_t capacity)
	{
		if (capacity > m_Capacity)
		{
			Reallocate(capacity);
		}
	}

	// 6. Reduce capacity to fit the current size
	template<typename T>
	void Vector<T>::ShrinkToFit()
	{
		if (m_Size < m_Capacity)
		{
			Reallocate(m_Size);
		}
	}

	// 7. Resize the vector to 'size', initializing new elements with default values
	template<typename T>
	void Vector<T>::Resize(size_t size)
	{
		if (newSize > m_Capacity)
		{
			Reallocate(newSize);
		}

		// Initialize new elements with default values
		for (size_t i = m_Size; i < size; i++)
		{
			m_Data[i] = T(); // Default construct the element
		}

		m_Size = size;
	}

	// 8. Resize the vector to 'size', initializing new elements with 'value'
	template<typename T>
	void Vector<T>::Resize(size_t size, const T& value)
	{
		if (size > m_Capacity)
		{
			Reallocate(size);
		}

		// Initialize new elements with the specified value
		for (size_t i = m_Size; i < size; i++)
		{
			m_Data[i] = value;
		}

		m_Size = size;
	}

	// Iterators

	// 1. Get an iterator to the beginning of the vector
	template<typename T>
	T* Vector<T>::begin()
	{
		return m_Data;
	}

	// 2. Get a const iterator to the beginning of the vector
	template<typename T>
	const T* Vector<T>::begin() const
	{
		return m_Data;
	}

	// 3. Get an iterator to the end of the vector
	template<typename T>
	T* Vector<T>::end()
	{
		return m_Data + m_Size;
	}

	// 4. Get a const iterator to the end of the vector
	template<typename T>
	const T* Vector<T>::end() const
	{
		return m_Data + m_Size;
	}

	// Iterators Insert

	// 1. Insert an element at a specific position
	template<typename T>
	T* Vector<T>::Insert(T* position, const T& value)
	{
		LUMINA_ASSERT(position >= m_Data && position <= m_Data + m_Size, "Vector: Position out of bounds");
		size_t index = position - m_Data;
		
		// Check if we need to grow the vector
		if (m_Size >= m_Capacity)
		{
			Grow();
		}

		// Shift elements to the right to make space for the new element
		for (size_t i = m_Size; i > index; i--)
		{
			m_Data[i] = std::move(m_Data[i - 1]);
		}

		// Insert the new element at the specified position
		m_Data[index] = value;
		m_Size++;

		return &m_Data[index];
	}

	// 2. Insert an element at a specific position (move semantics)
	template<typename T>
	T* Vector<T>::Insert(T* position, T&& value)
	{
		LUMINA_ASSERT(position >= m_Data && position <= m_Data + m_Size, "Vector: Position out of bounds");
		size_t index = position - m_Data;

		// Check if we need to grow the vector
		if (m_Size >= m_Capacity)
		{
			Grow();
		}
		
		// Shift elements to the right to make space for the new element
		for (size_t i = m_Size; i > index; i--)
		{
			m_Data[i] = std::move(m_Data[i - 1]);
		}
		
		// Insert the new element at the specified position
		m_Data[index] = std::move(value);
		m_Size++;
		
		return &m_Data[index];
	}

	// 3. Insert multiple elements at a specific position
	template<typename T>
	T* Vector<T>::Insert(T* position, InitList values)
	{
		LUMINA_ASSERT(position >= m_Data && position <= m_Data + m_Size, "Vector: Position out of bounds");
		size_t index = position - m_Data;
		
		// Check if we need to grow the vector
		if (m_Size + values.size() > m_Capacity)
		{
			Grow();
		}
		
		// Shift elements to the right to make space for the new elements
		for (size_t i = m_Size + values.size() - 1; i >= index + values.size(); i--)
		{
			m_Data[i] = std::move(m_Data[i - values.size()]);
		}
		
		// Insert the new elements at the specified position
		size_t i = 0;
		for (const auto& value : values)
		{
			m_Data[index + i++] = value;
		}
		m_Size += values.size();

		return &m_Data[index];
	}

	// Comparison Operators

	// 1. Equality operator
	template<typename T>
	bool Vector<T>::operator==(const Vector& other) const
	{
		if (m_Size != other.m_Size)
			return false;
		
		// Compare each element in the vectors
		for (size_t i = 0; i < m_Size; i++)
		{
			if (!(m_Data[i] == other.m_Data[i]))
				return false;
		}
		
		return true;
	}

	// 2. Inequality operator
	template<typename T>
	bool Vector<T>::operator!=(const Vector& other) const
	{
		return !(*this == other);
	}

	// Private Methods

	// 1. Increase the capacity of the vector when needed
	template<typename T>
	void Vector<T>::Grow()
	{
		size_t newCapacity = m_Capacity == 0 ? 1 : m_Capacity * 2;
		Reallocate(newCapacity);
	}

	// 2. Reallocate memory for the vector with a new capacity
	template<typename T>
	void Vector<T>::Reallocate(size_t capacity)
	{
		// Ensure the new capacity is greater than the current size
		T* newData = new T[capacity];

		// Move existing elements to the new data array
		for (size_t i = 0; i < m_Size; i++)
		{
			newData[i] = std::move(m_Data[i]);
		}

		// Delete the old data array and update the pointer
		delete[] m_Data;

		// Update the vector's data pointer and capacity
		m_Data = newData;
		m_Capacity = capacity;
	}
}