#include <siege/platform/win/core/com_client.hpp>
#include <siege/platform/win/core/com_variant.hpp>
#include <siege/platform/win/core/com_property_variant.hpp>
#include <siege/platform/win/core/com_stream_buf.hpp>
#include <siege/platform/win/core/com_xml.hpp>
#include <siege/platform/win/core/com.hpp>
#include <siege/platform/win/core/xcom.hpp>
#include <siege/platform/win/core/module.hpp>
#include <cassert>
#include <set>
#include <comcat.h>

namespace win32::com
{
    static_assert(sizeof(xcom::variant_t) == sizeof(VARIANT));
    static_assert(sizeof(xcom::variant_t::type) == sizeof(VARIANT::vt));
    static_assert(offsetof(xcom::variant_t, xcom::variant_t::data) == offsetof(VARIANT, VARIANT::boolVal));
    static_assert(offsetof(xcom::variant_t, xcom::variant_t::data) == offsetof(VARIANT, VARIANT::byref));
    static_assert(offsetof(xcom::variant_t, xcom::variant_t::data) == offsetof(VARIANT, VARIANT::punkVal));
    static_assert(offsetof(xcom::variant_t, xcom::variant_t::data) == offsetof(VARIANT, VARIANT::pdispVal));
    static_assert(sizeof(xcom::hresult_t) == sizeof(HRESULT));
    static_assert(sizeof(xcom::guid_t) == sizeof(GUID));
    static_assert(sizeof(std::uint32_t) == sizeof(ULONG));
    static_assert(sizeof(std::uint32_t) == sizeof(LCID));
    static_assert(sizeof(char16_t) == sizeof(wchar_t));

	HRESULT init_com()
	{
		thread_local HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		thread_local auto com_handle = std::unique_ptr<HRESULT, void(*)(HRESULT*)>(&result, [](auto*){ CoUninitialize(); });

        assert(std::memcmp(&IID_IUnknown, &xcom::IUnknown::iid, sizeof(GUID)) == 0);

        //ole structured storage
        assert(std::memcmp(&IID_ILockBytes, &xcom::ILockBytes::iid, sizeof(GUID)) == 0);
        assert(std::memcmp(&IID_IStorage, &xcom::IStorage::iid, sizeof(GUID)) == 0);
        assert(std::memcmp(&IID_ISequentialStream, &xcom::ISequentialStream::iid, sizeof(GUID)) == 0);
        assert(std::memcmp(&IID_IStream, &xcom::IStream::iid, sizeof(GUID)) == 0);
        assert(std::memcmp(&IID_IEnumSTATSTG, &xcom::IEnumSTATSTG::iid, sizeof(GUID)) == 0);

        // ole base?
        assert(std::memcmp(&IID_IEnumUnknown, &xcom::IEnumUnknown::iid, sizeof(GUID)) == 0);
        assert(std::memcmp(&IID_IEnumString, &xcom::IEnumString::iid, sizeof(GUID)) == 0);


        //ole automation
        assert(std::memcmp(&IID_IDispatch, &xcom::IDispatch::iid, sizeof(GUID)) == 0);
        assert(std::memcmp(&IID_IEnumVARIANT, &xcom::IEnumVARIANT::iid, sizeof(GUID)) == 0);
        
        //com cat?
        assert(std::memcmp(&IID_IEnumGUID, &xcom::IEnumGUID::iid, sizeof(GUID)) == 0);
        
        //ole eventing?
        assert(std::memcmp(&IID_IEnumConnectionPoints, &xcom::IEnumConnectionPoints::iid, sizeof(GUID)) == 0);
        assert(std::memcmp(&IID_IEnumConnections, &xcom::IEnumConnections::iid, sizeof(GUID)) == 0);
        assert(std::memcmp(&IID_IConnectionPoint, &xcom::IConnectionPoint::iid, sizeof(GUID)) == 0);
        assert(std::memcmp(&IID_IConnectionPointContainer, &xcom::IConnectionPointContainer::iid, sizeof(GUID)) == 0);
      
        return result;
	}

	std::set<void*>& GetHeapAllocations()
        {
            static std::set<void*> allocations;

            return allocations;
        }

        bool ComObject::IsHeapAllocated(void* object, std::size_t size)
        {
            auto& allocations = GetHeapAllocations();

            auto objectValue = reinterpret_cast<std::size_t>(object);
            auto item = std::find_if(allocations.begin(), allocations.end(), [&](auto start) {
                    
                    auto startRange = reinterpret_cast<std::size_t>(start);
                    auto endRange = startRange + size;

                    return objectValue >= startRange && objectValue < endRange;
            });
            return item != allocations.end();
        }

        void* ComObject::operator new(std::size_t count)
        {
            void* result = ::CoTaskMemAlloc(count);
            assert(result);
            GetHeapAllocations().insert(result);

            return result;
        }

        void ComObject::operator delete(void* ptr, std::size_t sz)
        {
            if (IsHeapAllocated(ptr, sz))
            {
                return;
            }

            return ::CoTaskMemFree(ptr);
        }

        ULONG __stdcall ComObject::AddRef() noexcept
        {
            return ++refCount;
        }

        ULONG __stdcall ComObject::Release() noexcept
        {
            if (refCount == 0)
            {
                return 0;
            }

            if (refCount == 1 && !IsHeapAllocated(this, sizeof(*this)))
            {
                return 1;
            }

            --refCount;

            if (refCount == 0)
            {
                delete this;
                return 0;
            }

            return refCount;
        }
}