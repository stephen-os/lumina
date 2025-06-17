#pragma once

#include <atomic>
#include <type_traits>
#include <utility>
#include <cstdint>
#include <memory>

namespace Lumina
{
    class Referencable
    {
    public:
        virtual ~Referencable() = default;

        void AddRef() const
        {
            m_Refs.fetch_add(1, std::memory_order_relaxed);
        }

        void DecRef() const
        {
            if (m_Refs.fetch_sub(1, std::memory_order_acq_rel) == 1)
            {
                delete this;
            }
        }

        uint32_t GetRefCount() const
        {
            return m_Refs.load(std::memory_order_relaxed);
        }

    protected:
        Referencable() = default;

    private:
        mutable std::atomic<uint32_t> m_Refs{ 0 };
    };

    template<typename T>
    class Ref
    {
    public:
        template<typename... Args>
        static Ref<T> Create(Args&&... args)
        {
            return Ref<T>(new T(std::forward<Args>(args)...));
        }

        Ref(std::nullptr_t) : m_Instance(nullptr) {}

        explicit Ref(T* instance) : m_Instance(instance)
        {
            static_assert(std::is_base_of<Referencable, T>::value, "T must inherit from Referencable");
            AddRef();
        }

        Ref(const Ref<T>& other)
            : m_Instance(other.m_Instance)
        {
            AddRef();
        }

        template<typename U, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
        Ref(const Ref<U>& other)
            : m_Instance(other.Get())
        {
            AddRef();
        }

        Ref(Ref<T>&& other) noexcept
            : m_Instance(other.m_Instance)
        {
            other.m_Instance = nullptr;
        }

        ~Ref()
        {
            Release();
        }

        Ref<T>& operator=(const Ref<T>& other)
        {
            if (this != &other)
            {
                Release();
                m_Instance = other.m_Instance;
                AddRef();
            }
            return *this;
        }
        
        Ref<T>& operator=(Ref<T>&& other) noexcept
        {
            if (this != &other)
            {
                Release();
                m_Instance = other.m_Instance;
                other.m_Instance = nullptr;
            }
            return *this;
        }

        T* operator->() { return m_Instance; }
        const T* operator->() const { return m_Instance; }

        T& operator*() { return *m_Instance; }
        const T& operator*() const { return *m_Instance; }

        T* Get() const { return m_Instance; }

        operator bool() const { return m_Instance != nullptr; }

        bool operator==(const Ref<T>& other) const { return m_Instance == other.m_Instance; }
        bool operator!=(const Ref<T>& other) const { return m_Instance != other.m_Instance; }

    private:
        void AddRef()
        {
            if (m_Instance)
                m_Instance->AddRef();
        }

        void Release()
        {
            if (m_Instance)
            {
                m_Instance->DecRef();
                m_Instance = nullptr;
            }
        }
    private:
        T* m_Instance = nullptr;
    };
}
