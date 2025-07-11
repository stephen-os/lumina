#pragma once

#include <cstddef>			// for size_t
#include <initializer_list> // for std::initializer_list

// This class is a simple implementation of a dynamic array (vector) in C++.
// This is mainly for my learning purposes and to understand how vectors work under the hood.
// Lumina probably wont use this. 

namespace Lumina
{
	// [24 Bytes] A simple implementation of a dynamic array (vector) in C++.
	template<typename T>
	class Vector
	{
		using InitList = std::initializer_list<T>;		// Type alias for initializer list of T

		// Creation
		Vector() = default;								// 1. Default constructor
		Vector(size_t capacity);						// 2. Constructor with initial capacity
														
		// Rule of Five - Resource Managment			
		~Vector();										// 1. Destructor
		Vector(const Vector& other);					// 2. Copy constructor
		Vector& operator=(const Vector& other);			// 3. Copy assignment operator
		Vector(Vector&& other) noexcept;				// 4. Move constructor
		Vector& operator=(Vector&& other) noexcept;		// 5. Move assignment operator
														
		// Element Access								
		T& operator[](size_t index);					// 1. Subscript operator for non-const access
		const T& operator[](size_t index) const;		// 2. Subscript operator for const access
		T& At(size_t index);							// 3. Access element at index with bounds checking
		const T& At(size_t index) const;				// 4. Access element at index with bounds checking (const)
		T& Front();										// 5. Access first element
		const T& Front() const;							// 6. Access first element (const)
		T& Back();										// 7. Access last element
		const T& Back() const;							// 8. Access last element (const)	

		// Push Elements
		void PushBack(const T& value);					// 1. Add an element to the end of the vector
		void PushBack(T&& value);						// 2. Add an element to the end of the vector (move semantics)
		
		// Emplace Elements
		template<typename... Args>
		void EmplaceBack(Args&&... args);				// 1. Construct and add an element to the end of the vector

		// Pop Elements
		void PopBack();									// 1. Remove the last element from the vector
		void PopBack(size_t count);						// 2. Remove 'count' elements from the end of the vector
		void PopFront();								// 3. Remove the first element from the vector
		void PopFront(size_t count);					// 4. Remove 'count' elements from the front of the vector

		// Erase Elements
		void Erase(size_t index);						// 1. Remove an element at a specific index
		void Erase(size_t index, size_t count);			// 2. Remove 'count' elements starting from a specific index

		// Capacity Management
		bool Empty() const;								// 1. Check if the vector is empty
		size_t Size() const;							// 2. Get the current number of elements in the vector
		size_t Capacity() const;						// 3. Get the current capacity of the vector
		void Clear();									// 4. Clear the vector, removing all elements
		void Reserve(size_t capacity);					// 5. Reserve space for at least 'capacity' elements without changing size
		void ShrinkToFit();								// 6. Reduce capacity to fit the current size
		void Resize(size_t size);						// 7. Resize the vector to 'newSize', initializing new elements with default values
		void Resize(size_t size, const T& value);		// 8. Resize the vector to 'newSize', initializing new elements with 'value'

		// Iterators
		T* begin();					 					// 1. Get an iterator to the beginning of the vector
		const T* begin() const;							// 2. Get a const iterator to the beginning of the vector
		T* end();										// 3. Get an iterator to the end of the vector
		const T* end() const;							// 4. Get a const iterator to the end of the vector

		// Iterators Insert
		T* Insert(T* position, const T& value);			// 1. Insert an element at a specific position
		T* Insert(T* position, T&& value);				// 2. Insert an element at a specific position (move semantics)
		T* Insert(T* position, InitList values);		// 3. Insert multiple elements at a specific position
		
		// Comparison Operators
		bool operator==(const Vector& other) const;		// 1. Equality operator
		bool operator!=(const Vector& other) const;		// 2. Inequality operator

	private:
		void Grow();									// 1. Increase the capacity of the vector when needed	
		void Reallocate(size_t capacity);				// 2. Reallocate the data array to a new capacity

	private:
		T* m_Data = nullptr;							// [8 Bytes] Pointer to the data array
		size_t m_Size = 0;								// [8 Bytes] Current number of elements
		size_t m_Capacity = 0;							// [8 Bytes] Current capacity of the vector
	};
}